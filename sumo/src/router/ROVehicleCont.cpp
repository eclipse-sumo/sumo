//---------------------------------------------------------------------------//
//                        ROVehicleCont.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.5  2004/01/12 15:39:35  dkrajzew
// reproduces changes to NamedObjectsMap
//
// Revision 1.4  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/NamedObjectCont.h>
#include <queue>
#include "ROVehicle.h"
#include "ROHelper.h"
#include "ROVehicleCont.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROVehicleCont::ROVehicleCont()
{
}


ROVehicleCont::~ROVehicleCont()
{
}


priority_queue<ROVehicle*,
    std::vector<ROVehicle*>, ROHelper::VehicleByDepartureComperator> &
ROVehicleCont::sort()
{
    _sorted =
        priority_queue<ROVehicle*,
            std::vector<ROVehicle*>,
            ROHelper::VehicleByDepartureComperator>();
    const std::vector<ROVehicle*> &v = getVector();
    for(std::vector<ROVehicle*>::const_iterator i=v.begin(); i!=v.end(); i++) {
        _sorted.push(*i);
    }
    return _sorted;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


