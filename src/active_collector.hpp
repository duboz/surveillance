/**
 * @file x_ray.hpp
 * @author Virginie Baticle <baticle@polytech.unice.fr>
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
 * @brief X-Ray Surveillance model
 *
 * This class has exactly the same behavior as the data_collector class.
 * Only two differences are notable :
 *
 *     - The "modelName" attribute of the ExternalEvent sent by the infection
 *   model is taken in consideration to know the state of each SIR model.
 *
 *     - When an answer is got from a SIR model, a probability that the
 *   collector "make a mistake" is applied to determine whether the stocked
 *   result is the good one or its opposite.
 *
 *     - Every observation step, each couple [modelName, value] got are returned
 *   by the observation method.
 *
 */
class ActiveCollector : public vd::Dynamics
{
public:
    ActiveCollector(const vd::DynamicsInit& init, const vd::InitEventList& events);
    virtual ~ActiveCollector();
    virtual vd::Time init(const vd::Time& time);
    void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const;
    virtual vd::Time timeAdvance() const;
    virtual void internalTransition(const vd::Time& time);
    virtual void externalTransition(const vd::ExternalEventList& event,
                                    const vd::Time& time);
    virtual void confluentTransitions(const vd::Time& time,
                                      const vd::ExternalEventList& events);
    virtual void request(const vd::RequestEvent& /*event*/,
                         const vd::Time& /*time*/,
                         vd::ExternalEventList& /*output*/) const;
    virtual vv::Value* observation(const vd::ObservationEvent& /*event*/) const;
    virtual void finish();  

    //Generic active collector functions
    virtual void connectToNodes(vd::ExternalEventList& output) const = 0;
    virtual void receiveData(const vd::ExternalEventList& event,
                                  const vd::Time& time);
protected:
    enum PHASE {SEND, RECEIVE, SEND_OBS, INIT, IDLE, CHG_CONNECTION};
    PHASE mPhase;
    vd::Time mLastRequestTime;
    vd::Time mCurrentTime;
    double mObservationTimeStep;
    double mProbabilityRightSR;
    double mProbabilityRightI;
    int mSampleSize;    
    int mNbModel;
    std::string mPrefix;
    std::map<std::string, std::string> mapResult;
    double mPrevalence;
    double mIncidence;
};

} // namespace model
