#ifndef NBCont_h
#define NBCont_h
/***************************************************************************
                          NBCont.h
			  Some list definitions
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.5  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.4  2003/03/20 16:23:08  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:58:51  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/06/11 16:00:39  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.1  2002/04/09 12:22:52  dkrajzew
// extracted the definitions of basic container types
//
//

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdge;


/* =========================================================================
 * container definitions
 * ======================================================================= */
/** structure specifying a certain lane on a certain edge */
class EdgeLane {
public:
    /// The according edge (semantics may change)
    NBEdge *edge;
    /// The according lane (semantics may change)
    size_t lane;

	friend bool operator==(const EdgeLane &lhs, const EdgeLane &rhs) {
		return lhs.edge==rhs.edge && lhs.lane==rhs.lane;
	}
};


/** container for (sorted) lanes of edges */
typedef std::vector<EdgeLane> EdgeLaneVector;


/** container for (sorted) edges */
typedef std::vector<NBEdge*> EdgeVector;


/** container for (sorted) lanes.
    The lanes are sorted from rightmost (id=0) to leftmost (id=nolanes-1) */
typedef std::vector<int> LaneVector;




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

