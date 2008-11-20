/****************************************************************************/
/// @file    PCElmar.h
/// @author  Danilot Teta Boyom
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A reader of polygons stored in elmar-format
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
#ifndef PCElmar_h
#define PCElmar_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "PCPolyContainer.h"
#include "PCTypeMap.h"


// ===========================================================================
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCElmar
 * @brief A reader of polygons stored in elmar-format
 *
 * Reads polygons stored in "...myPolygons.txt" or "...myWater_polygons.txt",
 *  applies the given projection and network offset and stores the so build
 *  polygons into the given map.
 */
class PCElmar
{
public:
    /// Constructor
    PCElmar(PCPolyContainer &toFill, PCTypeMap &tm);

    /// Destructor
    ~PCElmar();

    /// loads Elmar's data parsed from GDF
    void load(OptionsCont &oc);


private:
    /// The container to store the converted polygons into
    PCPolyContainer &myCont;

    /// The type map to use
    PCTypeMap &myTypeMap;

};


#endif

/****************************************************************************/

