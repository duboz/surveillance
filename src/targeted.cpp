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

#include <vle/value.hpp>
#include <vle/devs.hpp>
#include "targeted.hpp"
#include <vle/devs/DynamicsDbg.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

  Targeted::Targeted(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : model::ActiveCollector(init, events)
  {
        mControlRadius = vv::toDouble(events.get("targetedSurveillanceRadius"));
        std::vector<double> positions= vv::toTuple(events.get("nodes_positions"));
        mPrefix = vv::toString(events.get("graphInfo_prefix"));
        int nbNodes = positions.size()/2;
        double nodePositions[nbNodes][2];
        int c = 0;
        for (int node = 0; node < nbNodes; node++) {
            nodePositions[node][1] = positions[c];
            c++;
            nodePositions[node][2] = positions[c];
            c++;
        }
        for (int node = 0; node < nbNodes; node++) {
             std::string vetName = 
                 mPrefix + "-" + boost::lexical_cast<std::string>(node);
             std::vector<std::string> neighbors;
             //std::cout<<"key: "<<vetName<<std::endl;
             mNodeNeighbors[vetName] = neighbors;
             for (int node2 = 0; node2 < nbNodes; node2++) {
                 double dist = std::sqrt(std::pow((nodePositions[node][1] - nodePositions[node2][1]),2)
                                         + std::pow((nodePositions[node][2] - nodePositions[node2][2]),2));
                 if (dist <= mControlRadius) {
                std::string vetName2 = 
                    mPrefix + "-" + boost::lexical_cast<std::string>(node2);
                mNodeNeighbors[vetName].push_back(vetName2);
                 }
            }
        }
  }

  Targeted::~Targeted()
  {
  }
  
  vd::Time Targeted::init(const vd::Time& time)
  {
      mPhase = IDLE;
      mCurrentTime = vd::Time(time);
      mLastRequestTime = vd::Time(time);
      mObservationTimeStep = vd::Time::infinity;
      return vd::Time::infinity;
  }
  
  void Targeted::externalTransition(
                                  const vd::ExternalEventList& event,
                                  const vd::Time& time) 
  {
      if (mPhase == RECEIVE) {
        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
          if ((*it)->onPort("status"))
              receiveData(event, time);
          if ((*it)->onPort("newInfections"))
        vle::utils::ModellingError::ModellingError("Une approximation pour simplifier le code \na eu une mauvaise conséquence dans la classe targeted.cpp:\n Une surveillance de réaction a été oubliée..");
        }
     }
     else if (mPhase == SEND_OBS) {
        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            if ((*it)->onPort("status"))
                vle::utils::ModellingError::ModellingError("This should not happen in this phase (in targeted.cpp)");
            if((*it)->onPort("newInfections")) {
                mNewInfectedNodes.clear();
                vv::Map infNodes = (*it)->getMapAttributeValue("infectedNodes");
                for (vv::MapValue::const_iterator node = infNodes.begin();
                     node != infNodes.end(); node++) {
                    mNewInfectedNodes.push_back(node->first);
                }
            }
            mPhase = SEND;
        }
      }
  }


  vv::Value* Targeted::observation(
                       const vd::ObservationEvent& event) const
  {
    
    if (event.onPort("targetedview")){ 
        vv::Map* tmpResult = new vv::Map();
        std::map<std::string, std::string>::const_iterator it;

        for ( it = mapResult.begin(); it != mapResult.end(); ++it ) {
          tmpResult->addString(it->first, it->second);
        }
        return tmpResult;
    }
    if (event.onPort("prevalence")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildDouble(mPrevalence);
    }
    if (event.onPort("incidence")){
        //std::cout<<"incidence is "<<mIncidence<<"\n";
        return buildDouble(mIncidence);
    }
    else return 0;

  }

  //Generic functions for active surveillance
  void Targeted::connectToNodes(vd::ExternalEventList& output) const 
  {
    typedef std::vector<std::string>::const_iterator NodeIterator;
    for (NodeIterator it = mNewInfectedNodes.begin(); 
         it !=  mNewInfectedNodes.end(); it++) {
        std::string spotedNode;
        spotedNode = *it;
        std::vector<std::string> nodesToInspect = mNodeNeighbors.at(spotedNode);
        vd::ExternalEvent* connectionRequest = new vd::ExternalEvent("connectTo");
        vv::Set linkTo;
        for (NodeIterator nodeToInspect = nodesToInspect.begin(); 
           nodeToInspect != nodesToInspect.end(); nodeToInspect++) {
           linkTo.addString(*nodeToInspect);
        }
          connectionRequest << vd::attribute("modelName", 
                                             std::string (getModelName()));
          connectionRequest << vd::attribute("linkTo", linkTo);
          output.addEvent(connectionRequest);
    }
  }

} // namespace vle example

DECLARE_NAMED_DYNAMICS_DBG(targeted, model::Targeted)
