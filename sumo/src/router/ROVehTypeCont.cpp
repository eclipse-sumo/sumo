//---------------------------------------------------------------------------//
//                        ROVehTypeCont.cpp -
//  A container or vehicle types
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
#include <map>
#include "ROVehicleType.h"
#include "ROVehTypeCont.h"
#include <utils/common/NamedObjectCont.h>

using namespace std;


ROVehTypeCont::ROVehTypeCont(ROVehicleType *defType)
    : _defaultType(defType)
{
}


ROVehTypeCont::~ROVehTypeCont()
{
    delete _defaultType;
}


ROVehicleType *
ROVehTypeCont::getDefault() const {
    return _defaultType;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROVehTypeCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


