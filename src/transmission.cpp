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
#include <boost/tokenizer.hpp>

#include "transmission.hpp"

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vg = vle::graph;


namespace model {

    Transmission::Transmission(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
         mRate = vv::toDouble(events.get("rate"));
         mPrefix = vv::toString(events.get("prefix"));
    }

    Transmission::~Transmission()
    {
    }
    
    typedef boost::tokenizer < boost::char_separator < char > > tokenizer;
    boost::char_separator<char> sep("_");

    vd::Time Transmission::init(const vd::Time& /*time*/)
    {

        mPhase = INIT;
        mPortIndex = -1;
        return 0;
    }

    void Transmission::output(const vd::Time& time,
                        vd::ExternalEventList& output) const
    {
        if (mPhase == INFECTING ) {
            // Send event to the good port
            std::string portName = mPorts[mPortIndex];
            if (getModel().existOutputPort(portName)){
                vd::ExternalEvent* event = new vd::ExternalEvent( portName );
                event << vd::attribute("infection_evt", 1);
                output.addEvent(event);
            }
        }
    }

    vd::Time Transmission::timeAdvance() const
    {
        switch (mPhase) {
        case INIT:
            return vd::Time(0);
        case IDLE:
            return vd::Time::infinity;
        case INFECTING:
            if (mPortIndex < mPorts.size())
                return vd::Time(mInfectionOffsets[mPortIndex]);
            else
                return vd::Time::infinity;
        }
        return vd::Time::infinity;
    }

    void Transmission::internalTransition(const vd::Time& /*time*/)
    {
        if (mPhase == INIT) {
            mPhase = IDLE;
        }else if (mPhase == INFECTING) {
            if (mPortIndex < mPorts.size())
                mPortIndex++;
        }
    }

    void Transmission::externalTransition(const vd::ExternalEventList& events,
                                    const vd::Time& /*time*/)
    {
        bool cleaned = false;
        for (unsigned int i = 0; i < events.size(); i++) {

            if (events[i]->onPort("state")) {
                if (events[i]->getBooleanAttributeValue("state")) {
                    mPhase = INFECTING;
                    randomizeOrder();
                    randomizeInfectionTime();
                    mPortIndex = 0;
                } else
                    mPhase = IDLE;
            }

            if (events[i]->onPort("control")) {
                if (events[i]->getStringAttributeValue("type") == "clean")
                    cleaned = true;
                if (events[i]->getStringAttributeValue("type") == "move_restriction") {
                    double ratio = events[i]->getDoubleAttributeValue("ratio");
                    std::cout<<"car control ratio: "<<ratio<<" on? "<<events[i]->getBooleanAttributeValue("on")<<std::endl;
                    if (events[i]->getBooleanAttributeValue("on"))
                        mRate = mRate - mRate * ratio;
                    else
                        mRate = mRate / (1. - ratio);
                }
            }
        }
        if (mPhase == INFECTING) {
            randomizeInfectionTime();
        }
        if (cleaned)
            mPhase = IDLE;
    }

    void Transmission::confluentTransitions(const vd::Time& time,
                                    const vd::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    void Transmission::request(const vd::RequestEvent& /*event*/,
                        const vd::Time& /*time*/,
                        vd::ExternalEventList& /*output*/) const
    {
    }

    vv::Value* Transmission::observation(const vd::ObservationEvent& event) const
    {
        if (event.onPort("state")) {
            switch (mPhase) {
            case INIT:
                return new vv::String("INIT");
            case IDLE:
                return new vv::String("IDLE");
            case INFECTING:
                if (mPortIndex > 0 && mPortIndex <= mPorts.size()) {
                    std::stringstream portStr;
                    portStr << mPorts[mPortIndex-1];
                    return new vv::String("INFECTING : " + portStr.str() + " is now infected !");
                }else
                    return new vv::String("INFECTING : wait");
            default :
                return new vv::String("?");
            }
        }else if (event.onPort("rate")) {
            return new vv::Double(mRate);
        } else
            return 0;
    }
    
    void Transmission::randomizeOrder() 
    {
        mPorts = getNeighboursNames();
        for (unsigned int i = 0; i < mPorts.size(); i++) {
            int idx =  rand().getInt(0, mPorts.size() - 1);
            std::string tmp = mPorts[i];
            mPorts[i] = mPorts[idx];
            mPorts[idx] = tmp;
        }
    }
    
    void Transmission::randomizeInfectionTime() 
    {
        mInfectionOffsets = new double[mPorts.size()];
        // Generate dates
        double infectionDates[mPorts.size()];
        std::cout<<"call exponential with rate= "<<mRate<<std::endl;
        for (unsigned int i = 0; i < mPorts.size(); i++)
            infectionDates[i] = rand().exponential(mRate);
        // Sort this dates
        for (unsigned int i = 0; i < mPorts.size(); i++) {
            for (unsigned int j = i + 1; j < mPorts.size(); j++) {
                if (infectionDates[i] > infectionDates[j]) {
                    double tmp = infectionDates[i];
                    infectionDates[i] = infectionDates[j];
                    infectionDates[j] = tmp;
                }
            }
        }
        // Calculate the offsets
        mInfectionOffsets[0] = infectionDates[0];
        for (unsigned int i = 1; i < mPorts.size(); i++)
            mInfectionOffsets[i] = infectionDates[i] - infectionDates[i - 1];
    }

    std::vector<std::string> Transmission::getNeighboursNames()
    {
        std::vector<std::string> vect;
        vle::graph::ConnectionList map = getModel().getOutputPortList();
        vle::graph::ConnectionList::const_iterator end = map.end();
        for (vle::graph::ConnectionList::const_iterator it = map.begin(); it != end; ++it)
        {
            std::string port = it->first;
	    tokenizer tok(port, sep);
            if (*(tok.begin()) == mPrefix){
                vect.push_back(port);
            }
        }
        return vect;
    }

} // namespace vle example

DECLARE_DYNAMICS(model::Transmission);
