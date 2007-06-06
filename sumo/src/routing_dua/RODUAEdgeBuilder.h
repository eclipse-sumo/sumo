/****************************************************************************/
/// @file    RODUAEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// The builder for due-edges
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
#ifndef RODUAEdgeBuilder_h
#define RODUAEdgeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/ROAbstractEdgeBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODUAEdgeBuilder
 * This class builds edges that may be used by the dynamic user assignment-
 *  router.
 */
class RODUAEdgeBuilder : public ROAbstractEdgeBuilder
{
public:
    /// Constructor
    RODUAEdgeBuilder(bool useBoundariesOnOverride);

    /// Destructor
    ~RODUAEdgeBuilder();

    /// Builds the dua-edge
    ROEdge *buildEdge(const std::string &name);

protected:
    /// Information whether to use the first/last entry of the time in case it is too short
    bool myUseBoundariesOnOverride;

};


#endif

/****************************************************************************/

