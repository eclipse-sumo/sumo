/****************************************************************************/
/// @file    HaveBoundary.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for objects that do have a boundary
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
#ifndef HaveBoundary_h
#define HaveBoundary_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/Boundary.h>


// ===========================================================================
// included modules
// ===========================================================================
/**
 * @class HaveBoundary
 * Class which allows the retrieval of a boundary
 */
class HaveBoundary
{
public:
    /// Constructor
    HaveBoundary() { }

    /// Destructor
    virtual ~HaveBoundary() { }

    /// Returns the object's boundary
    virtual Boundary getBoundary() const = 0;

};


#endif

/****************************************************************************/

