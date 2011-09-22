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
#include <active_collector.hpp>

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
class smart_surv: public model::ActiveCollector
{
public:
    smart_surv(const vd::DynamicsInit& init, const vd::InitEventList& events);
    ~smart_surv();
    vv::Value* observation(const vd::ObservationEvent& /*event*/) const;
    virtual void connectToNodes(vd::ExternalEventList& output) const;
    void computeSampleProp();
private:
    double mPrecision;
    double mSampleMin;
};


} // namespace model
