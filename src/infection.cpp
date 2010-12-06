/**
* @author Thomas DE BONA and Virginie BATICLE
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

#include "infection.hpp"
extern "C"{
    #include <unistd.h>
}

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

    Infection::Infection(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
        mInfectiousPeriod = vv::toDouble(events.get("infectiousPeriod"));
        mSecuredPeriod = vv::toDouble(events.get("securedPeriod"));
        mAutoInfect = vv::toDouble(events.get("autoInfect"));
    }

    Infection::~Infection()
    {
    }

    vd::Time Infection::init(const vd::Time& /*time*/)
    {
        mPhase = INIT;
        return 0;
    }


    void Infection::output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const
    {
        if (mPhase == S || mPhase == SI) {
            vd::ExternalEvent* event = new vd::ExternalEvent("state");
            event << vd::attribute("state", true);
            output.addEvent(event);
        }else if (mPhase == I) {
            vd::ExternalEvent* event = new vd::ExternalEvent("state");
            event << vd::attribute("state", false);     
            output.addEvent(event);
        }
    }

    vd::Time Infection::timeAdvance() const
    {
        switch (mPhase) {
        case INIT :
            return vd::Time(0);
        case S:
            if (mAutoInfect > 0)
                return vd::Time(mAutoInfect);
            else
                return vd::Time::infinity;
        case I :
            return vd::Time(mInfectiousPeriod);
        case R :
            return vd::Time(
                mSecuredPeriod - (mCurrentTime - mCleaningTime));
        case SI :
            return 0;
        }
        return vd::Time::infinity;
    }

    void Infection::internalTransition(const vd::Time& time)
    {
        switch (mPhase) {
        case INIT :
            mPhase = S;
            break;
        case SI:
        case S:
            mPhase = I;
            break;
        case I:
            mPhase = R;
            mCleaningTime = time;
            mCurrentTime = time;
            break;
        case R:
            mPhase = S;
            mAutoInfect = 0;
            break;
        }
    }

    void Infection::externalTransition(const vd::ExternalEventList& /*event*/,
                                    const vd::Time& time)
    {
        if (mPhase == S)
            mPhase = SI;
        
        mCurrentTime = time;
    }

    void Infection::confluentTransitions(const vd::Time& time,
                                    const vd::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
	}

    vv::Value* Infection::observation(const vd::ObservationEvent& event) const
    {
        if (event.onPort("state")) {
            switch (mPhase) {
           case S :
                return new vv::Integer(1);
           case SI :
                return new vv::Integer(2);
           case I :
                return new vv::Integer(2);
           case R :
                return new vv::Integer(3);
           default:
                return new vv::Integer(0);
          }
        }else
            return 0;
    }

    void Infection::finish()
    {
    }

    void Infection::request(const vd::RequestEvent& event,
                         const vd::Time& /*time*/,
                         vd::ExternalEventList& output) const
    {
        if (event.getPortName() == "status?") {
            std::string responseValue = std::string("");
            switch (mPhase) {
            case INIT:
            case S:
                responseValue = std::string("S");
                break;
            case SI:
            case I:
                responseValue = std::string("I");
                break;

            case R:
                responseValue = std::string("R");
                break;

            }
            vd::ExternalEvent * response = 
                new vd::ExternalEvent (event.getStringAttributeValue("modelName"));
            response << vd::attribute ("value", responseValue);
            response << vd::attribute ("modelName", 
                                          getModel().getParent()->getName());
            output.addEvent (response);
        }
    }

} // namespace vle example

DECLARE_NAMED_DYNAMICS(infection, model::Infection)
