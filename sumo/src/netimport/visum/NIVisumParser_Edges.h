/****************************************************************************/
/// @file    NIVisumParser_Edges.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-edges
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
#ifndef NIVisumParser_Edges_h
#define NIVisumParser_Edges_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>

#include "NIVisumLoader.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_Edges
 * Parses edges from their visum format.
 */
class NIVisumParser_Edges :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_Edges(NIVisumLoader &parent, NBNodeCont &nc,
                        NBEdgeCont &ec, NBTypeCont &tc, const std::string &dataName,
                        bool useVisumPrio);

    /// Destructor
    ~NIVisumParser_Edges();

protected:
    /** @brief Parses a single edge using data from the inherited NamedColumnsParser. */
    void myDependentReport();

private:
    /// Checks whether the nodes are ok
    bool checkNodes(NBNode *from, NBNode *to) const;

    /// Returns the length of the edge, either read from the parameter or computed from the geometry
    SUMOReal getLength(NBNode *from, NBNode *to) const;

    /// Returns the maximum allowed speed on this edge, either read from the parameter or the one of the type the edge is of
    SUMOReal getSpeed(const std::string &type) const;

    /// Returns the number of the lanes the edge has; Either read from the parameter or the number of lanes the edges of this type have
    int getNoLanes(const std::string &type) const;

    /// Build the edge checking the insertion
    void insertEdge(const std::string &id,  NBNode *from, NBNode *to,
                    const std::string &type, SUMOReal speed, int nolanes, SUMOReal length,
                    int prio, bool oneway) const;

private:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBTypeCont &myTypeCont;
    std::vector<std::string > myTouchedEdges;
    bool myUseVisumPrio;

};


#endif

/****************************************************************************/

