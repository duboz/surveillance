/**
 * @file vle/share/template/package/src/information_center.cpp
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
#include <boost/tokenizer.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
using namespace vle;
namespace model {

class information_center : public devs::Dynamics
    {
private:
    enum Phase {INIT, IDLE};
    Phase m_phase;
    std::map<std::string, std::pair<std::string, vd::Time> > m_nodeStates;

public:
    information_center(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
    }

    virtual ~information_center()
    {
    }

    virtual devs::Time init(
        const devs::Time& /*time*/)
    {   
        m_phase = INIT;
        return 0;
    }

    virtual void output(
        const devs::Time& /* time */,
        devs::ExternalEventList& /*output*/ ) const
    {
    }

    virtual devs::Time timeAdvance() const
    {
      return devs::Time::infinity;
    }

    virtual void internalTransition(
        const devs::Time& time)
    {
        if (m_phase == INIT)
            m_phase = IDLE;
    }

    virtual void externalTransition(
        const devs::ExternalEventList&  event ,
        const devs::Time& time)
    {
        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            if ((*it)->onPort("information")) {
                value::Map infNodes = (*it)->getMapAttributeValue("nodesStates");
                for (vv::MapValue::const_iterator node = infNodes.begin();
                     node != infNodes.end(); node++) {
                    m_nodeStates[node->first] = 
                       std::pair<std::string, vd::Time> (node->second->toString().value(), time);
                }
            }
        }
    }

    virtual void confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    virtual void request(
        const devs::RequestEvent& /* event */,
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const
    {
    }

    virtual value::Value* observation(
        const devs::ObservationEvent& event) const
    {
    vv::Map* tmpResult = new vv::Map();
    std::map<std::string, std::pair<std::string, vd::Time> >::const_iterator it;
    int Ss=0;
    int Is=0;
    int Rs=0;
    for ( it = m_nodeStates.begin(); it != m_nodeStates.end(); ++it ) {
        tmpResult->addString(it->first, (it->second).first);
        if ((it->second).first == "S") Ss++; 
        else if ((it->second).first == "I") Is++; 
        else if ((it->second).first == "R") Rs++;
    }
    if (event.onPort("outbreakReport")) {

        typedef boost::tokenizer < boost::char_separator < char > > tokenizer;
        boost::char_separator<char> sep("-");

        vv::Set* repport = new vv::Set();
        for ( it = m_nodeStates.begin(); it != m_nodeStates.end(); ++it ) {
            if ((it->second).second >= (event.getTime() - vd::Time(7)) and ((it->second).first=="I")) {
                //std::cout<<"pour le noeud "<<it->first<<", que se passe-t-il?"<<std::endl;
                std::string node = it->first;
                tokenizer tok(node, sep);
                boost::tokenizer<boost::char_separator < char > >::const_iterator i = tok.begin();
                i++;
                //std::cout<<*tok.begin()<<" et "<<*i<<std::endl;
                repport->addInt(std::atoi(i->c_str()));
            }
        }
        return repport;
    }
    if (event.onPort("nbIs")) {
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(Is);
    }

    if (event.onPort("nbSs")) {
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(Ss);
    }

    if (event.onPort("nbRs")) {
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildInteger(Rs);
    }

    return tmpResult;
    }

    virtual void finish()
    {
    }
};

} // namespace model

DECLARE_DYNAMICS(model::information_center);
