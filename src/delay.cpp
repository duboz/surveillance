/**
 * @file vle/share/template/package/src/Delay.cpp
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

#include <vle/devs/Dynamics.hpp>
#include <delay.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;
using namespace vle;
namespace model {
    
    Delay::Delay(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
        m_delay = vv::toDouble(events.get("waitingDelay"));
        m_agreggationThreshold =  vv::toDouble(events.get("obsAggregationThreshold"));
    }

    Delay::~Delay()
    {
    }

    devs::Time Delay::init(
        const devs::Time& time)
    {   
        m_current_time = time.getValue();
        m_phase = INIT;
        return 0;
    }

    void Delay::output(
        const devs::Time& time,
        devs::ExternalEventList& output ) const
    {
        if (m_phase == WAITING) {
            typedef std::vector<vv::Map*>::const_iterator AttIterator;
            //std::cout<<(*m_evBags.begin()->second.begin())->begin()->first<<std::endl; 
            for (AttIterator it = m_evBags.begin()->second.begin(); 
                 it !=  m_evBags.begin()->second.end(); it++) {
                vd::ExternalEvent * ev = new vd::ExternalEvent ("output");
                vv::Map attributes(*(*it));
                ev->putAttributes(attributes);
                output.addEvent (ev);
                //std::cout<<"evt got delayed at: "<<time<<
                //    " (first att being: "<< ev->getAttributes().begin()->first<<")"<<std::endl;
            }
        }
    }

    devs::Time Delay::timeAdvance() const
    {
        if (m_phase == WAITING)
            return m_evBags.begin()->first - devs::Time(m_current_time);
        if (m_phase == INIT)
            return 0;

        return devs::Time::infinity;
    }

   void Delay::internalTransition(
        const devs::Time& time)
    {
        if (m_phase == INIT)
            m_phase = IDLE;
        else if (m_phase == WAITING) {
            m_evBags.erase(m_evBags.begin());
            if (m_evBags.empty())
                m_phase = IDLE;
        }
        m_current_time = time.getValue();
    }

    void Delay::externalTransition(
        const devs::ExternalEventList&  event ,
        const devs::Time& time)
    {
        evBag *newevBag;
        if ((m_phase == WAITING) and (time + vd::Time(m_delay) - m_evBags.back().first) <
            m_agreggationThreshold) {
            newevBag = &m_evBags.back();
            std::cout<<"pas bon"<<std::endl;
        } else {
            m_phase = WAITING;
            newevBag = new evBag();
            newevBag->first = time + vd::Time(m_delay);
            m_evBags.push_back(*newevBag);
        }
        evBag& bag = m_evBags.back();
        for (vd::ExternalEventList::const_iterator it = event.begin();
             it != event.end(); ++it) {
            vv::Map* evtAttributes = new vv::Map(vv::toMapValue(*(*it)->getAttributes().clone()));
            //(*it)->getAttributes().writeFile(std::cout);
            //std::cout<<(*it)->getAttributes().begin()->first<<std::endl;
            bag.second.push_back(evtAttributes);
            //evtAttributes->writeFile(std::cout);
            //std::cout<<"at: "<<time.getValue()<<" evt to be delayed to: "<<bag.first.getValue()<<
            //    " (lenght att is: "<<m_evBags.size() << std::endl;//(*m_evBags.back().second.begin())->begin()->first<<")"<<std::endl;
            //std::cout<<"and time is indeed:  "<<(m_evBags.begin()->first)<<std::endl; 
            //std::cout<<((m_evBags.back().second.size()));//->writeFile(std::cout); 
        }
        m_current_time = time.getValue();
    }

    void Delay::confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    void Delay::request(
        const devs::RequestEvent& /* event */,
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const
    {
    }

    value::Value* Delay::observation(
        const devs::ObservationEvent& /* event */) const
    {
        return 0;
    }

    void Delay::finish()
    {
    }
    
} // namespace model

DECLARE_DYNAMICS(model::Delay);
