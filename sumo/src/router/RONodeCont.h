/****************************************************************************/
/// @file    RONodeCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for router's nodes
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
#ifndef RONodeCont_h
#define RONodeCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "RONode.h"
#include <utils/common/NamedObjectCont.h>


// ===========================================================================
// class definitions
// ===========================================================================
typedef NamedObjectCont<RONode*> RONodeCont;


#endif

/****************************************************************************/

