#ifndef MSDETECTORHALTINGCONTAINERWRAPPER_H
#define MSDETECTORHALTINGCONTAINERWRAPPER_H

/**
 * @file   MSDetectorHaltingContainerWrapper.h
 * @author Christian Roessel
 * @date   Started Fri Sep 26 19:11:26 2003
 * @version $Id$
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//


#include "MSDetectorContainerWrapper.h"
#include "MSDetectorOccupancyCorrection.h"
#include "MSUnit.h"
#include "MSUpdateEachTimestep.h"
#include <list>

class MSVehicle;

template< class WrappedContainer >
struct MSDetectorHaltingContainerWrapper :
    public MSDetectorContainerWrapper< WrappedContainer >,
    public MSUpdateEachTimestep<
    MSDetectorHaltingContainerWrapper< WrappedContainer > >
{
    typedef typename WrappedContainer::iterator HaltingsIt;
    typedef typename WrappedContainer::const_iterator HaltingsConstIt;
    typedef WrappedContainer InnerContainer;


    MSDetectorHaltingContainerWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection,
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold,
        MSUnit::Cells jamDistThreshold )
        : MSDetectorContainerWrapper< WrappedContainer >( occupancyCorrection),
//           MSUpdateEachTimestep< MSDetectorContainerWrapperBase >(),
          MSUpdateEachTimestep< MSDetectorHaltingContainerWrapper< WrappedContainer > >(),
          timeThresholdM( timeThreshold ),
          speedThresholdM( speedThreshold ),
          jamDistThresholdM( jamDistThreshold )
        {}

    void updateEachTimestep( void )
        {
            // set isHaltingM and haltingDurationM
            typedef typename WrappedContainer::iterator ContainerIt;
            for ( ContainerIt haltIt = containerM.begin();
                  haltIt != containerM.end(); ++haltIt ) {
                if ( haltIt->vehM->speed() >= speedThresholdM ) {
                    haltIt->timeBelowSpeedThresholdM = 0;
                    haltIt->isHaltingM = false;
                    haltIt->haltingDurationM = 0.0;
                }
                else {
                    haltIt->timeBelowSpeedThresholdM++;
                    if ( haltIt->timeBelowSpeedThresholdM > timeThresholdM ) {
                        if ( ! haltIt->isHaltingM ) {
                            haltIt->isHaltingM = true;
                            // time to detect halting contributes to
                            // halting-duration
                            haltIt->haltingDurationM =
                                haltIt->timeBelowSpeedThresholdM++;
                        }
                        else {
                            haltIt->haltingDurationM++;
                        }
                    }
                }
            }

            // set isInJamM
            if ( containerM.size() >= 1 ) {
                containerM.begin()->isInJamM = false;
            }
            if ( containerM.size() > 1 ) {
                for ( ContainerIt jamIt = containerM.begin();
                      jamIt != --containerM.end(); /* empty */ ){
                    ContainerIt rearIt = jamIt;
                    MSVehicle* rear = jamIt->vehM;
                    MSVehicle* front = (++jamIt)->vehM;
                    if ( front->pos() - front->length() - rear->pos() <=
                         jamDistThresholdM ) {
                        if ( rearIt == containerM.begin() ) {
                            rearIt->isInJamM = true;
                        }
                        jamIt->isInJamM = true;
                    }
                    else {
                        jamIt->isInJamM = false;
                    }
                }
            }
        }

    MSUnit::Steps timeThresholdM;
    MSUnit::CellsPerStep speedThresholdM;
    MSUnit::Cells jamDistThresholdM;
};

namespace DetectorContainer
{
    struct Halting
    {
        Halting( MSVehicle* veh )
            : vehM( veh ),
              timeBelowSpeedThresholdM( 0 ),
              isHaltingM( false ),
              isInJamM( false ),
              haltingDurationM( 0 )
            {}
        MSVehicle* vehM;
        MSUnit::Steps timeBelowSpeedThresholdM;
        bool isHaltingM;
        bool isInJamM;
        MSUnit::Steps haltingDurationM;
    };

    typedef MSDetectorHaltingContainerWrapper<
        std::list< Halting > > Haltings;

    typedef MSUpdateEachTimestep< Haltings > UpdateHaltings;
}

namespace Predicate
{
	// specialization
    template<>
    struct PosGreaterC< DetectorContainer::Halting > :
        public std::binary_function< DetectorContainer::Halting,
                                     double, bool >
    {
        bool operator() ( const DetectorContainer::Halting& item,
                          double pos ) const {
            return item.vehM->pos() > pos;
        }
    };
    
    // specialization
    template<>
    struct VehEqualsC< DetectorContainer::Halting > :
        public std::binary_function< DetectorContainer::Halting,
                                     MSVehicle*, bool >
    {
        bool operator() ( DetectorContainer::Halting item,
                          const MSVehicle* veh ) const {
            return item.vehM == veh;
        }
    };
}


#endif // MSDETECTORHALTINGCONTAINER_H

// Local Variables:
// mode:C++
// End:
