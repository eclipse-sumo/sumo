#ifndef MSTHRESHOLDDETECTORCONTAINER_H
#define MSTHRESHOLDDETECTORCONTAINER_H

/**
 * @file   MSDetectorContainer.h
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

#include "MSDetectorContainer.h"
#include "MSUnit.h"
#include <list>

class MSVehicle;

template< typename InnerContainer >
struct MSHaltingDetectorContainer :
    public MSDetectorContainer< InnerContainer >
{
//     typedef typename InnerContainer::value_type ContainerItem;
    typedef typename InnerContainer::iterator HaltingsIt;
    typedef typename InnerContainer::const_iterator HaltingsConstIt;
    typedef InnerContainer InnerCont;


    MSHaltingDetectorContainer( MSUnit::Steps timeThreshold,
                                MSUnit::CellsPerStep speedThreshold,
                                MSUnit::Cells jamDistThreshold )
        : timeThresholdM( timeThreshold ),
          speedThresholdM(  speedThreshold ),
          jamDistThresholdM( jamDistThreshold )
        {}

    void update( void )
        {
            // set isHaltingM
            typedef typename InnerContainer::iterator ContainerIt;
            for ( ContainerIt haltIt = containerM.begin();
                  haltIt != containerM.end(); ++haltIt ) {
                if ( haltIt->vehM->speed() >= speedThresholdM ) {
                    haltIt->timeBelowSpeedThresholdM = 0;
                    haltIt->isHaltingM = false;
                }
                else {
                    haltIt->timeBelowSpeedThresholdM++;
                    if ( haltIt->timeBelowSpeedThresholdM > timeThresholdM ) {
                        haltIt->isHaltingM = true;
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
              isInJamM( false )
            {}
        MSVehicle* vehM;
        MSUnit::Steps timeBelowSpeedThresholdM;
        bool isHaltingM;
        bool isInJamM;
    };

    typedef MSHaltingDetectorContainer< std::list< Halting > > Haltings;
}

namespace Predicate
{
    // specialization
/*    template<>
    struct PosGreaterC< DetectorContainer::Haltings > :
        public std::binary_function< DetectorContainer::Haltings,
                                     double, bool >
    {
        bool operator() ( const DetectorContainer::Halting& item,
                          double pos ) const {
            return item.vehM->pos() > pos;
        }
    };

    // specialization
    template<>
    struct VehEqualsC< DetectorContainer::Haltings > :
        public std::binary_function< DetectorContainer::Halting,
                                     MSVehicle*, bool >
    {
        bool operator() ( const DetectorContainer::Halting& item,
                          const MSVehicle* veh ) const {
            return item.vehM == veh;
        }
    };

*/
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
        public std::binary_function< DetectorContainer::Halting, MSVehicle*, bool >
    {
        bool operator() ( DetectorContainer::Halting item,
                          const MSVehicle* veh ) const {
            return item.vehM == veh;
        }
    };

}


#endif // MSTHRESHOLDDETECTORCONTAINER_H

// Local Variables:
// mode:C++
// End:
