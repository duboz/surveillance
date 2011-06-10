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

#ifndef CONTROLER_HPP
#define CONTROLER_HPP

#include <vle/value.hpp>
#include <vle/devs.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

using namespace vle;

namespace model {
class controler : public devs::Dynamics
    {
protected:
    enum Phase {INIT, IDLE, CONTROL, DISABLED};
    Phase m_phase;
    std::map<std::string, std::string> m_nodeStates;
    typedef std::pair<double, std::vector<std::string> > Intervention;
    typedef std::vector<Intervention> InterventionPlan;
    InterventionPlan m_interventions;
    double m_delay;
    double m_current_time;
    bool m_disabled;
    long m_nbInterventions;

public:
    controler(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events);
    
    virtual ~controler();
    
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

        //Generic functions for controllers.
        void loadSpatialParameters(const devs::InitEventList& events);
};
}

#endif
