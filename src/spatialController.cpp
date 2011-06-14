/**
 * @file vle/share/template/package/src/controler.cpp
 * @author The VLE Development Team
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
#include <boost/lexical_cast.hpp>
#include <vle/devs/Dynamics.hpp>
#include <controller.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;
using namespace vle;
namespace model {

class spatialControler : public controler
    {
private:
    double mControlRadius;
    std::map<std::string, std::vector<std::string> > mNodeNeighbors;
    std::string m_model_prefix;

public:
    spatialControler(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : controler(init, events)
    {
        loadSpatialParameters(events);
    }

    virtual ~spatialControler()
    {
    }

    virtual void output(
        const devs::Time& /* time */,
        devs::ExternalEventList& output ) const
    {
        if (m_phase == CONTROL) {
            typedef std::vector<std::string>::const_iterator NodeIterator;
    
            vv::Map* nodeObservations = vv::Map::create();
            for (NodeIterator it = m_interventions.begin()->second.begin(); 
                 it !=  m_interventions.begin()->second.end(); it++) {
                std::string spotedNode;
                spotedNode = *it;
                std::cout<<"intervention sur: "<<spotedNode<<std::endl;
                std::vector<std::string> nodesToKill = mNodeNeighbors.at(spotedNode);
                for (NodeIterator nodeToKill = nodesToKill.begin(); 
                     nodeToKill != nodesToKill.end(); nodeToKill++) {
                    nodeObservations->addString(*nodeToKill, "R");
                std::cout<<*nodeToKill<<std::endl;
                    vd::ExternalEvent * ev = new vd::ExternalEvent (*nodeToKill);
                    ev << vd::attribute ("type", buildString("clean"));
                    output.addEvent (ev);
                }
            }
            if (getModel().existOutputPort("info_center")) {
                vd::ExternalEvent * evInfo = new vd::ExternalEvent ("info_center");
                evInfo << vd::attribute("nodesStates", nodeObservations);
                output.addEvent(evInfo);
            } else {
                delete nodeObservations;
            }
        }
    }


    //Generic functions for controllers.
    virtual void loadSpatialParameters(const devs::InitEventList& events) {
             std::cout<<"Loading paramters.."<<std::endl;
        mControlRadius = vv::toDouble(events.get("controlRadius"));
        std::vector<double> positions= vv::toTuple(events.get("nodes_positions"));
        m_model_prefix = value::toString(events.get("graphInfo_prefix"));
        int nbNodes = positions.size()/2;
        double nodePositions[nbNodes][2];
        int c = 0;
        for (int node = 0; node < nbNodes; node++) {
            nodePositions[node][1] = positions[c];
            c++;
            nodePositions[node][2] = positions[c];
            c++;
        }
        for (int node = 0; node < nbNodes; node++) {
             std::string vetName = 
                 m_model_prefix + "-" + boost::lexical_cast<std::string>(node);
             std::vector<std::string> neighbors;
             std::cout<<"key: "<<vetName<<std::endl;
             mNodeNeighbors[vetName] = neighbors;
             for (int node2 = 0; node2 < nbNodes; node2++) {
                 double dist = std::sqrt(std::pow((nodePositions[node][1] - nodePositions[node2][1]),2)
                                         + std::pow((nodePositions[node][2] - nodePositions[node2][2]),2));
                 if (dist <= mControlRadius) {
                std::string vetName2 = 
                    m_model_prefix + "-" + boost::lexical_cast<std::string>(node2);
                mNodeNeighbors[vetName].push_back(vetName2);
                 }
            }
        }
    }

};

} // namespace model

DECLARE_NAMED_DYNAMICS(spatialControler, model::spatialControler)
