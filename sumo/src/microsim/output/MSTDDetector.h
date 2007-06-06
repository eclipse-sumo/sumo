/****************************************************************************/
/// @file    MSTDDetector.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 20:04 CET
/// @version $Id$
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSTDDetector_h
#define MSTDDetector_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSUnit.h>
#include "MSPredicates.h"
#include "MSTDDetectorInterface.h"
#include <microsim/MSEventControl.h>
#include <utils/helpers/WrappingCommand.h>
#include <deque>
#include <string>
#include <algorithm>
#include <functional>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;

namespace TD // timestep data
{
/// This class is part of the detector-framework which consists of
/// MSMeanDetector or MSSumDetector, one out of LD::MSDetector,
/// TD::MSDetector or ED::MSDetector and a ConcreteDetector,
/// e.g. MSDensity. This TD::MSDetector defines methods and
/// members special to the TD detectors.
template< class ConcreteDetector >
class MSDetector
            :
            public TD::MSDetectorInterface
            , public ConcreteDetector
{
public:

    /// Type of the detected quantity.
    typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
    /// Type of the "vehicle-container".
    typedef typename ConcreteDetector::Container DetectorContainer;

    /// Get the most recently collected element of the detected values.
    ///
    /// @return The most recently collected element of the
    /// detected values or -1 if there no values have been
    /// collected so far.
    ///
    DetAggregate getCurrent(void) const
    {
        if (aggregatesM.size()==0) {
            return -1;
        }
        return aggregatesM.back();
    }

    /// Get the aggregated value of the detector. This method is
    /// defined in MSSumDetector or MSMeanDetetcor.
    ///
    /// @param lastNSeconds Length of the aggregation intervall
    /// (now-lastNSeconds, now].
    ///
    /// @return The aggregated value, sampled over lastNSeconds.
    ///
    virtual DetAggregate getAggregate(MSUnit::Seconds lastNSeconds) = 0;

protected:

    /// Ctor for detectors using a "vehicle"-container.  Starts
    /// old-data-removal.
    ///
    /// @param id The detector's id.
    /// @param deleteDataAfterSeconds The old-data-removal interval.
    /// @param container "Vehicle"-container handed to the
    /// ConcreteDetector.
    MSDetector(std::string id,
               SUMOReal lengthInMeters,
               MSUnit::Seconds deleteDataAfterSeconds,
               const DetectorContainer& container)
            : TD::MSDetectorInterface(id)
            , ConcreteDetector(lengthInMeters, container)
            , deleteDataAfterStepsM(MSUnit::getInstance()->getIntegerSteps(
                                        deleteDataAfterSeconds))
    {
        startOldDataRemoval();
    }

    /// Ctor for detector using a helper-detector
    /// (e.g. E2QueueLengthAheadOfTrafficLightsInVehicles).
    /// Starts old-data-removal.
    ///
    /// @param id The detector's id.
    /// @param deleteDataAfterSeconds The old-data-removal interval.
    /// @param helperDetector The helper-detector handed to the
    /// ConcreteDetector.
    MSDetector(std::string id,
               SUMOReal lengthInMeters,
               MSUnit::Seconds deleteDataAfterSeconds,
               const TD::MSDetectorInterface& helperDetector)
            : TD::MSDetectorInterface(id)
            , ConcreteDetector(lengthInMeters, helperDetector)
            , deleteDataAfterStepsM(MSUnit::getInstance()->getIntegerSteps(
                                        deleteDataAfterSeconds))
    {
        if (detNameM == "") {
            detNameM = this->getDetectorName() + "Sum";
        }
        startOldDataRemoval();
    }

    /// Dtor. Cleares the detector-quantities-container.
    virtual ~MSDetector(void)
    {
        aggregatesM.clear();
    }

    /// Called every timestep by MSUpdateEachTimestep inherited
    /// from TD::MSDetectorInterface.
    ///
    /// @return Dummy to please MSVC++.
    ///
    bool updateEachTimestep(void)
    {
        aggregatesM.push_back(this->getDetectorAggregate());
        return false;
    }

    /// Call once from ctor to initialize the recurring call to
    /// freeContainer() via the MSEventControl mechanism.
    void startOldDataRemoval(void)
    {
        // start old-data removal through MSEventControl
        Command* deleteData = new WrappingCommand< MSDetector >(
                                  this, &MSDetector::freeContainer);
        MSNet::getInstance()->getEndOfTimestepEvents().addEvent(
            deleteData,
            deleteDataAfterStepsM,
            MSEventControl::ADAPT_AFTER_EXECUTION);
    }

    /// Type to the container that holds the detectors quantities.
    typedef typename std::deque< DetAggregate > AggregatesCont;
    /// Iterator to the container that holds the detectors quantities.
    typedef typename AggregatesCont::iterator AggregatesContIter;

    AggregatesCont aggregatesM; ///< Container holding the
    ///detected quantities.

    /// Get an iterator to an element of the
    /// detector-quantities-container aggregatesM that is at least
    /// lastNTimesteps "old". Used to sample in MSMeainDetector
    /// and MSSumDetector.
    ///
    /// @param lastNTimesteps Length of interval.
    ///
    /// @return Iterator to aggregatesM.
    ///
    AggregatesContIter getAggrContStartIterator(
        MSUnit::Steps lastNTimesteps)
    {
        AggregatesContIter start = aggregatesM.begin();
        typedef typename AggregatesCont::difference_type Distance;
        Distance steps = static_cast< Distance >(lastNTimesteps);
        if (aggregatesM.size() > lastNTimesteps) {
            start = aggregatesM.end() - steps;
        }
        return start;
    }

    /// Frees the AggregatesCont aggregatesM so that container
    /// elements that are collected more than
    /// deleteDataAfterStepsM ago will vanish.
    ///
    /// @return deleteDataAfterStepsM to restart this removal via
    /// the MSEventControl mechanism.
    ///
    SUMOTime freeContainer(SUMOTime)
    {
        AggregatesContIter end = aggregatesM.end();
        if (aggregatesM.size() > deleteDataAfterStepsM) {
            end -= deleteDataAfterStepsM;
            aggregatesM.erase(aggregatesM.begin(), end);
        }
        return deleteDataAfterStepsM;
    }

private:

    MSUnit::IntSteps deleteDataAfterStepsM; ///< Time between
    ///calls to
    ///freeContainer().
};

} // end namespace TD


#endif

/****************************************************************************/

