#ifndef NIVisumParser_Edges_h
#define NIVisumParser_Edges_h
/***************************************************************************
                          NIVisumParser_Edges.h
              Parser for visum-edges
                             -------------------
    project              : SUMO
    begin                : Thu, 14 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.3  2004/11/23 10:23:51  dkrajzew
// debugging
//
// Revision 1.2  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIVisumLoader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_Edges
 * Parses edges from their visum format.
 */
class NIVisumParser_Edges :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_Edges(NIVisumLoader &parent,
        const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Edges();

protected:
    /** @brief Parses a single edge using data from the inherited NamedColumnsParser. */
    void myDependentReport();

private:
    /// Checks whether the nodes are ok
    bool checkNodes(NBNode *from, NBNode *to) const;

    /// Returns the length of the edge, either read from the parameter or computed from the geometry
    double getLength(NBNode *from, NBNode *to) const;

    /// Returns the maximum allowed speed on this edge, either read from the parameter or the one of the type the edge is of
    double getSpeed(const std::string &type) const;

    /// Returns the number of the lanes the edge has; Either read from the parameter or the number of lanes the edges of this type have
    int getNoLanes(const std::string &type) const;

    /// Build the edge checking the insertion
    void insertEdge(const std::string &id,  NBNode *from, NBNode *to,
        const std::string &type, double speed, int nolanes, double length,
        int prio) const;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVisumParser_Edges.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

