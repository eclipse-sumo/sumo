#ifndef MSE1MEANSPEED_H
#define MSE1MEANSPEED_H

///
/// @file    MSE1MeanSpeed.h
/// @author  Christian Roessel <christian.roessel@gmx.de>
/// @date    Started Fri Jul 09 2004 21:31 CEST
/// @version
///
/// @brief
///
///

/* Copyright (C) 2004 by Christian Roessel <christian.roessel@gmx.de> */

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

#include "MSE1.h"
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSE1MeanSpeed
{
public:

    static std::string getDetectorName( void )
        {
            return "E1MeanSpeed";
        }

protected:
    typedef SUMOReal DetectorAggregate;
    typedef E1::E1Container Container;

    MSE1MeanSpeed( const Container& container )
        :
        containerM( container )
        {}

    virtual ~MSE1MeanSpeed( void )
        {}

    bool hasVehicle( MSVehicle& veh ) const
        {
            return containerM.hasVehicle( &veh );
        }

    DetectorAggregate getValue( MSVehicle& veh )
        {
            assert( &veh == containerM.vehOnDetectorM );
            // speed in [m/s]
            return containerM.vehLengthM /
                (containerM.leaveTimeM - containerM.entryTimeM);
        }

private:
    const Container& containerM;

    MSE1MeanSpeed();
    MSE1MeanSpeed( const MSE1MeanSpeed& );
    MSE1MeanSpeed& operator=( const MSE1MeanSpeed& );
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/



// Local Variables:
// mode:C++
// End:

#endif // MSE1MEANSPEED_H
