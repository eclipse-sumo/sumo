#ifndef MSAPPROACHINGVEHICLESSTATES_H
#define MSAPPROACHINGVEHICLESSTATES_H

/**
 * @file   MSApproachingVehiclesStates.h
 * @author Christian Roessel
 * @date   Started Thu Oct 16 14:45:00 2003
 * @version
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
// $Log$
// Revision 1.6  2006/07/06 07:18:33  dkrajzew
// applied current microsim-APIs
//
// Revision 1.5  2006/05/15 05:47:50  dkrajzew
// got rid of the cell-to-meter conversions
//
// Revision 1.5  2006/05/08 10:54:42  dkrajzew
// got rid of the cell-to-meter conversions
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSDetectorContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <string>
#include <vector>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSApproachingVehiclesStates
{
    friend class MSE2Collector;
public:

    class VehicleState
    {
    public:
        VehicleState( SUMOReal gap2DetectorEnd,
                      MSUnit::CellsPerStep speed )
            : gap2DetectorEndM(gap2DetectorEnd), speedM(MSUnit::getInstance()->getMetersPerSecond( speed ) )
            {}

        SUMOReal getGap2DetectorEnd( void ) const
            {
                return gap2DetectorEndM;
            }

        MSUnit::MetersPerSecond getSpeed( void ) const
            {
                return speedM;
            }

    private:
        SUMOReal gap2DetectorEndM;
        MSUnit::MetersPerSecond speedM;

    };

    // the first element of the container corresponds to first vehicle
    // in driving direction
    typedef std::vector< VehicleState > DetectorAggregate;

protected:
    typedef DetectorContainer::VehiclesList Container;
    typedef Container::InnerContainer VehicleCont;

    MSApproachingVehiclesStates( SUMOReal detectorEndPos,
                                 const Container& vehicleCont )
        : detectorEndPosM(detectorEndPos),
          containerM( vehicleCont.containerM ),
          statesM()
        {}

    virtual ~MSApproachingVehiclesStates( void )
        {
            statesM.clear();
        }

    const DetectorAggregate& getDetectorAggregate( unsigned nApproachingVeh )
        {
            statesM.clear();

            VehicleCont::const_iterator vehIt = containerM.begin();
            if ( (*vehIt)->getPositionOnLane() > detectorEndPosM ) {
                // first vehicle left detector partially, start with the next
                // one.
                ++vehIt;
            }
            unsigned nValidVehicles = std::distance( vehIt, containerM.end() );
            if ( nValidVehicles < nApproachingVeh ) {
                nApproachingVeh = nValidVehicles;
            }

            for ( unsigned index = 0; index < nApproachingVeh;
                  ++index, ++vehIt ) {
                statesM.push_back(
                    VehicleState(
                        detectorEndPosM - (*vehIt)->getPositionOnLane(),
                        (*vehIt)->getSpeed() ) );
            }
            return statesM;
        }

    static std::string getDetectorName( void )
        {
            return "approachingVehiclesStates";
        }

private:
    SUMOReal detectorEndPosM;
    const VehicleCont& containerM;
    DetectorAggregate statesM;
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif // MSAPPROACHINGVEHICLESSTATES_H

// Local Variables:
// mode:C++
// End:
