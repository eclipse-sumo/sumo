#ifndef ROVehTypeCont_h
#define ROVehTypeCont_h
//---------------------------------------------------------------------------//
//                        ROVehTypeCont.h -
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
// $Log$
// Revision 1.4  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
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
#include <utils/common/NamedObjectCont.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehTypeCont
 * A vehicle type container. Allows the retrieval of a default type.
 */
class ROVehTypeCont :
        public NamedObjectCont<ROVehicleType*> {
public:
    /// Constructor
    ROVehTypeCont(ROVehicleType *defType);

    /// Destructor
    ~ROVehTypeCont();

    /// Returns the default vehicle type
    ROVehicleType *getDefault() const;

private:
    /// The default vehicle type
    ROVehicleType *_defaultType;

private:
    /// we made the copy constructor invalid
    ROVehTypeCont(const ROVehTypeCont &src);

    /// we made the assignment operator invalid
    ROVehTypeCont &operator=(const ROVehTypeCont &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

