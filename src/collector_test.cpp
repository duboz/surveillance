/**
 * @file collector_test.cpp
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

class CollectorTest : public vd::Dynamics
{    
public:
    CollectorTest(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
    }

    virtual ~CollectorTest()
    {
    }

    virtual vd::Time init(const vd::Time& /*time*/)
    {
        mResult = 0;
        return 5;
    }

    virtual vd::Time timeAdvance() const
    {
        return 5;
    }

    virtual void internalTransition(const vd::Time& /*time*/)
    {
        mResult++;
    }
    virtual void request(const vd::RequestEvent& event,
                         const vd::Time& /*time*/,
                         vd::ExternalEventList& output) const
    {
        if (event.getStringAttributeValue ("name") == "status") {
            std::string responseValue = std::string("");
            switch (mResult%3) {
            case 0:
                responseValue = std::string("S");
                break;

            case 1:
                responseValue = std::string("I");
                break;

            case 2:
                responseValue = std::string("R");
                break;

            }
            vd::ExternalEvent * response = new vd::ExternalEvent ("status");
            response << vd::attribute ("name", std::string("status"));
            response << vd::attribute ("value", responseValue);
            output.addEvent (response);
        }
    }

    virtual vv::Value* observation(const vd::ObservationEvent& /*event*/) const
    {
        return 0;
    }

private:
    int mResult;
};

} // namespace vle example

DECLARE_NAMED_DYNAMICS(collector_test, model::CollectorTest)
