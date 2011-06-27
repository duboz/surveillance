/**
 * @file vle/share/template/package/src/SurvScheduler.cpp
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

#include <vle/devs/DynamicsDbg.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

using namespace vle;

namespace model {

class SurvScheduler : public devs::Dynamics
    {
       
typedef std::pair<double, std::vector<std::string> > Intervention;
typedef std::vector<Intervention> InterventionPlan;
typedef std::vector<std::string>::const_iterator NodeIterator;

private:
    enum Phase {INIT, SCHEDULING, IDLE};
    Phase m_phase;
    InterventionPlan m_interventions;
    double m_current_time;
    double m_aggregationThreshold;

public:
    SurvScheduler(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
        std::string modelPrefix = vv::toString(events.get("graphInfo_prefix"));
        std::vector<double> first_wave = vv::toTuple(events.get("first_wave"));
        std::vector<double> second_wave = vv::toTuple(events.get("second_wave"));
        unsigned int node_number = vv::toInteger(events.get("graphInfo_number"));
        m_aggregationThreshold =  vv::toDouble(events.get("obsAggregationThreshold"));

        if (node_number != first_wave.size())
            utils::ModellingError::ModellingError(boost::format(
        "Bad node number in Xray first_wave scedule == %1%") % node_number );
        if (node_number != second_wave.size())
            utils::ModellingError::ModellingError(boost::format(
        "Bad node number in Xray second_wave scedule == %1%") % node_number );
        for (unsigned int i = 0; i < node_number; i++) {
            Intervention newIntervention;
            newIntervention.first = first_wave[i];
            std::string nodeName((boost::format("%1%-%2%") % modelPrefix % i).str());
            newIntervention.second.push_back(nodeName);
            Intervention newIntervention2;
            newIntervention2.first = second_wave[i];
            newIntervention2.second.push_back(nodeName);
            m_interventions.push_back(newIntervention);
            m_interventions.push_back(newIntervention2);
        }
        std::sort(m_interventions.begin(), m_interventions.end(), IsBefore());
        unsigned int i = 0;
        while (i+1 < m_interventions.size()) {
            while ((i+1 < m_interventions.size()) and
                   ((m_interventions[i+1].first - m_interventions[i].first) < 
                   m_aggregationThreshold)) {
               for (NodeIterator it = m_interventions[i+1].second.begin(); 
                    it !=  m_interventions[i+1].second.end(); it++) {
                   m_interventions[i].second.push_back(*it);
               }
                m_interventions.erase(m_interventions.begin()+i+1);
            }
            i++;
        }
    }
    //Fonctor for the ordering of the interventions
    struct IsBefore
    {
        bool operator() (const Intervention & int1, const Intervention & int2) const
        {
            return int1.first < int2.first;
        }
    };

    virtual ~SurvScheduler()
    {
    }

    virtual devs::Time init(
        const devs::Time& time)
    {
        m_current_time = time.getValue();
        m_phase = INIT;
        return 0;
    }

    virtual void output(
        const devs::Time& /* time */,
        devs::ExternalEventList& output) const
    {
        if (m_phase == SCHEDULING) {
            vv::Map* nodeToObserve = vv::Map::create();
            vd::ExternalEvent * ev = new vd::ExternalEvent("surveillance");
            for (NodeIterator it = m_interventions.begin()->second.begin(); 
                 it !=  m_interventions.begin()->second.end(); it++) {
                nodeToObserve->addString(*it,"rien");
            }
            ev << vd::attribute ("infectedNodes", nodeToObserve);
            output.addEvent (ev);
        }
    }

    virtual devs::Time timeAdvance() const
    {   
        if (m_phase == SCHEDULING)
            return devs::Time(m_interventions.begin()->first - m_current_time);
        if (m_phase == INIT)
            return 0;
        return devs::Time::infinity;
    }

    virtual void internalTransition(
        const devs::Time& time)
    {
       if (m_phase == INIT)
            m_phase = SCHEDULING;
        else if (m_phase == SCHEDULING) {
            m_interventions.erase(m_interventions.begin());
            if (m_interventions.empty())
                m_phase = IDLE;
        }
        m_current_time = time.getValue();
    }

    virtual void externalTransition(
        const devs::ExternalEventList& /* event */,
        const devs::Time& /* time */)
    {
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
        const devs::ObservationEvent& /* event */) const
    {
        return 0;
    }

    virtual void finish()
    {
    }
};

} // namespace model

DECLARE_NAMED_DYNAMICS_DBG(SurvScheduler, model::SurvScheduler)
