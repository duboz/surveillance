/** 
 * @file DessExec.cpp
 * @brief DessExec class
 * @author Vivien Massart
 */

/*
 * Copyright (C) 2010 - Vivien Massart
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
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Condition.hpp>

using namespace vle;

namespace vd = vle::devs;
namespace vv = vle::value;
namespace model {
 /*
*@brief An Executive using a graphTranslator class and graph condition, and node 
* classes definitions specified in the vpz to create all node models and connect them
*/
class DessExec : public devs::Executive
{
public:
   /* void addCondition(const std::string& modelName,
                              const std::string& condName,
				const std::string& portname,
                                value::Value* value )
	
{
graph::Model* mdl = cpled()->findModel(modelName);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelName);
    }

vpz::Condition cond(condName);
cond.add(portname);
cond.addValueToPort(portname,value);
	
}

void  addObservable(const std::string& portname,
                                const std::string& view)
{
	vpz::Observable obs(portname);
	obs.add(view);
}
*/
    DessExec(const devs::ExecutiveInit& mdl,
		   const devs::InitEventList& events)
        : devs::Executive(mdl, events)
    {
	//std::string myints[10];
	m_graphInfo.value() = value::toMap(events.get("graphInfo")->clone());
	m_nb_model = m_graphInfo.getInt("number");
	m_model_prefix = m_graphInfo.getString("prefix");
	
	//std::string myints[] = {m_graphInfo.getString("inputs")};
  	m_inputs.push_back(m_graphInfo.getString("inputs"));
	
	//myints =   {m_graphInfo.getString("outputs")};	
	m_outputs.push_back(m_graphInfo.getString("outputs") ); 
  	m_dynamics = m_model_prefix+"Dess";

  	//myints[] = {"cond_"+m_dynamics};
    	m_conditions.push_back("cond_"+m_dynamics) ;

	//myints[] = {"cond_"+m_dynamics};
    	m_observable ="cond_"+m_dynamics;

    }
    
    std::string getconecName(std::string m_model_prefix)
    {
    
    switch(m_model_prefix[0]){
	case  'S'	:	
	    return "I";
	case  'I':
	    return "S";
	case  'R':
	    return "I";
    }
    return "I";
    }
 
    virtual ~DessExec()
    {}
 
    virtual devs::Time init(const devs::Time& /* time */)
    {
        translator::GraphTranslator tr(*this); 
        tr.translate2(m_graphInfo,m_inputs, m_outputs, m_dynamics, m_conditions, m_observable);
        std::string modelName;
        value::Value* modelExtName;

		for (int i = 0; i < m_nb_model; i++) {
			std::string vetName = m_model_prefix  + boost::lexical_cast<std::string>(i);
			std::string ext = "ext";
			std::string ss = "cond_" +modelName;	
			std::string sss	= "obs_" +boost::lexical_cast<std::string>(modelExtName);	
			addInputPort(vetName, "update");
			addOutputPort(vetName, "update");
			addConnection(vetName, "update","update","update");
			addConnection("update","update",vetName, "update");
			
			modelName = m_model_prefix + boost::lexical_cast<std::string>(i);
			modelExtName = buildString( m_model_prefix + boost::lexical_cast<std::string>(i-1)+","+getconecName(m_model_prefix)+ boost::lexical_cast<std::string>(i)) ;
			addCondition(modelName,ss,ext, modelExtName);
			addObservable(sss,"dessview");
		}

        return devs::Time::infinity;
    }

private:
    value::Map m_graphInfo;
    int m_nb_model;
    std::string m_model_prefix;
    std::vector < std::string > m_inputs;
    std::vector < std::string > m_outputs;
    std::string m_dynamics;
    std::vector< std::string > m_conditions;
    std::string m_observable;
};

} // namespace model
 
DECLARE_NAMED_EXECUTIVE_DBG(dyn_dessExec, model::DessExec)
