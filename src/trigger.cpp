/**
 * @file vle/share/template/package/src/Trigger.cpp
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

class Trigger : public devs::Dynamics
{
private:
    enum Phase {INIT, IDLE, TRIGGER, OFF};
    Phase m_phase;
    double m_init_control_rate;
    double m_starting_time;
    int m_nbSubModel;
    int m_nulPrev;
    int m_max_nulPrev;
    std::string m_subModelPrefix;

public:
    Trigger(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
        m_init_control_rate = vv::toTuple(events.get("ratios"))[0];
        m_nbSubModel = events.getMap("graphInfo").getInt("number");
        m_subModelPrefix = events.getMap("graphInfo").getString("prefix");
        m_phase = INIT;
        m_nulPrev = 0;
        m_max_nulPrev = events.getInt("nb_obs_min");
    }

    virtual ~Trigger()
    {
    }

    virtual devs::Time init(
        const devs::Time& /* time */)
    {
        return devs::Time::infinity;
    }

    virtual void output(
        const devs::Time& /*time*/,
        devs::ExternalEventList& output) const
    { 
        if (m_phase == TRIGGER) {
            vd::ExternalEvent * ev = new vd::ExternalEvent ("control");
            vv::Map* nodes = vv::Map::create();
            for (int num = 0; num < m_nbSubModel; num ++) {
            std::string vetName = 
            m_subModelPrefix + "-" + boost::lexical_cast<std::string>(num);
            nodes->addString(vetName, "to_control");
            }
            ev << vd::attribute ("controledNodes", nodes);
            ev << vd::attribute ("type", buildString("move_restriction"));
            ev << vd::attribute ("ratio", vv::Double::create(m_init_control_rate));
            ev << vd::attribute ("on", vv::Boolean::create(true));
            output.addEvent (ev);
        }
        else if (m_phase == OFF) {
            vd::ExternalEvent * ev = new vd::ExternalEvent ("stop");
            output.addEvent (ev);
        }
    }

    virtual devs::Time timeAdvance() const
    {
        if ((m_phase == TRIGGER) or (m_phase == OFF))
            return devs::Time(0); 
        else 
            return devs::Time::infinity;
    }

    virtual void internalTransition(
        const devs::Time& /* time */)
    {
        m_phase = IDLE;
    }

    virtual void externalTransition(
        const devs::ExternalEventList& event,
        const devs::Time& /* time */)
    {
        if (m_phase == INIT)
            m_phase = TRIGGER;
        else if(m_phase == IDLE)
        {
            for (devs::ExternalEventList::const_iterator it = event.begin();
                    it != event.end(); ++it) {
                if ((*it)->onPort("continue")) {
                    double prev =(*it)->getDoubleAttributeValue("value");
                    if (prev >0)
                        m_nulPrev = 0;
                    else
                        m_nulPrev++;
                }
            }
            if (m_nulPrev > m_max_nulPrev)
                m_phase = OFF;
        }
    }

    void confluentTransitions(
        const devs::Time&  time,
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

DECLARE_DYNAMICS(model::Trigger)
