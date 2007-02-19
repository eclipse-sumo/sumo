/****************************************************************************/
/// @file    ROHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some helping functions
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
#ifndef ROHelper_h
#define ROHelper_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <functional>
#include "ROVehicle.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROHelper
 * Some helping methods and classes
 */
class ROHelper
{
public:
    /// Class to sort vehicles (ROVehicle) by their departure time
class VehicleByDepartureComperator :
                public std::less<ROVehicle*>
    {
    public:
        /// Constructor
        explicit VehicleByDepartureComperator()
        { }

        /// Destructor
        ~VehicleByDepartureComperator()
        { }

        /// Comparing operator
        bool operator()(ROVehicle *veh1, ROVehicle *veh2) const
        {
            return veh1->getDepartureTime()>veh2->getDepartureTime();
        }
    };
};


#endif

/****************************************************************************/

