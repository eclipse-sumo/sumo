#ifndef MSLDDETECTOR_H
#define MSLDDETECTOR_H

///
/// @file    MSLDDetector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:05 CET
/// @version
///
/// @brief
///
///

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <deque>
#include <microsim/MSUnit.h>
#include "MSLDDetectorInterface.h"
#include <microsim/MSEventControl.h>
#include <utils/helpers/SimpleCommand.h>
#include <string>
#include <algorithm>

namespace LD
{
    /// This class is part of the detector-framework which consists of
    /// MSMeanDetector or MSSumDetector, one out of LD::MSDetector,
    /// TD::MSDetector or ED::MSDetector and a ConcreteDetector,
    /// e.g. MSDensity.  This LD::MSDetector defines methods and
    /// members special to the LD detectors.
    template< class ConcreteDetector >
    class MSDetector
        :
        public LD::MSDetectorInterface,
        public ConcreteDetector
    {
    public:

        /// Type of the detected quantity.
        typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
        /// Type of the "vehicle-container".
        typedef typename ConcreteDetector::Container DetectorContainer;

        /// The struct that is stored when a vehicle leaves the detector. It
        /// consists of the leave-time and a value.
        struct TimeValue
        {
            /// Ctor.
            ///
            /// @param leaveSecond The second when the vehicle left the
            /// detector.
            /// @param value The corresponding value.
            TimeValue( MSUnit::Seconds leaveSecond
                       , DetAggregate value )
                : leaveSecM( leaveSecond )
                , valueM( value )
                {}
            MSUnit::Seconds leaveSecM; ///< The second when the
                                       ///vehicle left the detector.
            DetAggregate valueM; ///< The corresponding value.
        };

        /// Type to the container that holds the detectors quantities.
        typedef std::deque< TimeValue > AggregatesCont;
        /// Iterator to the container that holds the detectors quantities.
        typedef typename AggregatesCont::iterator AggregatesContIter;

        /// Adds a value to the container of detected quantities. This
        /// method is called if a vehicle leave the detector.
        ///
        /// @param veh The leaving vehicle.
        ///
        void leave( MSVehicle& veh )
            {
                if ( ! hasVehicle( veh ) ) {
                    // vehicle left detector but did not enter it.
                    return;
                }
                aggregatesM.push_back(
                    TimeValue((MSUnit::Seconds) MSNet::getInstance()->simSeconds(),
                              getValue( veh ) ) );
            }

        /// Get the aggregated value of the detector. This method is
        /// defined in MSSumDetector or MSMeanDetetcor.
        ///
        /// @param lastNSeconds Length of the aggregation intervall
        /// (now-lastNSeconds, now].
        ///
        /// @return The aggregated value, sampled over lastNSeconds.
        ///
        virtual DetAggregate getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    protected:

        /// Ctor.  Starts old-data-removal.
        ///
        /// @param id The detector's id.
        /// @param deleteDataAfterSeconds The old-data-removal interval.
        /// @param container "Vehicle"-container handed to the
        /// ConcreteDetector.
        MSDetector( std::string id
                    , MSUnit::Seconds deleteDataAfterSeconds
                    , const DetectorContainer& container )
            :
            LD::MSDetectorInterface( id )
            , ConcreteDetector( container )
            , aggregatesM()
            , deleteDataAfterStepsM(
                MSUnit::getInstance()->getIntegerSteps(
                    deleteDataAfterSeconds ) )
            {
                startOldDataRemoval();
            }

        /// Ctor as above, but container is not const.
        MSDetector( std::string id
                    , MSUnit::Seconds deleteDataAfterSeconds
                    , DetectorContainer& container )
            :
            LD::MSDetectorInterface( id )
            , ConcreteDetector( container )
            , aggregatesM()
            , deleteDataAfterStepsM(
                MSUnit::getInstance()->getIntegerSteps(
                    deleteDataAfterSeconds ) )
            {
                startOldDataRemoval();
            }

        /// Dtor. Cleares the detector-quantities-container.
        virtual ~MSDetector( void )
            {
                aggregatesM.clear();
            }

        /// Predicate for searching in a container<TimeValue>.
        struct TimeLesser :
            public std::binary_function< TimeValue, MSUnit::Seconds, bool >
        {
            bool operator()( const TimeValue& aTimeValue,
                             MSUnit::Seconds timeBound ) const
                {
                    return aTimeValue.leaveSecM < timeBound;
                }
        };

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
            MSUnit::Steps lastNTimesteps )
            {
                return std::lower_bound(
                    aggregatesM.begin(), aggregatesM.end(),
                    getStartTime( lastNTimesteps ),
                    TimeLesser() );
            }

        /// Helper for getAggrContStartIterator() that calculates the
        /// time (now - lastNTimesteps) in seconds.
        ///
        /// @param lastNTimesteps Timesteps to substract.
        ///
        /// @return 0 if now - lastNTimesteps is negative. Else the
        /// corresponding time in seconds.
        ///
        MSUnit::Seconds getStartTime( MSUnit::Steps lastNTimesteps )
            {
                MSUnit::Steps timestep =
                    MSNet::getInstance()->getCurrentTimeStep() - lastNTimesteps;
                if ( timestep < 0 ) {
                    return MSUnit::Seconds(0);
                }
                return MSUnit::getInstance()->getSeconds( timestep );
            }

        /// Call once from ctor to initialize the recurring call to
        /// freeContainer() via the MSEventControl mechanism.
        void startOldDataRemoval( void )
            {
                // start old-data removal through MSEventControl
                Command* deleteData = new SimpleCommand< MSDetector >(
                    this, &MSDetector::freeContainer );
                MSEventControl::getEndOfTimestepEvents()->addEvent(
                    deleteData,
                    deleteDataAfterStepsM,
                    MSEventControl::ADAPT_AFTER_EXECUTION );
            }

        /// Frees the AggregatesCont aggregatesM so that container
        /// elements that are collected more than
        /// deleteDataAfterStepsM ago will vanish.
        ///
        /// @return deleteDataAfterStepsM to restart this removal via
        /// the MSEventControl mechanism.
        ///
        SUMOTime freeContainer( void )
            {
                AggregatesContIter end =
                    getAggrContStartIterator( (MSUnit::Steps) deleteDataAfterStepsM );
                aggregatesM.erase( aggregatesM.begin(), end );
                return deleteDataAfterStepsM;
            }

        AggregatesCont aggregatesM; ///< Container holding the
                                    ///detected quantities.

    private:

        MSUnit::IntSteps deleteDataAfterStepsM; ///< Time between
                                                ///calls to
                                                ///freeContainer().

    };

} // end namespace LD


// Local Variables:
// mode:C++
// End:

#endif // MSLDDETECTOR_H
