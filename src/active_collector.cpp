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
#include <active_collector.hpp>
#include <vle/devs/DynamicsDbg.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

  ActiveCollector::ActiveCollector(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
  {
    mProbabilityRightSR = vv::toDouble(events.get("probabilityRightSR"));
    mProbabilityRightI = vv::toDouble(events.get("probabilityRightI"));
    mObservationTimeStep =  vv::toDouble(events.get("timeStep"));
    if (events.exist("asleep"))
        mAsleep = vv::toBoolean(events.get("asleep"));
    else
        mAsleep = false;
    if (events.exist("obsAggregationThreshold")) { 
        mObsAggregationThreshold =  vv::toDouble(events.get("obsAggregationThreshold"));
    }
    else
        mObsAggregationThreshold = 0.000001;

    if (events.exist("R_INIT") and 
        (vv::toBoolean(events.get("R_INIT")))) {
        mNbModel =vv::toInteger(events.get("graphInfo_number"));
        mPrefix =vv::toString(events.get("graphInfo_prefix"));
    } else {
        mNbModel = events.getMap("graphInfo").getInt("number");
        mPrefix = events.getMap("graphInfo").getString("prefix");
    }
    mSampleSize = /*boost::lexical_cast<int>*/ (vv::toDouble(events.get("echProp")) * mNbModel);
    //std::cout<<"smplesize= "<<mSampleSize<<"\n";
    mPrevalence=0.; mIncidence=0.;
  }

  ActiveCollector::~ActiveCollector()
  {
  }

  vd::Time ActiveCollector::init(const vd::Time& time)
  {
      mPhase = INIT;
      mCurrentTime = vd::Time(time);
      mLastRequestTime = vd::Time(time);
      computeSampleProp();
      if (mAsleep)
          return vd::Time(0);
      else
          return vd::Time::infinity;
  }

  void ActiveCollector::output(const vd::Time& /*time*/,
                      vd::ExternalEventList& output) const
  {
      if (mPhase == SEND) {
          vd::RequestEvent * request = new vd::RequestEvent ("status?");
          request << vd::attribute ("modelName", std::string (getModelName()));
          output.addEvent (request);
      }

      if ((mPhase == RECEIVE)and 
          (getModel().existOutputPort("observations"))) { 
          vd::ExternalEvent * ev = new vd::ExternalEvent ("observations");
          ev << vd::attribute ("value", buildDouble(mPrevalence));
          output.addEvent (ev);
      }
 
      if ((mPhase == RECEIVE)and 
          (getModel().existOutputPort("control"))) { 
          vd::ExternalEvent * ev = new vd::ExternalEvent ("control");
          vv::Map* nodeObservations = vv::Map::create();
          typedef std::map<std::string, std::string>::const_iterator mapit;
          for (mapit it = mapResult.begin(); it != mapResult.end(); it++) {
              if (it->second == "I"){
                  nodeObservations->addString(it->first, it->second);
              /*std::cout<<"active coll: "<<getModelName()<<"told to control: "
                  <<it->first<<" is "<<it->second<<std::endl;*/
              }
          }
          ev << vd::attribute ("infectedNodes", nodeObservations);
          output.addEvent (ev);
      }

 
      if ((mPhase == RECEIVE)and 
          (getModel().existOutputPort("info_center"))) { 
          vd::ExternalEvent * ev = new vd::ExternalEvent ("info_center");
          vv::Map* nodeObservations = vv::Map::create();
          typedef std::map<std::string, std::string>::const_iterator mapit;
          for (mapit it = mapResult.begin(); it != mapResult.end(); it++) {
              nodeObservations->addString(it->first, it->second);
              /*std::cout<<"active coll: "<<getModelName()<<"told to info_center: "
                  <<it->first<<" is "<<it->second<<std::endl;*/
          }
          ev << vd::attribute ("nodesStates", nodeObservations);
          output.addEvent (ev);
      }

      if ((mPhase == CHG_CONNECTION or mPhase == INIT) 
          and getModel().existOutputPort("connectTo")) {
        connectToNodes(output); 
      }
  }

  vd::Time ActiveCollector::timeAdvance() const
  {
      if (mPhase == SEND) {
          return 0;
      }
      
      if (mPhase == INIT) {
          return 0;
      }

      if ((mPhase == CHG_CONNECTION) or (mPhase == WAIT_CONNECTION)) {
          return 0;
      }

      if (mPhase == RECEIVE) {
          return vd::Time(mObsAggregationThreshold/2);
      }

      if (mPhase == IDLE) {
          return vd::Time::infinity;
      }

      return mObservationTimeStep - (mCurrentTime.getValue() 
             - mLastRequestTime.getValue());
  }

  void ActiveCollector::internalTransition(const vd::Time& time)
  {
      switch (mPhase) {
      case SEND:
          mPhase = RECEIVE;
          mLastRequestTime = vd::Time(time);
          mCurrentTime = vd::Time(time);
          break;
      case RECEIVE:
          mPhase = SEND_OBS;
          mCurrentTime = vd::Time(time);
          break;
      case SEND_OBS:
          mPhase = CHG_CONNECTION; 
          computeSampleProp();
          mCurrentTime = vd::Time(time);
          break;
      case CHG_CONNECTION:
          mPhase = WAIT_CONNECTION;
          mCurrentTime = vd::Time(time);
          break;
      case WAIT_CONNECTION:
          mPhase = SEND;
          mCurrentTime = vd::Time(time);
          break;
      case INIT:
          mPhase = SEND;
          mCurrentTime = vd::Time(time);
          break;
      default:
          std::cout << "bad state in ActiveCollector model !";
      }
  }

  void ActiveCollector::externalTransition(
                                  const vd::ExternalEventList& events,
                                  const vd::Time& time)
  {
      if (mPhase == INIT) {
        for (unsigned int i = 0; i < events.size(); i++) {
            if (events[i]->onPort("start")) {
                mPhase = SEND;
                mAsleep = false;
            }
            if (events[i]->onPort("stop")) {
                mPhase = INIT;
                mAsleep = true;
            }
        }
      }
      else if (mPhase == RECEIVE) {
        receiveData(events, time);
      }

  }

  void ActiveCollector::confluentTransitions(const vd::Time& time,
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

  void ActiveCollector::request(const vd::RequestEvent& /*event*/,
                       const vd::Time& /*time*/,
                       vd::ExternalEventList& /*output*/) const
  {
  }

  vv::Value* ActiveCollector::observation(
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

  void ActiveCollector::finish()
  {
  }

  void ActiveCollector::computeSampleProp()
  {
  }
  
  //Generic functions for active collectors
  
  void ActiveCollector::receiveData(const vd::ExternalEventList& event,
                   const vd::Time& time)
  {
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
              /*std::cout<<"active coll: "<<getModelName()<<"indeed recieved that: "
                  <<modelName<<" is "<<value<<std::endl;*/

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
  int nbInfected = 0;
  std::map<std::string, std::string>::iterator node;
  for (node=mapResult.begin(); node!=mapResult.end();node++){
      if (node->second == "I")
          nbInfected++;
  }
  double tempPrev = ((double) nbInfected) / mSampleSize;
  mIncidence = (tempPrev - mPrevalence)/mObservationTimeStep;
  mPrevalence = tempPrev;
  }

} // namespace vle example
