/****************************************************************************/
/// @file    ROAbstractRouter.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Definition of an abstract routing class
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
#ifndef ROAbstractRouter_h
#define ROAbstractRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOAbstractRouter.h>
#include <string>
#include <functional>
#include <vector>
#include <set>
#include <utils/common/InstancePool.h>
#include "ROEdgeVector.h"
#include "ROEdge.h"


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROAbstractRouter
 */
typedef SUMOAbstractRouter<ROEdge,ROVehicle> ROAbstractRouter;


#endif

/****************************************************************************/

