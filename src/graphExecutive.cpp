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
 /**
  *@brief An Executive using a graphTranslator class and graph condition, and node 
  * classes definitions specified in the vpz to create all node models and connect them.
  *
  * This executive can receive message on its "connectTo" input port to change the connection of
  * a collector. 
  * The event must contain: 
  *     -an attribute "modelName" with the name of the node wich will be affected.
  *     -an attribute "linkTo" which is a Set containing the names of the new model to connect.
  *     -an attribute "unlink" which is a Set containing the names of the model to disconnect.
  * /!\ DO NOT USE THIS TO CHANGE THE CONNECTION OF OTHER NODE IT WON'T WORK
  *
  * All the verteces are initialized with all the ports needed in order to receive request message and send
  * response messages for active collectors and also a port for sending messages to the passive collectors.
  * The connections with the passive collectors are also made at the initilization.
  *
  * The active collectors are connected to the Executive through an output port "connectTo" and they have to
  * send an event to the Executive if they want to be connected to some SIR models.
  *
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
      m_active_collectors.value() = value::toSet(events.get("activeCollectors")->clone());
      m_passive_collectors.value() = value::toSet(events.get("passiveCollectors")->clone());
    }
 
    virtual ~GraphExecutive()
    {}
 
    virtual devs::Time init(const devs::Time& /* time */)
    {
        translator::GraphTranslator tr(*this); 
        tr.translate(m_graphInfo);

        /* Add a port to the executive to listen for connection requests */
        addInputPort(getModelName(),"connectTo");

        /* 
         * Create input port "status" and output port "status?" for active 
         * collectors and an output port "connectTo" to send messages to the
         * executive.
         */
        for (unsigned int i = 0; i < m_active_collectors.size(); i++) {
          addInputPort(m_active_collectors[i]->writeToString(), "status");
          addOutputPort(m_active_collectors[i]->writeToString(), "status?");
          addOutputPort(m_active_collectors[i]->writeToString(), "connectTo");
          /* Connect the port to the executive */
          addConnection(m_active_collectors[i]->writeToString(), "connectTo",
                        getModelName(),"connectTo");
        }

        /* Create input port "status" for passive collectors */
        for (unsigned int i = 0; i < m_passive_collectors.size(); i++) {
          addInputPort(m_passive_collectors[i]->writeToString(), "status");
        }

        for (unsigned int i = 0; i < m_nb_model; i++) {
          /* Add an input port "status?" for vertex */
          std::string vetName = 
            m_model_prefix + "-" + boost::lexical_cast<std::string>(i);			
          addInputPort(vetName, "status?");
          
          /* Add output ports with the name of the active collectors models */
          for (unsigned int j = 0; j < m_active_collectors.size(); j++) {
            addOutputPort(vetName, m_active_collectors[j]->writeToString());
          }

          /* Add an output port "passiveCollectors" for the connection 
           * with passive collectors */
          addOutputPort(vetName, "passiveCollectors");

          /* Connect the vertex to all passive collectors */
          for (unsigned int j = 0; j < m_passive_collectors.size(); j++) {
            addConnection(vetName, "passiveCollectors", 
                m_passive_collectors[j]->writeToString(), "status");
          }         
        }

        return devs::Time::infinity;
    }

    void externalTransition(
                                  const vd::ExternalEventList& event,
                                  const vd::Time& /*time*/)
    {
      for (vd::ExternalEventList::const_iterator it = event.begin();
          it != event.end(); ++it) {
        std::string modelName = (*it) -> getStringAttributeValue("modelName");
        vv::Set linkTo = (*it) -> getSetAttributeValue("linkTo");
      //  vv::Set unlink = (*it) -> getSetAttributeValue("unlink");
       /* for (unsigned int i = 0; i < unlink.size(); i++) {
          removeConnection(modelName, "status?", 
                           unlink[i]->writeToString(), "status?");
          removeConnection(unlink[i]->writeToString(), modelName,
                           modelName, "status");
        }*/
        removeOutputPort(modelName, "status?");
        addOutputPort(modelName, "status?");
        
        for (unsigned int i = 0; i < linkTo.size(); i++) {
          addConnection(modelName, "status?", 
                           linkTo[i]->writeToString(), "status?");
          addConnection(linkTo[i]->writeToString(), modelName,
                           modelName, "status");
        }
      }
    }

private:
    value::Map m_graphInfo;
    unsigned int m_nb_model;
    std::string m_model_prefix;
    value::Set m_active_collectors;
    value::Set m_passive_collectors;
};

} // namespace model
 
DECLARE_NAMED_EXECUTIVE_DBG(dyn_graphExecutive, model::GraphExecutive)
