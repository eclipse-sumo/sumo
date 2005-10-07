#ifndef ROVehicleCont_h
#define ROVehicleCont_h
//---------------------------------------------------------------------------//
//                        ROVehicleCont.h -
//  A container for vehicles
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
// Revision 1.10  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.7  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.6  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.5  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
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

#include <string>
#include <map>
#include <queue>
#include <vector>
#include "ROVehicle.h"
#include <utils/helpers/NamedObjectCont.h>
#include "ROHelper.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicleCont
 * A map of vehicle-ids to the vehicles themselves. Additionally supplies
 * a list of vehicles where these are sorted by their departure times.
 */
class ROVehicleCont
    : public NamedObjectCont<ROVehicle*> {
public:
    /// Constructor
    ROVehicleCont();

    /// Destructor
    ~ROVehicleCont();

    /// Returns the list of sorted vehicles
    std::priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator> &sort();

private:
    /// The sorted vehicle list
    std::priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator> _sorted;

private:
    /// we made the copy constructor invalid
    ROVehicleCont(const ROVehicleCont &src);

    /// we made the assignment operator invalid
    ROVehicleCont &operator=(const ROVehicleCont &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

