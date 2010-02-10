/****************************************************************************/
/// @file    NGEdge.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// A netgen-representation of an edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <algorithm>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include "NGEdge.h"
#include "NGNode.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NGEdge-definitions
// ---------------------------------------------------------------------------
NGEdge::NGEdge(const std::string &id, NGNode *startNode, NGNode *endNode) throw()
        : myID(id), myStartNode(startNode), myEndNode(endNode) {
    myStartNode->addLink(this);
    myEndNode->addLink(this);
}


NGEdge::~NGEdge() throw() {
    myStartNode->removeLink(this);
    myEndNode->removeLink(this);
}


NBEdge *
NGEdge::buildNBEdge(NBNetBuilder &nb) const throw(ProcessError) {
    return new NBEdge(
               myID, // id
               nb.getNodeCont().retrieve(myStartNode->getID()), // from
               nb.getNodeCont().retrieve(myEndNode->getID()), // to
               "netgen-default", // type
               nb.getTypeCont().getDefaultSpeed(),
               nb.getTypeCont().getDefaultNoLanes(),
               nb.getTypeCont().getDefaultPriority()
           );
}


/****************************************************************************/

