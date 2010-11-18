/**
 * @file SirDifferentialEquation.hpp
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
 
#ifndef SIR_DIFFERENTIAL_EQUATION_HPP
#define SIR_DIFFERENTIAL_EQUATION_HPP 1
#include <vle/extension/differential-equation/QSS.hpp>
#include <vle/utils/Debug.hpp>

namespace vd = vle::devs;
namespace vq = vle::extension::QSS;
namespace vv = vle::value;

namespace model {

class SirDiffentialEquation : public vq::Multiple
{
public:
    SirDiffentialEquation(const vd::DynamicsInit& model,
	     const vd::InitEventList& events);

    ~SirDiffentialEquation();

    virtual double compute(unsigned int i, const vd::Time& time) const;

private:
	
    int value;
    std::vector <Var > mSIR;
    
    double mBeta;
    double mGamma;

};

} // namespace model

#endif
