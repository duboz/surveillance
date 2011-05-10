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

#ifndef INFECTION_HPP
#define INFECTION_HPP

#include <vle/value.hpp>
#include <vle/devs.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

class Infection : public vd::Dynamics
{
        
public:
    Infection(const vd::DynamicsInit& init, const vd::InitEventList& events);

    virtual ~Infection();
	
    virtual vd::Time init(const vd::Time& /*time*/);

    /**
    *Envoie message infecté : transmission
    *Envoie message recovered : arrete d'infecter les gens
    */
    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const;

    virtual vd::Time timeAdvance() const;

    virtual void internalTransition(const vd::Time& /*time*/);

    virtual void externalTransition(const vd::ExternalEventList& /*event*/,
                                    const vd::Time& /*time*/);

    virtual void confluentTransitions(const vd::Time& time,
                                    const vd::ExternalEventList& events);

    virtual vv::Value* observation(const vd::ObservationEvent& event) const;

    virtual void finish();

    virtual void request(const vd::RequestEvent& event,
                         const vd::Time& /*time*/,
                         vd::ExternalEventList& output) const;

private:
    enum PHASE {INIT, S, I, R, SI};
    PHASE mPhase;
    double mInfectiousPeriod;
    double mSecuredPeriod;
    double mCleaningTime;
    double mInfectiousTimeLeft;
    double mSecuredTimeLeft;
    double mCurrentTime;
    double mAutoInfect;
    bool mConstPeriods;
    
};

} // namespace vle example

#endif
