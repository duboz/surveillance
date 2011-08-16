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
        if (events.exist("restockingPeriod"))
            mRestockingPeriod = vv::toDouble(events.get("restockingPeriod"));
        else 
            mRestockingPeriod = mSecuredPeriod;
        mAutoInfect = vv::toDouble(events.get("autoInfect"));
        if (events.exist("constPeriods"))
            mConstPeriods = vv::toBoolean(events.get("constPeriods"));
        else 
            mConstPeriods = true;
    }

    Infection::~Infection()
    {
    }

    vd::Time Infection::init(const vd::Time& /*time*/)
    {
        mPhase = INIT;
        //Need post initialisation in order to wait for the vertexExecutive
        //to draw the connections with the passive_collecctor..
        return vd::Time(0.000001);
    }


    void Infection::output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const
    {
        if (mPhase == SI) {
            vd::ExternalEvent* event = new vd::ExternalEvent("state");
            event << vd::attribute("state", true);
            event << vd::attribute ("value", std::string("I"));
            event << vd::attribute ("modelName", 
                                       getModel().getParent()->getName());
            output.addEvent(event);
        } else if (mPhase == I) {
            vd::ExternalEvent* event = new vd::ExternalEvent("state");
            event << vd::attribute("state", false);
            event << vd::attribute ("value",std::string("R"));
            event << vd::attribute ("modelName", 
                                       getModel().getParent()->getName());
            output.addEvent(event);
        } else if (mPhase == R) {
            vd::ExternalEvent* event = new vd::ExternalEvent("state");
            event << vd::attribute("state", false);
            event << vd::attribute ("value", std::string("S"));
            event << vd::attribute ("modelName", 
                                       getModel().getParent()->getName());
            output.addEvent(event);
        } else if (mPhase == INIT) {
            vd::ExternalEvent* event = new vd::ExternalEvent("state");
            event << vd::attribute("state", false);
            event << vd::attribute ("value", std::string("S"));
            event << vd::attribute ("modelName", 
                                       getModel().getParent()->getName());
            output.addEvent(event);
        }

    }

    vd::Time Infection::timeAdvance() const
    {
        switch (mPhase) {
        case INIT :
            return vd::Time(0.);
        case S:
            if (mAutoInfect > 0)
                return vd::Time(mAutoInfect);
            else
                return vd::Time::infinity;
        case I :
            return vd::Time(mInfectiousTimeLeft);
        case R :
            return vd::Time(mSecuredTimeLeft);
        case SECURED :
            return vd::Time(mSecuredTimeLeft);
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
            mPhase = I;
            break;
        case S:
            mPhase = SI;
            if (mConstPeriods)
                mInfectiousTimeLeft  = mInfectiousPeriod;
            else
                mInfectiousTimeLeft = rand().exponential(1/mInfectiousPeriod);
            mAutoInfect = 0;
            break;
        case I:
            mPhase = R;
             if (mConstPeriods)
                 mSecuredTimeLeft  = mRestockingPeriod;
            else
                mSecuredTimeLeft = rand().exponential(1/mRestockingPeriod);
            break;
        case R:
            mPhase = S;
            break;
        case SECURED:
            mPhase = S;
            break;
        }
            mCurrentTime = time;
    }

    void Infection::externalTransition(const vd::ExternalEventList& event,
                                    const vd::Time& time)
    {
        if (mPhase == I)
            mInfectiousTimeLeft = mInfectiousTimeLeft - (time - mCurrentTime);
        if (mPhase == R)
            mSecuredTimeLeft = mSecuredTimeLeft - (time - mCurrentTime);

        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            if ((*it)->onPort("infection")) {
                if (!(*it)->existAttributeValue("infection_evt"))
                    throw vle::utils::ArgError(
                    "Bad connection of vertex.\nOnly accepted ports in Vertex class are status? and control");
                if (mPhase == S) {
                    mPhase = SI;
                    if (mConstPeriods)
                        mInfectiousTimeLeft  = mInfectiousPeriod;
                    else
                        mInfectiousTimeLeft = rand().exponential(1/mInfectiousPeriod);
                }
            }
            else if ((*it)->onPort("control")) {
                if ((*it)->getStringAttributeValue("type") == "clean") {
                if ((mPhase == I) or (mPhase == SI)) {
                    mPhase = SECURED;
                    if (mConstPeriods)
                        mSecuredTimeLeft  = mSecuredPeriod;
                    else
                        mSecuredTimeLeft = rand().exponential(1/mSecuredPeriod);
                }
                }
            }
        }
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
           case SECURED :
                return new vv::Integer(4);

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
                responseValue = std::string("I");
                break;
            case I:
                responseValue = std::string("I");
                break;
            case R:
                responseValue = std::string("R");
                break;
            case SECURED:
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

DECLARE_DYNAMICS(model::Infection);
