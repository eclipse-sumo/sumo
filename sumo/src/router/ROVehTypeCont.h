/****************************************************************************/
/// @file    ROVehTypeCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container or vehicle types
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
#ifndef ROVehTypeCont_h
#define ROVehTypeCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include "ROVehicleType.h"
#include <utils/helpers/NamedObjectCont.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehTypeCont
 * A vehicle type container. Allows the retrieval of a default type.
 */
class ROVehTypeCont :
            public NamedObjectCont<ROVehicleType*>
{
public:
    /// Constructor
    ROVehTypeCont();

    /// Destructor
    ~ROVehTypeCont();

private:
    /// we made the copy constructor invalid
    ROVehTypeCont(const ROVehTypeCont &src);

    /// we made the assignment operator invalid
    ROVehTypeCont &operator=(const ROVehTypeCont &src);
};


#endif

/****************************************************************************/

