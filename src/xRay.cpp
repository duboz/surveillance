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
#include "xRay.hpp"
#include <vle/devs/DynamicsDbg.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

  XRay::XRay(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
  {
    mProbabilityRightSR = vv::toDouble(events.get("probabilityRightSR"));
    mProbabilityRightI = vv::toDouble(events.get("probabilityRightI"));
    mNbModel = events.getMap("graphInfo").getInt("number");
    mPrefix = events.getMap("graphInfo").getString("prefix");
    mObservationTimeStep =  vv::toDouble(events.get("timeStep"));
    mSampleSize = /*boost::lexical_cast<int>*/ (vv::toDouble(events.get("echProp")) * mNbModel);
    //std::cout<<"smplesize= "<<mSampleSize<<"\n";
    mPrevalence=0.; mIncidence=0.;
      }

  XRay::~XRay()
  {
  }

  vd::Time XRay::init(const vd::Time& time)
  {
      mPhase = INIT;
      mCurrentTime = vd::Time(time);
      mLastRequestTime = vd::Time(time);
      return 0;
  }

  void XRay::output(const vd::Time& /*time*/,
                      vd::ExternalEventList& output) const
  {
      if (mPhase == SEND) {
          vd::RequestEvent * request = new vd::RequestEvent ("status?");
          request << vd::attribute ("modelName", std::string (getModelName()));
          output.addEvent (request);
      }

      if ((mPhase == RECEIVE or mPhase == INIT) 
          and getModel().existOutputPort("connectTo")) {
          vd::ExternalEvent* connectionRequest = 
                new vd::ExternalEvent("connectTo");
          vv::Set linkTo;
          std::vector <int> sample;
          int i=0;
          while (sample.size()<mNbModel) { 
              sample.push_back(i);
              i++;
          }
          for (int j = 0; j < sample.size(); j++) {
              int k = rand().getInt(0,sample.size()-1);
              int temp = sample[j];
              sample[j]=sample[k];
              sample[k]=temp;
          }
          while (sample.size()>mSampleSize)
              sample.pop_back();

          for (std::vector<int>::iterator i = sample.begin(); i!=sample.end(); ++i) {
            std::string vetName = 
              mPrefix + "-" + boost::lexical_cast<std::string>(*i);
            linkTo.addString(vetName);
          }
          connectionRequest << vd::attribute("modelName", 
                                             std::string (getModelName()));
          connectionRequest << vd::attribute("linkTo", linkTo);
          output.addEvent(connectionRequest);
      }
  }

  vd::Time XRay::timeAdvance() const
  {
      if (mPhase == SEND) {
          return 0;
      }
      return mObservationTimeStep - (mCurrentTime.getValue() 
             - mLastRequestTime.getValue());
  }

  void XRay::internalTransition(const vd::Time& time)
  {
      switch (mPhase) {
      case SEND:
          mPhase = RECEIVE;
          mLastRequestTime = vd::Time(time);
          mCurrentTime = vd::Time(time);
          break;
      case RECEIVE:
          mPhase = SEND;
          mCurrentTime = vd::Time(time);
          break;
      case INIT:
          mPhase = SEND;
          mCurrentTime = vd::Time(time);
          break;
      default:
          std::cout << "bad state in XRay model !";
      }
  }

  void XRay::externalTransition(
                                  const vd::ExternalEventList& event,
                                  const vd::Time& time)
  {
      
      if (mPhase == RECEIVE) {
          if (!mapResult.empty())
              mapResult.clear();
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
                  } else if (value == "I") {
                      if (randValue > mProbabilityRightI) {
                        value = "S";
                      }
                  }
                  mapResult[modelName] = value;
              }
          }
          mCurrentTime = vd::Time(time);
      }
      int nbInfected = 0;
      //int nbNonInfected = 0;
      std::map<std::string, std::string>::iterator node;
      for (node=mapResult.begin(); node!=mapResult.end();node++){
          if (node->second == "I")
              nbInfected++;
          //else
          //    nbNonInfected++;
      }
      //std::cout<<"xray recieve: "<<nbNonInfected<<" non infecteds and: "<< nbInfected
      //    <<" infecteds at time: "<<time.getValue()<<"\n";
      double tempPrev = ((double) nbInfected) / mSampleSize;
      //std::cout<<"tempPrev: "<<tempPrev<<"\n";
      mIncidence = (tempPrev - mPrevalence)/mObservationTimeStep;
      mPrevalence = tempPrev;
  }

  void XRay::confluentTransitions(const vd::Time& time,
                                    const vd::ExternalEventList& events)
  {
      if (mPhase == SEND) {
          internalTransition(time);
          externalTransition(events, time);
      } else {
          externalTransition(events, time);
          internalTransition(time);
      }
  }

  void XRay::request(const vd::RequestEvent& /*event*/,
                       const vd::Time& /*time*/,
                       vd::ExternalEventList& /*output*/) const
  {
  }

  vv::Value* XRay::observation(
                       const vd::ObservationEvent& event) const
  {
    
    if (event.onPort("xrayview")){ 
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

  void XRay::finish()
  {
  }

} // namespace vle example

DECLARE_NAMED_DYNAMICS_DBG(x_ray, model::XRay)
