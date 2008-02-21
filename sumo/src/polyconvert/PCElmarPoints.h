/****************************************************************************/
/// @file    PCElmarPoints.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader of pois stored in elmar-format
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
#ifndef PCElmarPoints_h
#define PCElmarPoints_h


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
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCElmarPoints
 * @brief A reader of pois stored in elmar-format
 *
 * Reads pois stored in "pointcollection.txt",
 *  applies the given projection and network offset and stores the so build
 *  pois into the given map.
 * @todo Use a LineReader instead of getline
 */
class PCElmarPoints
{
public:
    /// Constructor
    PCElmarPoints(PCPolyContainer &toFill,
                  PCTypeMap &tm);

    /// Destructor
    ~PCElmarPoints();

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

