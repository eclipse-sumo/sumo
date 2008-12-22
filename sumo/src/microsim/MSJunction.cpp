/****************************************************************************/
/// @file    MSJunction.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junctions.
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class declarations
// ===========================================================================
class MSLink;


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definition
// ===========================================================================
MSJunction::MSJunction(std::string id, const Position2D &position)
        : myID(id), myPosition(position)
{}


MSJunction::~MSJunction()
{}


const Position2D &
MSJunction::getPosition() const
{
    return myPosition;
}


void
MSJunction::postloadInit() throw(ProcessError)
{}


const std::string &
MSJunction::getID() const
{
    return myID;
}


GUIJunctionWrapper *
MSJunction::buildJunctionWrapper(GUIGlObjectStorage &)
{
    return 0;//!!!
}


/****************************************************************************/

