/**
 * @file 
 * @author Romaric Pighetti <romaric.pighetti@gmail.com>
 *         Jeremy Gabriele  <gabriele@polytech.unice.fr>
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

#include <vle/value.hpp>
#include <vle/devs.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

/**
 * @brief A class to collect data from the experiments made with sir models.
 *
 * This class send a RequestEvent on its "status?" port periodically. The time 
 * step is given by mObservationTimeStep. This variable is fixed in the source
 * code for now but should become a parameter soon.
 *
 * The response for the RequestEvent is expected on the "status" port of this 
 * model and must be an ExternalEvent with the structure:
 *   -one attribute value which name is "status" and value is the string "S", 
 *   "I" or "R", depending on the state of the model that respond.
 *
 * For every request the number of each response is stocked together with the 
 * date of the request in a matrix. The matrix can be seen through the obser-
 * vation method at the end of the simulation.
 *
 */
class DataCollector : public vd::Dynamics
{        
public:
    DataCollector(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
    }

    virtual ~DataCollector()
    {
    }

    virtual vd::Time init(const vd::Time& time)
    {
        mPhase = SEND;
        mObservationTimeStep = 5;
        mSainNumber = 0;
        mInfectedNumber = 0;
        mRecoveredNumber = 0;
        mCurrentTime = vd::Time(time);
        mLastRequestTime = vd::Time(time);
        mSainResult = 0;
        mRecoveredResult = 0;
        mInfectedResult = 0;
        mDataCollection = new vv::Matrix();
        mDataCollection->addRow();
        mDataCollection->addColumn();
        mDataCollection->addColumn();
        mDataCollection->addColumn();
        mDataCollection->addColumn();
        mDataCollection->addString(0,0,std::string("Time"));
        mDataCollection->addString(1,0,std::string("S"));
        mDataCollection->addString(2,0,std::string("I"));
        mDataCollection->addString(3,0,std::string("R"));
        return 0;
    }

    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const
    {
        if (mPhase == SEND) {
            vd::RequestEvent * request = new vd::RequestEvent ("status?");
            request << vd::attribute ("name", std::string ("status"));
            output.addEvent (request);
        }
    }

    virtual vd::Time timeAdvance() const
    {
        if (mPhase == SEND) {
            return 0;
        }
        return mObservationTimeStep - (mCurrentTime.getValue() 
               - mLastRequestTime.getValue());
    }

    virtual void internalTransition(const vd::Time& time)
    {
        switch (mPhase) {
        case SEND:
            mPhase = RECEIVE;
            mLastRequestTime = vd::Time(time);
            mCurrentTime = vd::Time(time);
            break;
        case RECEIVE:
            mDataCollection->addRow();
            mDataCollection->addDouble(0,mDataCollection->rows()-1,
                                                                  mLastRequestTime.getValue());
            mDataCollection->addDouble(1,mDataCollection->rows()-1,
                                                                  mSainResult);
            mDataCollection->addDouble(2,mDataCollection->rows()-1,
                                                                  mInfectedResult);
            mDataCollection->addDouble(3,mDataCollection->rows()-1,
                                                                  mRecoveredResult);
            mSainResult = 0;
            mInfectedResult = 0;
            mRecoveredResult = 0;
            mPhase = SEND;
            mCurrentTime = vd::Time(time);
            break;
        default:
            std::cout << "bad state in DataCollector model !";
        }
    }

    virtual void externalTransition(const vd::ExternalEventList& event,
                                    const vd::Time& time)
    {
        
        if (mPhase == RECEIVE) {
            for (vd::ExternalEventList::const_iterator it = event.begin();
                        it != event.end(); ++it) {
                    if ((*it) -> getStringAttributeValue ("name") == "status") {
                        std::string value = 
                              (*it)-> getStringAttributeValue ("value");
                        if (value == "S") {
                            mSainResult++;
                        } else if (value == "I") {
                            mInfectedResult++;
                        } else if (value == "R") {
                            mRecoveredResult++;
                        }
                    }
            }
            mCurrentTime = vd::Time(time);
        }
    }

    virtual void confluentTransitions(const vd::Time& time,
                                      const vd::ExternalEventList& events)
    {
        if (mPhase == SEND) {
            internalTransition(time);
            externalTransition(events, time);
        } else {
            externalTransition(events, time);
            internalTransition(time);
        }
    }

    virtual void request(const vd::RequestEvent& /*event*/,
                         const vd::Time& /*time*/,
                         vd::ExternalEventList& /*output*/) const
    {
    }

    virtual vv::Value* observation(const vd::ObservationEvent& /*event*/) const
    {
        return mDataCollection;
    }

    virtual void finish()
    {
    }
    
private:
    enum PHASE {SEND, RECEIVE};
    PHASE mPhase;
    vd::Time mLastRequestTime;
    vd::Time mCurrentTime;
    int mObservationTimeStep;
    int mSainNumber;
    int mInfectedNumber;
    int mRecoveredNumber;
    int mSainResult;
    int mInfectedResult;
    int mRecoveredResult;
    vv::Matrix *mDataCollection;
};

} // namespace vle example

DECLARE_NAMED_DYNAMICS(data_collector, model::DataCollector)
