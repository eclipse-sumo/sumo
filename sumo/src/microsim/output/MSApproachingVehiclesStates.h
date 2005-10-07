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
        VehicleState( MSUnit::Cells gap2DetectorEnd,
                      MSUnit::CellsPerStep speed )
            : gap2DetectorEndM(
                MSUnit::getInstance()->getMeters( gap2DetectorEnd ) )
            , speedM(
                MSUnit::getInstance()->getMetersPerSecond( speed ) )
            {}

        MSUnit::Meters getGap2DetectorEnd( void ) const
            {
                return gap2DetectorEndM;
            }

        MSUnit::MetersPerSecond getSpeed( void ) const
            {
                return speedM;
            }

    private:
        MSUnit::Meters gap2DetectorEndM;
        MSUnit::MetersPerSecond speedM;
    };

    // the first element of the container corresponds to first vehicle
    // in driving direction
    typedef std::vector< VehicleState > DetectorAggregate;

protected:
    typedef DetectorContainer::VehiclesList Container;
    typedef Container::InnerContainer VehicleCont;

    MSApproachingVehiclesStates( MSUnit::Meters detectorEndPos,
                                 const Container& vehicleCont )
        : detectorEndPosM( MSUnit::getInstance()->getCells( detectorEndPos ) ),
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
            if ( (*vehIt)->pos() > detectorEndPosM ) {
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
                        detectorEndPosM - (*vehIt)->pos(),
                        (*vehIt)->speed() ) );
            }
            return statesM;
        }

    static std::string getDetectorName( void )
        {
            return "approachingVehiclesStates";
        }

private:
    MSUnit::Cells detectorEndPosM;
    const VehicleCont& containerM;
    DetectorAggregate statesM;
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif // MSAPPROACHINGVEHICLESSTATES_H

// Local Variables:
// mode:C++
// End:
