#ifndef ROHelper_h
#define ROHelper_h
//---------------------------------------------------------------------------//
//                        ROHelper.h -
//  Some helping functions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2006/01/26 08:37:23  dkrajzew
// removed warnings 4786
//
// Revision 1.5  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

#include <functional>
#include "ROVehicle.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROHelper
 * Some helping methods and classes
 */
class ROHelper {
public:
    /// Class to sort vehicles (ROVehicle) by their departure time
    class VehicleByDepartureComperator :
                public std::less<ROVehicle*> {
    public:
        /// Constructor
        explicit VehicleByDepartureComperator() { }

        /// Destructor
        ~VehicleByDepartureComperator() { }

        /// Comparing operator
        bool operator()(ROVehicle *veh1, ROVehicle *veh2) const {
            return veh1->getDepartureTime()>veh2->getDepartureTime();
        }
    };
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

