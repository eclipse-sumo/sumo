/****************************************************************************/
/// @file    ROJTREdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id: $
///
// The builder for jp-edges
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
#ifndef ROJTREdgeBuilder_h
#define ROJTREdgeBuilder_h
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

#include <router/ROAbstractEdgeBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RONet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTREdgeBuilder
 * This class builds edges that may be used by the junction-percantage
 *  router.
 */
class ROJTREdgeBuilder : public ROAbstractEdgeBuilder
{
public:
    /// Constructor
    ROJTREdgeBuilder();

    /// Destructor
    ~ROJTREdgeBuilder();

    /** @brief Builds a jp-edge */
    ROEdge *buildEdge(const std::string &name);

    /** Post process the edges */
    void setTurningDefinitions(RONet &net,
                               const std::vector<SUMOReal> &turn_defs);

private:
    /// The turn definitions
    std::vector<std::string> myNames;

};


#endif

/****************************************************************************/

