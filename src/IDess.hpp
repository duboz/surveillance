/*
 * @file examples/dess/IDess.hpp
 ** @author Vivien Massart
 *
 */
 
 /* This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_EXAMPLES_SIR_HPP
#define VLE_EXAMPLES_SIR_HPP

#include <vle/extension/differential-equation/DESS.hpp>
namespace vd = vle::devs;
namespace ve = vle::extension;
namespace vv = vle::value;

namespace model { 

class IDess : public ve::DifferentialEquation::DESS
{
public:
    IDess(const vd::DynamicsInit&, const vd::InitEventList&);
    virtual ~IDess(){}

    virtual double compute(const vd::Time& time) const;

private:
    double r;
    double a;
    double m;
    std::string sint;
    std::string sext;

    std::vector <Var > mSIR;
    std::vector <Ext > mSIRext;
    std::vector <std::string> vint;
    std::vector <std::string> vext;
};

} // namespace model

#endif

