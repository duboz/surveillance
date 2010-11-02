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

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

class Infection : public vd::Dynamics
{
        
public:
    Infection(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
        mInfectiousPeriod = vv::toDouble(events.get("infectiousPeriod"));
        mAutoInfect = vv::toDouble(events.get("autoInfect"));
    }

    virtual ~Infection()
    {
    }

    virtual vd::Time init(const vd::Time& /*time*/)
    {
        mPhase = INIT;
        return 0;
    }

    /**
    *Envoie message infecté : transmission
    *Envoie message recovered : arrete d'infecter les gens
    */
    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const
    {
        if (mPhase == S) {
            vd::ExternalEvent* event = new vd::ExternalEvent("infect");
            event << vd::attribute("infecting", true);
            output.addEvent(event);
        }else if (mPhase == I) {
            vd::ExternalEvent* event = new vd::ExternalEvent("infect");
            event << vd::attribute("infecting", false);     
            output.addEvent(event);
        }
    }

    virtual vd::Time timeAdvance() const
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
            return vd::Time::infinity;
        }
    }

    virtual void internalTransition(const vd::Time& /*time*/)
    {
        switch (mPhase) {
        case INIT :
            mPhase = S;
            break;
        case S:
            mPhase = I;
            break;
        case I:
            mPhase = R;
            break;
        case R:
            break;
        }
    }

    virtual void externalTransition(const vd::ExternalEventList& /*event*/,
                                    const vd::Time& /*time*/)
    {
        if (mPhase == S)
            mPhase = I;
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

    virtual vv::Value* observation(const vd::ObservationEvent& event) const
    {
        if (event.onPort("state")) {
            switch (mPhase) {
            case INIT :
                return new vv::String("INIT");
            case S:
                return new vv::String("S");
            case I :
                return new vv::String("I");
            case R :
                return new vv::String("R");
            default :
                return new vv::String("?");
            }
        }else
            return 0;
    }

    virtual void finish()
    {
    }
    
private:
    enum PHASE {INIT, S, I, R};
    PHASE mPhase;
    double mInfectiousPeriod;
    double mAutoInfect;
    
};

} // namespace vle example

DECLARE_NAMED_DYNAMICS(infection, model::Infection)
