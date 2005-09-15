#ifndef NBNBConnection_h
#define NBNBConnection_h
//---------------------------------------------------------------------------//
//                        NBConnection.h -
//  The class holds a description of a connection between two edges
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.5  2003/12/04 13:05:42  dkrajzew
// some work for joining vissim-edges
//
// Revision 1.4  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.3  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.2  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "NBEdge.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NBConnection {
public:
    /// Constructor
    NBConnection(NBEdge *from, NBEdge *to);

    /// Constructor
    NBConnection(NBEdge *from, int fromLane, NBEdge *to, int toLane);

    /// Constructor
    NBConnection(const std::string &fromID, NBEdge *from,
        const std::string &toID, NBEdge *to);

    /// Constructor
    NBConnection(const NBConnection &c);

    /// Destructor
    ~NBConnection();

    /// returns the from-edge (start of the connection)
    NBEdge *getFrom() const;

    /// returns the to-edge (end of the connection)
    NBEdge *getTo() const;

    /// replaces the from-edge by the one given
    bool replaceFrom(NBEdge *which, NBEdge *by);

    /// replaces the from-edge by the one given
    bool replaceFrom(NBEdge *which, int whichLane, NBEdge *by, int byLane);

    /// replaces the to-edge by the one given
    bool replaceTo(NBEdge *which, NBEdge *by);

    /// replaces the to-edge by the one given
    bool replaceTo(NBEdge *which, int whichLane, NBEdge *by, int byLane);

    /// checks whether the edges are still valid
    bool check(const NBEdgeCont &ec);

    /// returns the from-lane
    int getFromLane() const;

    /// returns the to-lane
    int getToLane() const;

    /// returns an od of the connection (!!! not really pretty)
    std::string getID() const;

    /// Compares both connections in order to allow sorting
    friend bool operator<(const NBConnection &c1, const NBConnection &c2);

private:
    /// Checks whether the from-edge is still valid
    NBEdge *checkFrom(const NBEdgeCont &ec);

    /// Checks whether the to-edge is still valid
    NBEdge *checkTo(const NBEdgeCont &ec);

private:
    /// The from- and the to-edges
    NBEdge *myFrom, *myTo;

    /// The names of both edges, needed for verification of validity
    std::string myFromID, myToID;

    /// The lanes; may be -1 if no certain lane was specified
    int myFromLane, myToLane;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

