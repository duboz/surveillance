/**
 * @file 
 * @author Virginie Baticle <baticle@polytech.unice.fr>
 *         Jeremy Gabriele  <gabriele@polytech.unice.fr>
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2009 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/tokenizer.hpp>
#include <vle/value.hpp>
#include <vle/devs.hpp>
#include "passive_collector.hpp"

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

  PassiveCollector::PassiveCollector(const vd::DynamicsInit& init, 
                                  const vd::InitEventList& events)
      : vd::Dynamics(init, events)
  {
    mProbabilityRightSR = vv::toDouble(events.get("probabilityRightSR"));
    mProbabilityRightI = vv::toDouble(events.get("probabilityRightI"));
    mProbabilityDeclaration = vv::toDouble(events.get("probabilityDeclaration"));
    mNbOutbreaks = 0;
  }

  PassiveCollector::~PassiveCollector()
  {
  }

  vd::Time PassiveCollector::init(const vd::Time&  /*time*/)
  {
      mPhase = IDLE;
      return 0;
  }

  void PassiveCollector::output(const vd::Time& time,
                      vd::ExternalEventList& output) const
  {
      if ((mPhase == SEND_RESULT)and 
          (getModel().existOutputPort("control"))) {
          vv::Map* nodeObservations = vv::Map::create();
          typedef std::map<std::string, std::pair<std::string, vd::Time> >::const_iterator mapit;
          bool detection = false;
          for (mapit it = mapResult.begin(); it != mapResult.end(); it++) {
              if ((it->second.first == "I") and (time == it->second.second)) {
                  nodeObservations->addString(it->first, it->second.first);
                  detection = true;
              }
          }
          if (detection) {
              vd::ExternalEvent * ev = new vd::ExternalEvent ("control");
              ev << vd::attribute ("infectedNodes", nodeObservations);
              output.addEvent (ev);
          } else {
              delete nodeObservations;
          }
      }
     
      if ((mPhase == SEND_RESULT)and 
          (getModel().existOutputPort("info_center"))) {
          vd::ExternalEvent * ev = new vd::ExternalEvent ("info_center");
          vv::Map* nodeObservations = vv::Map::create();
          typedef std::map<std::string, std::pair<std::string, vd::Time> >::const_iterator mapit;
          for (mapit it = mapResult.begin(); it != mapResult.end(); it++) {
              if (time == it->second.second)
                nodeObservations->addString(it->first, it->second.first);
          }
          ev << vd::attribute ("nodesStates", nodeObservations);
          output.addEvent (ev);
      }

  }

  vd::Time PassiveCollector::timeAdvance() const
  {
      if (mPhase == IDLE) {
          return vd::Time::infinity;
      }
      else if (mPhase == SEND_RESULT) {
        return 0;
      }
      else 
          return vd::Time::infinity;
  }

  void PassiveCollector::internalTransition(const vd::Time& /*time*/)
  {
      if (mPhase == SEND_RESULT) {
          mPhase = IDLE;
      }
  }

  void PassiveCollector::externalTransition(
                                  const vd::ExternalEventList& event,
                                  const vd::Time&  time)
  {
          //mapResult.clear();
          for (vd::ExternalEventList::const_iterator it = event.begin();
                      it != event.end(); ++it) {

              double randValue = rand().getDouble();
              if ((*it) -> getPortName() == "status") {
                  std::string value = 
                        (*it)-> getStringAttributeValue ("value");
                  std::string modelName = 
	                      (*it)-> getStringAttributeValue ("modelName");

                  if (value == "S" || value == "R") {
                      // probability of a wrong interpretation
                      if (randValue > mProbabilityRightSR) {
                        value = "I";
                      }
                      mapResult[modelName] = 
                          std::pair<std::string, vd::Time> (value, time);
                  
                  } else if (value == "I") {
                      mNbOutbreaks++;
                      if (randValue > mProbabilityRightI) {
                        value = "S";
                      }
                      randValue = rand().getDouble();
                      if (randValue < mProbabilityDeclaration)
                      mapResult[modelName] = 
                          std::pair<std::string, vd::Time> (value, time);

                  }
              }
             
          }
          mPhase = SEND_RESULT;
  }

  void PassiveCollector::confluentTransitions(const vd::Time& time,
                                    const vd::ExternalEventList& events)
  {
          externalTransition(events, time);
          internalTransition(time);
  }

  void PassiveCollector::request(const vd::RequestEvent& /*event*/,
                       const vd::Time& /*time*/,
                       vd::ExternalEventList& /*output*/) const
  {
  }

  vv::Value* PassiveCollector::observation(
                       const vd::ObservationEvent& event) const
  {
    vv::Map* tmpResult = new vv::Map();
    std::map<std::string, std::pair<std::string, vd::Time> >::const_iterator it;
    int Ss=0;
    int Is=0;
    int Rs=0;
    for ( it = mapResult.begin(); it != mapResult.end(); ++it ) {
        tmpResult->addString(it->first, (it->second).first);
        if ((it->second).first == "S") Ss++; 
        else if ((it->second).first == "I") Is++; 
        else if ((it->second).first == "R") Rs++;
    }

    if (event.onPort("outbreakReport")) {

        typedef boost::tokenizer < boost::char_separator < char > > tokenizer;
        boost::char_separator<char> sep("-");

        vv::Set* repport = new vv::Set();
        for ( it = mapResult.begin(); it != mapResult.end(); ++it ) {
            if ((it->second).second >= (event.getTime() - vd::Time(7)) and ((it->second).first=="I")) {
               //std::cout<<"pour le noeud "<<it->first<<", que se passe-t-il?"<<std::endl;
                std::string node = it->first;
                tokenizer tok(node, sep);
                boost::tokenizer<boost::char_separator < char > >::const_iterator i = tok.begin();
                i++;
                //std::cout<<*tok.begin()<<" et "<<*i<<std::endl;
                repport->addInt(std::atoi(i->c_str()));
            }
        }
        return repport;
    }

    if (event.onPort("nbIs")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(Is);
    }
    
    if (event.onPort("nb_outbreaks")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(mNbOutbreaks);
    }

    if (event.onPort("epidemic_width")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(mapResult.size());
    }


    if (event.onPort("nbSs")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(Ss);
    }

    if (event.onPort("nbRs")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(Rs);
    }

    return tmpResult;
  }

  void PassiveCollector::finish()
  {
  }

} // namespace vle example

DECLARE_DYNAMICS(model::PassiveCollector);
