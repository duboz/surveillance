/**
 * @file 
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

#include <boost/lexical_cast.hpp>
#include <vle/value.hpp>
#include <vle/devs.hpp>
#include "smart_surv.hpp"
#include <vle/devs/DynamicsDbg.hpp>
namespace vd = vle::devs;
namespace vv = vle::value;

namespace model {

  smart_surv::smart_surv(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : model::ActiveCollector(init, events)
  {
    mPrecision = vv::toDouble(events.get("relative_precision"));
    mSampleMin = vv::toDouble(events.get("echProp"));
  }

  smart_surv::~smart_surv()
  {
  }

  vv::Value* smart_surv::observation(
                       const vd::ObservationEvent& event) const
  {
    
    if (event.onPort("smart_survview")){ 
        vv::Map* tmpResult = new vv::Map();
        std::map<std::string, std::string>::const_iterator it;

        for ( it = mapResult.begin(); it != mapResult.end(); ++it ) {
          tmpResult->addString(it->first, it->second);
        }
        return tmpResult;
    }
    if (event.onPort("prevalence")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        return buildDouble(mPrevalence);
    }
    if (event.onPort("proportion")){
        //std::cout<<"Prev is "<<mPrevalence<<"\n";
        double doubleSize = mSampleSize;
        return buildDouble(doubleSize/mNbModel);
    }

    if (event.onPort("incidence")){
        //std::cout<<"incidence is "<<mIncidence<<"\n";
        return buildDouble(mIncidence);
    }
    else return 0;

  }

  //Generic functions for active surveillance
  void smart_surv::connectToNodes(vd::ExternalEventList& output) const 
  {
          vd::ExternalEvent* connectionRequest = 
                new vd::ExternalEvent("connectTo");
          vv::Set linkTo;
          std::vector <int> sample;
          int i=0;
          while (sample.size()<mNbModel) { 
              sample.push_back(i);
              i++;
          }
          for (int j = 0; j < sample.size(); j++) {
              int k = rand().getInt(0,sample.size()-1);
              int temp = sample[j];
              sample[j]=sample[k];
              sample[k]=temp;
          }
          while (sample.size()>mSampleSize)
              sample.pop_back();

          for (std::vector<int>::iterator i = sample.begin(); i!=sample.end(); ++i) {
            std::string vetName = 
              mPrefix + "-" + boost::lexical_cast<std::string>(*i);
            linkTo.addString(vetName);
          }
          connectionRequest << vd::attribute("modelName", 
                                             std::string (getModelName()));
          connectionRequest << vd::attribute("linkTo", linkTo);
          output.addEvent(connectionRequest);
  }
  void smart_surv::computeSampleProp()
  {
      double prev = 0.01;
      if (mPrevalence > 0)
          prev = mPrevalence;
      double infSample = std::pow(1.96,2) * prev * (1. - prev) / std::pow(prev * mPrecision,2);
      mSampleSize = std::max(1./(1./infSample + 1./mNbModel), mSampleMin);
  }

} // namespace vle example

DECLARE_DYNAMICS(model::smart_surv);
