/** 
 * @file GraphExecutive.cpp
 * @brief GraphExecutive class
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


#include <vle/devs/Executive.hpp>
#include <vle/devs/ExecutiveDbg.hpp>
#include <vle/translator/GraphTranslator.hpp>
#include <vle/value/Map.hpp>
#include <boost/lexical_cast.hpp>

using namespace vle;

namespace vd = vle::devs;
namespace vv = vle::value;
namespace model {
 /*
*@brief An Executive using a graphTranslator class and graph condition, and node 
* classes definitions specified in the vpz to create all node models and connect them
*/
class GraphExecutive : public devs::Executive
{
public:
    GraphExecutive(const devs::ExecutiveInit& mdl,
		   const devs::InitEventList& events)
        : devs::Executive(mdl, events)
    {
	m_graphInfo.value() = value::toMap(events.get("graphInfo")->clone());
	m_nb_model = m_graphInfo.getInt("number");
	m_model_prefix = m_graphInfo.getString("prefix");
    }
 
    virtual ~GraphExecutive()
    {}
 
    virtual devs::Time init(const devs::Time& /* time */)
    {
        translator::GraphTranslator tr(*this); 
        tr.translate(m_graphInfo);

		addInputPort("data_collector", "status");
		addOutputPort("data_collector", "status?");
				
		for (int i = 0; i < m_nb_model; i++) {
			/* Add an input and output port "status" and "status?" for vertex */
			std::string vetName = m_model_prefix + "-" + boost::lexical_cast<std::string>(i);			
			addInputPort(vetName, "status?");
			addOutputPort(vetName, "status");
			addConnection(vetName, "status","data_collector","status");
			addConnection("data_collector","status?",vetName, "status?");			
		}

        return devs::Time::infinity;
    }

private:
    value::Map m_graphInfo;
    int m_nb_model;
    std::string m_model_prefix;
};

} // namespace model
 
DECLARE_NAMED_EXECUTIVE_DBG(dyn_graphExecutive, model::GraphExecutive)
