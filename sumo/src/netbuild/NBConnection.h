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
// Revision 1.3  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.2  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
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

    /// replaces the to-edge by the one given
    bool replaceTo(NBEdge *which, NBEdge *by);

    /// checks whether the edges are still valid
    bool check();

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
    NBEdge *checkFrom();

    /// Checks whether the to-edge is still valid
    NBEdge *checkTo();

private:
    /// The from- and the to-edges
    NBEdge *myFrom, *myTo;

    /// The names of both edges, needed for verification of validity
    std::string myFromID, myToID;

    /// The lanes; may be -1 if no certain lane was specified
    int myFromLane, myToLane;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBConnection.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

