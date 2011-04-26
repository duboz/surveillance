/**
 * @file vle/share/template/package/src/controler.cpp
 * @author The VLE Development Team
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

#include <vle/devs/Dynamics.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;
using namespace vle;
namespace model {

class controler : public devs::Dynamics
    {
private:
    enum Phase {INIT, IDLE, CONTROL};
    Phase m_phase;
    std::map<std::string, std::string> m_nodeStates;

public:
    controler(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
    }

    virtual ~controler()
    {
    }

    virtual devs::Time init(
        const devs::Time& /* time */)
    {   
        m_phase = INIT;
        return devs::Time(0.);
    }

    virtual void output(
        const devs::Time& /* time */,
        devs::ExternalEventList& output ) const
    {
        if (m_phase == CONTROL) {
            typedef std::map<std::string, std::string>::const_iterator mapit;
            for (mapit it = m_nodeStates.begin(); it != m_nodeStates.end(); it++) {
                if (it->second == "I") {
                    vd::ExternalEvent * ev = new vd::ExternalEvent (it->first);
                    ev << vd::attribute ("type", buildString("clean"));
                    output.addEvent (ev);
                }
            }
        }
    }

    virtual devs::Time timeAdvance() const
    {
        if (m_phase == CONTROL)
            return devs::Time(0.);
        if (m_phase == INIT)
            return devs::Time(0.);

        return devs::Time::infinity;
    }

    virtual void internalTransition(
        const devs::Time& /* time */)
    {
        if (m_phase == INIT)
            m_phase = IDLE;
        else if (m_phase == CONTROL)
            m_phase = IDLE;
    }

    virtual void externalTransition(
        const devs::ExternalEventList&  event ,
        const devs::Time& /* time */)
    {
        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            if ((*it)->onPort("surveillance")) {
                value::Map infNodes = (*it)->getMapAttributeValue("infectedNodes");
                for (vv::MapValue::const_iterator node = infNodes.begin();
                     node != infNodes.end(); node++) {
                    m_nodeStates[node->first] = node->second->toString().value();
                }
                if (m_phase == IDLE) 
                    m_phase = CONTROL;
                else {
                    vle::utils::InternalError error("\nControler must recieve only one surveillance repport in this version\n");
                    throw error;
                }
            }
        }
    }

    virtual void confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    virtual void request(
        const devs::RequestEvent& /* event */,
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const
    {
    }

    virtual value::Value* observation(
        const devs::ObservationEvent& /* event */) const
    {
        return 0;
    }

    virtual void finish()
    {
    }
};

} // namespace model

DECLARE_NAMED_DYNAMICS(controler, model::controler)
