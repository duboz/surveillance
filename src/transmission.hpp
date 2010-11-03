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

#ifndef TRANSMISSION_HPP
#define TRANSMISSION_HPP

#include <vle/value.hpp>
#include <vle/devs.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

class Transmission : public vd::Dynamics
{

    
public:
    Transmission(const vd::DynamicsInit& init, const vd::InitEventList& events);

    virtual ~Transmission();
	
    virtual vd::Time init(const vd::Time& /*time*/);

    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const;

    virtual vd::Time timeAdvance() const;

    virtual void internalTransition(const vd::Time& /*time*/);

    virtual void externalTransition(const vd::ExternalEventList& events,
                                    const vd::Time& /*time*/);

    virtual void confluentTransitions(const vd::Time& time,
                                    const vd::ExternalEventList& events);

    virtual void request(const vd::RequestEvent& /*event*/,
                        const vd::Time& /*time*/,
                        vd::ExternalEventList& /*output*/) const;

    virtual vv::Value* observation(const vd::ObservationEvent& event) const;
    
private:
    enum PHASE {INIT, IDLE, INFECTING};
    PHASE mPhase;
    int mNbPort;
    int mPortIndex;
    int* mOrder;
    double* mInfectionOffsets;
    double mRate;
    bool mStateDisplayed;
    
    void randomizeOrder();
    
    void randomizeInfectionTime();
        
};

} // namespace vle example

#endif
