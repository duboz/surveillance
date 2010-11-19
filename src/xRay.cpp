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

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

  XRay::XRay(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
  {
    mProbabilityRightSR = vv::toDouble(events.get("probabilityRightSR"));
    mProbabilityRightI = vv::toDouble(events.get("probabilityRightI"));
  }

  XRay::~XRay()
  {
  }

  vd::Time XRay::init(const vd::Time& time)
  {
      mPhase = SEND;
      mObservationTimeStep = 0.5;
      mCurrentTime = vd::Time(time);
      mLastRequestTime = vd::Time(time);
      return 0;
  }

  void XRay::output(const vd::Time& /*time*/,
                      vd::ExternalEventList& output) const
  {
      if (mPhase == SEND) {
          vd::RequestEvent * request = new vd::RequestEvent ("status?");
          request << vd::attribute ("name", std::string ("status"));
          output.addEvent (request);
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
      default:
          std::cout << "bad state in XRay model !";
      }
  }

  void XRay::externalTransition(
                                  const vd::ExternalEventList& event,
                                  const vd::Time& time)
  {
      
      if (mPhase == RECEIVE) {
          for (vd::ExternalEventList::const_iterator it = event.begin();
                      it != event.end(); ++it) {

              vle::utils::Rand r;
              r.seed((uint32_t)12345);
              double randValue = r.getDouble();
              if ((*it) -> getStringAttributeValue ("name") == "status") {

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
                       const vd::ObservationEvent& /*event*/) const
  {
      
    vv::Map* tmpResult = new vv::Map();
    std::map<std::string, std::string>::const_iterator it;

    for ( it = mapResult.begin(); it != mapResult.end(); ++it ) {
      tmpResult->addString(it->first, it->second);
    }

    return tmpResult;
  }

  void XRay::finish()
  {
  }

} // namespace vle example

DECLARE_NAMED_DYNAMICS(x_ray, model::XRay)
