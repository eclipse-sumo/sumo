/**
 * @file   MSNVehicles.cpp
 * @author Christian Roessel
 * @date   Started Wed Oct 15 13:33:22 2003
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 11:09:33  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/15 07:18:59  dkrajzew
// code style applied
//
//
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

#include "MSNVehicles.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * used namespaces
 * ======================================================================= */

using namespace std;

MSNVehicles::MSNVehicles( const DetectorContainer::Count& counter ) :
    counterM( counter )
{}

MSNVehicles::MSNVehicles( SUMOReal,
                          const DetectorContainer::Count& counter ) :
    counterM( counter )
{}

MSNVehicles::DetectorAggregate
MSNVehicles::getDetectorAggregate( void ) // [veh]
{
    SUMOReal nVehOnDet = counterM.vehicleCountM;
    return nVehOnDet;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
