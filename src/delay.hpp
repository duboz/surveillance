/**
* @author Bruno Bonté
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

#ifndef Delay_HPP
#define Delay_HPP

#include <vle/value.hpp>
#include <vle/devs.hpp>
#include <vle/devs/DynamicsDbg.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

using namespace vle;

namespace model {
class Delay : public devs::Dynamics
    {
protected:
    enum Phase {INIT, IDLE, WAITING, DISABLED};
    Phase m_phase;
    typedef std::pair<vd::Time, std::vector<vv::Map*> > evBag;
    typedef std::vector<evBag> evBagPlan;
    evBagPlan m_evBags;
    double m_delay;
    double m_current_time;
    double m_agreggationThreshold;

public:
    Delay(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events);
    
    virtual ~Delay();
    
    virtual devs::Time init(
        const devs::Time& time);
    
    virtual void output(
        const devs::Time& /* time */,
        devs::ExternalEventList& output ) const;
    
    virtual devs::Time timeAdvance() const;
    
    virtual void internalTransition(
        const devs::Time& time);
    
    virtual void externalTransition(
        const devs::ExternalEventList&  event ,
        const devs::Time& time);
    
    virtual void confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& events);
    
    virtual void request(
        const devs::RequestEvent& /* event */,
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const;
    
        virtual value::Value* observation(
        const devs::ObservationEvent& /* event */) const;
        
        virtual void finish();
};
}

#endif
