/****************************************************************************/
/// @file    ROJTRHelpers.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// A set of helping functions
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
#ifndef ROJTRHelpers_h
#define ROJTRHelpers_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class ROJTREdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTRHelpers
 * Some functions commonly used within the junction-percentage router.
 */
class ROJTRHelpers
{
public:
    /** @brief Parses the names of given edges as a list of edge names, adds the edges into the container
        It is assumed, the names are divided by a ';' */
    static void parseROJTREdges(RONet &net, std::set<ROJTREdge*> &into,
                                    const std::string &chars);

};


#endif

/****************************************************************************/

