/** 
 * @file vertexExecutive.cpp
 * @brief VertexExecutive class
 * @author Bruno Bonte
 * @date mardi 9 février 2010, 11:40:50 (UTC+0100)
 */

/*
 * Copyright (C) 2010 - Bruno Bonte
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "vertexExecutive.hpp"
#include <vle/graph/CoupledModel.hpp>

using namespace vle;

namespace model {
    VertexExecutive::VertexExecutive(const vle::devs::ExecutiveInit& mdl,
                                 const vle::devs::InitEventList& evList): 
    devs::Executive(mdl,evList)
    {
    }

    devs::Time VertexExecutive::init(const vle::devs::Time& /* time */)
    {
	m_phase=INIT;
	addInputPort("infection", "status?");
	addOutputPort("infection", "data_collector");
	return 0.0;
    }

    void VertexExecutive::internalTransition(const devs::Time & /*date*/)
    {
    
       /* input ports for connection with the coupled model*/
       const graph::ConnectionList& listin = coupledmodel().getInternalInputPortList();
       graph::ConnectionList::const_iterator itin;
      
       /* Output ports for connection with the coupled model*/
       const graph::ConnectionList& list =coupledmodel().getInternalOutputPortList();
       graph::ConnectionList::const_iterator it;
       
       /* Input and Output connections */
       //in
       addConnection(coupledmodelName(),"status?", "infection","status?");
       for (itin = listin.begin(); itin != listin.end(); ++itin) {
           if (itin->first != "status?")
          	addConnection(coupledmodelName(),itin->first,"infection","infection");
       }
       //out
	addConnection("infection","data_collector",coupledmodelName(),"data_collector");
        int nb = 0;
        for (it = list.begin(); it != list.end(); ++it) {
           if (it->first != "data_collector")
           {
   		std::string prtName = "infection_" + boost::lexical_cast<std::string>(nb);
   		addOutputPort("transmission", prtName);
    		addConnection("transmission", prtName, coupledmodelName(),it->first);
		nb++;
	    }        
        }
        m_phase=INITIALIZED;
    }
    devs::Time VertexExecutive::timeAdvance() const
    {
	if(m_phase == INIT)
	    return devs::Time(0.);
	return devs::Time::infinity;
}


DECLARE_NAMED_EXECUTIVE(dyn_vertexExecutive, model::VertexExecutive)
}
