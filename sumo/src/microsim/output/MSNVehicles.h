#ifndef MSNVEHICLES_H
#define MSNVEHICLES_H

/**
 * @file   MSNVehicles.h
 * @author Christian Roessel
 * @date   Started Wed Oct 15 13:29:34 2003
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

#include <string>
#include "MSDetectorCounterContainerWrapper.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSNVehicles
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::Count Container;

    MSNVehicles( const Container& counter );
    MSNVehicles( SUMOReal, const Container& counter );

    virtual ~MSNVehicles( void )
        {}

    DetectorAggregate getDetectorAggregate( void );

    static std::string getDetectorName( void )
        {
            return "nE2Vehicles";
        }

private:
    const Container& counterM;

    MSNVehicles();
    MSNVehicles( const MSNVehicles& );
    MSNVehicles& operator=( const MSNVehicles& );

};

#endif // MSNVEHICLES_H

// Local Variables:
// mode:C++
// End:
