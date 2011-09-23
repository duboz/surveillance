/**
 * @file vle/share/template/package/src/multi_trigger.cpp
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

class multi_trigger : public devs::Dynamics
{
private:
    enum Phase {INIT, IDLE, TRIGGER};
    Phase m_phase;
    double m_init_control_rate;
    double m_starting_time;
    int m_nbSubModel;
    std::string m_subModelPrefix;

public:
    multi_trigger(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
        m_init_control_rate = vv::toSetValue(events.get("control_steps"))->getDouble(1);
        m_nbSubModel = events.getMap("graphInfo").getInt("number");
        m_subModelPrefix = events.getMap("graphInfo").getString("prefix");
        m_phase = INIT;
    }

    virtual ~multi_trigger()
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

    virtual devs::Time timeAdvance() const
    {
        if (m_phase == TRIGGER)
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
        const devs::ExternalEventList& /* event */,
        const devs::Time& /* time */)
    {
        m_phase = TRIGGER;
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

DECLARE_DYNAMICS(model::multi_trigger)
