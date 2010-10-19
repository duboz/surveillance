/**
 * @file vle/share/template/package/src/simple.cpp
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

#include <vle/value.hpp>
#include <vle/devs.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

class Sirs : public vd::Dynamics
{
private:
    enum PHASE {S,I,R};
    PHASE m_phase;
    double m_infectious_period;
        
public:
    Sirs(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
    }

    virtual ~Sirs()
    {
    }

    virtual vd::Time init(const vd::Time& /*time*/)
    {
	m_phase = S;
	m_infectious_period = 10;
        return vd::Time::infinity;
    }

    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& /*output*/) const
    {
    }

    virtual vd::Time timeAdvance() const
    {
	if(m_phase == I)
	    return vd::Time(m_infectious_period);
        return vd::Time::infinity;
    }

    virtual void internalTransition(const vd::Time& /*time*/)
    {
	switch (m_phase){
	case R:
	    m_phase = I;
	    break;
	case I:
	    m_phase = R;
	    break;
	default:
	    std::cout << "bad state in SIRS model !";
	}
    }

    virtual void externalTransition(const vd::ExternalEventList& event,
                                    const vd::Time& /*time*/)
    {
	//if (event.getPortName() == "status?")
	    m_phase;
	if (m_phase == S)
	    m_phase = I;
    }

    virtual void confluentTransitions(const vd::Time& time,
                                      const vd::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    virtual void request(const vd::RequestEvent& /*event*/,
                         const vd::Time& /*time*/,
                         vd::ExternalEventList& /*output*/) const
    {
    }

    virtual vv::Value* observation(const vd::ObservationEvent& /*event*/) const
    {
        return 0;
    }

    virtual void finish()
    {
    }
};

} // namespace vle example

DECLARE_NAMED_DYNAMICS(sirs, model::Sirs)
