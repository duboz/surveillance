/*
 * @file RDess.cpp
 *@author Vivien Massart
 *
 */
 
 /* This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright Vivien Massart
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


#include "RDess.hpp"
#include <vle/value/Double.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/devs/DynamicsDbg.hpp>

namespace model {

RDess::RDess(const vd::DynamicsInit& model, const vd::InitEventList& events) :
    ve::DifferentialEquation::DESS(model, events)	
{
     a = vv::toDouble(events.get("a"));
     a = vv::toDouble(events.get("m"));
     sint = vv::toString(events.get("int"));
     sext = vv::toString(events.get("ext"));
    
     size_t sizei = sint.size() + 1;
     size_t sizee = sext.size() + 1;
     char * sintc = new char[ sizei ]; 
     char * sextc = new char[ sizee ]; 
     char * pch;
   
    strncpy( sintc, sint.c_str(), sizei );
    strncpy( sextc, sext.c_str(), sizee );
    
    pch = strtok (sintc," ,");
    while (pch != NULL)
    {
    vint.push_back(pch);
    pch = strtok (NULL, " ,");
    } 
    pch = strtok (sextc," ,");
    while (pch != NULL)
    {
    vext.push_back(pch);
    pch = strtok (NULL, " ,");
    } 
    delete [] sextc;
    delete [] sintc;
    delete pch;

    unsigned int i;
    for (i=0;i<vint.size();i++){
	 mSIR.push_back(createVar(vint[i]));
	 }	 
    for (i=0;i<vext.size();i++){
         mSIRext.push_back(createExt(vext[i]));
         }
}

/*
*  -mSIR la variable interner R.
*  -mSIRext les variables externes : 
*       - I
*      - les autres noeud R connecter.
*/
double RDess::compute(const vd::Time& ) const
{
	double c = a * mSIRext[0]();
        for(unsigned int i = 1 ;i <vext.size() ; i++){  c += m*mSIRext[i]() - m * mSIR[0]();}
	return c;
}


		
} // namespace model
DECLARE_NAMED_DYNAMICS_DBG(rDess, model::RDess)
