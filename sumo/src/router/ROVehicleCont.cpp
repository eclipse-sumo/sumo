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
// Revision 1.4  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
//


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

using namespace std;

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
//    _sorted.reserve(_cont.size());
    _sorted =
        priority_queue<ROVehicle*,
            std::vector<ROVehicle*>,
            ROHelper::VehicleByDepartureComperator>();
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        _sorted.push((*i).second);
    }
    return _sorted;
}


void
ROVehicleCont::eraseVehicle(ROVehicle *v)
{
    std::string id = v->getID();
    myCont::iterator i = _cont.find(id);
    delete (*i).second;
    _cont.erase(i);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROVehicleCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


