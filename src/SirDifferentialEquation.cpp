/**
 * @file SirDifferentialEquation.cpp
 * @author Nima Izadi & Nicolas Dupont
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
 
#include "SirDifferentialEquation.hpp"
#include <vle/utils/Debug.hpp>

namespace vd = vle::devs;
namespace vq = vle::extension::QSS;
namespace vv = vle::value;

namespace model {

SirDiffentialEquation::SirDiffentialEquation(const vd::DynamicsInit& model,
                   const vd::InitEventList& events):
    vq::Multiple(model, events)
{
    mBeta = vv::toDouble(events.get("beta"));
    mGamma = vv::toDouble(events.get("gamma"));

    mSIR.push_back(createVar(0, "S"));
    mSIR.push_back(createVar(1, "I"));
    mSIR.push_back(createVar(2, "R"));
}

SirDiffentialEquation::~SirDiffentialEquation()
{
}

double SirDiffentialEquation::compute(unsigned int i, const vd::Time& /* time */) const
{
    switch(i) {
	case 0: // S		
	    return -mBeta * (mSIR[0])() * (mSIR[1])();
	case 1: // I
	    return mBeta *  (mSIR[0])() * (mSIR[1])() - mGamma * (mSIR[1])();
	case 2: // R
	    return mGamma * (mSIR[1])();
    }
    return 0.;
}

} // namespace model
 
DECLARE_DYNAMICS(model::SirDiffentialEquation);
