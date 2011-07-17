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
#include <controller.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;
using namespace vle;
namespace model {
    
    controler::controler(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
        m_delay = vv::toDouble(events.get("controlDelay"));
        m_disabled = vv::toBoolean(events.get("disabled"));
        m_nbInterventions = 0;
        //loadSpatialParameters(events);
    }

    controler::~controler()
    {
    }

    devs::Time controler::init(
        const devs::Time& time)
    {   
        m_current_time = time.getValue();
        m_phase = INIT;
        if (m_disabled)
            m_phase = DISABLED;
        return 0;
    }

    void controler::output(
        const devs::Time& /* time */,
        devs::ExternalEventList& output ) const
    {
        if (m_phase == CONTROL) {
            typedef std::vector<std::string>::const_iterator NodeIterator;
    
            vv::Map* nodeObservations = vv::Map::create();
            for (NodeIterator it = m_interventions.begin()->subSystems.begin(); 
                 it !=  m_interventions.begin()->subSystems.end(); it++) {
                vd::ExternalEvent * ev = new vd::ExternalEvent (*it);
                if (m_interventions.begin()->type == "clean") {
                    nodeObservations->addString(*it, "R");
                    ev << vd::attribute ("type", buildString("clean"));
                }
                else if (m_interventions.begin()->type == "move_restriction") {
                    ev << vd::attribute ("type", buildString("move_restriction"));
                    ev << vd::attribute ("ratio", buildDouble(m_interventions.begin()->ratio));
                    ev << vd::attribute ("on", buildBoolean(m_interventions.begin()->on));
                }
                output.addEvent (ev);

            }
            if (getModel().existOutputPort("info_center")) {
                vd::ExternalEvent * evInfo = new vd::ExternalEvent ("info_center");
                evInfo << vd::attribute("nodesStates", nodeObservations);
                output.addEvent(evInfo);
            } else {
                delete nodeObservations;
            }
        }
    }

    devs::Time controler::timeAdvance() const
    {
        if (m_phase == CONTROL)
            return devs::Time(m_interventions.begin()->date - m_current_time);
        if (m_phase == INIT)
            return 0;

        return devs::Time::infinity;
    }

   void controler::internalTransition(
        const devs::Time& time)
    {
        if (m_phase == INIT)
            m_phase = IDLE;
        else if (m_phase == CONTROL) {
            for (std::vector<std::string>::iterator node = m_interventions.begin()->subSystems.begin();
                 node != m_interventions.begin()->subSystems.end(); node++) {
                m_nodeStates[*node] = "R";
                m_nbInterventions++;
            }
            m_interventions.erase(m_interventions.begin());
            if (m_interventions.empty())
                m_phase = IDLE;
        }
        m_current_time = time.getValue();
    }

    void controler::externalTransition(
        const devs::ExternalEventList&  event ,
        const devs::Time& time)
    {
        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            if ((*it)->onPort("surveillance")) {
                Intervention newIntervention;
                value::Map infNodes = (*it)->getMapAttributeValue("infectedNodes");
                for (vv::MapValue::const_iterator node = infNodes.begin();
                     node != infNodes.end(); node++) {
                    m_nodeStates[node->first] = node->second->toString().value();
                    newIntervention.subSystems.push_back(node->first);
                }
                newIntervention.date = time.getValue() + m_delay;
                newIntervention.type = "clean";
                m_interventions.push_back(newIntervention);
                if (m_phase == IDLE)
                    m_phase = CONTROL;
            }
           if ((*it)->onPort("decision")) {
                Intervention newIntervention;
                value::Map infNodes = (*it)->getMapAttributeValue("controledNodes");
                for (vv::MapValue::const_iterator node = infNodes.begin();
                     node != infNodes.end(); node++) {
                    m_nodeStates[node->first] = node->second->toString().value();
                    newIntervention.subSystems.push_back(node->first);
                }
                newIntervention.date = time.getValue() + m_delay;
                newIntervention.type = (*it)->getStringAttributeValue("type");
                if (newIntervention.type == "move_restriction") {
                    newIntervention.ratio = (*it)->getDoubleAttributeValue("ratio");
                    newIntervention.on = (*it)->getBooleanAttributeValue("on");
                }
                m_interventions.push_back(newIntervention);
                if (m_phase == IDLE)
                    m_phase = CONTROL;
            }
        }
        m_current_time = time.getValue();
    }

    void controler::confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    void controler::request(
        const devs::RequestEvent& /* event */,
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const
    {
    }

    value::Value* controler::observation(
        const devs::ObservationEvent& /* event */) const
    {
        return buildInteger(m_nbInterventions);
    }

    void controler::finish()
    {
    }
    
    void controler::loadSpatialParameters(const devs::InitEventList& /*events*/)
    {
    }

} // namespace model

DECLARE_NAMED_DYNAMICS(controler, model::controler)
