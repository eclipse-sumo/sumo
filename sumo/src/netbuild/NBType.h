/****************************************************************************/
/// @file    NBType.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A single edge type
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
#ifndef NBType_h
#define NBType_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "NBEdge.h"


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * NBType
 * This class holds all informations about a single type of the sections/edges
 */
class NBType
{
public:
    /** parameterised constructor */
    NBType(const std::string &name, int noLanes, SUMOReal speed, int priority,
           NBEdge::EdgeBasicFunction function);

    /** destructor */
    ~NBType();

    /** the container may access the private members */
    friend class NBTypeCont;

private:
    /** the name of the type */
    std::string myName;

    /** the number of lanes of this type */
    int myNoLanes;

    /** the speed on a section/edge of this type */
    SUMOReal mySpeed;

    /** the priority of the edge/section of this type */
    int myPriority;

    /// The street's function
    NBEdge::EdgeBasicFunction myFunction;

};


#endif

/****************************************************************************/
