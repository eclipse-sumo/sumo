/****************************************************************************/
/// @file    NBConnection.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @date    Sept 2002
/// @version $Id$
///
// The class holds a description of a connection between two edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBConnection_h
#define NBConnection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "NBEdge.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBConnection
 */
class NBConnection {
public:
    /// Constructor
    NBConnection(NBEdge* from, NBEdge* to);

    /// Constructor
    NBConnection(NBEdge* from, int fromLane, NBEdge* to, int toLane, int tlIndex = InvalidTlIndex);

    /// Constructor
    NBConnection(const std::string& fromID, NBEdge* from,
                 const std::string& toID, NBEdge* to);

    /// Constructor
    NBConnection(const NBConnection& c);

    /// Destructor
    virtual ~NBConnection();

    /// returns the from-edge (start of the connection)
    NBEdge* getFrom() const;

    /// returns the to-edge (end of the connection)
    NBEdge* getTo() const;

    /// replaces the from-edge by the one given
    bool replaceFrom(NBEdge* which, NBEdge* by);

    /// replaces the from-edge by the one given
    bool replaceFrom(NBEdge* which, int whichLane, NBEdge* by, int byLane);

    /// replaces the to-edge by the one given
    bool replaceTo(NBEdge* which, NBEdge* by);

    /// replaces the to-edge by the one given
    bool replaceTo(NBEdge* which, int whichLane, NBEdge* by, int byLane);

    /// checks whether the edges are still valid
    bool check(const NBEdgeCont& ec);

    /// returns the from-lane
    int getFromLane() const;

    /// returns the to-lane
    int getToLane() const;

    /* @brief returns the index within the controlling tls or InvalidTLIndex if this
     * link is unontrolled */
    int getTLIndex() const {
        return myTlIndex;
    }

    // @brief reset the tlIndex
    void setTLIndex(int tlIndex) {
        myTlIndex = tlIndex;
    }

    /// returns the id of the connection (!!! not really pretty)
    std::string getID() const;

    /// Compares both connections in order to allow sorting
    friend bool operator<(const NBConnection& c1, const NBConnection& c2);

    /// Comparison operator
    bool operator==(const NBConnection& c) const;

    /// Comparison operator
    bool operator!=(const NBConnection& c) const {
        return !(*this == c);
    }

    const static int InvalidTlIndex;
    const static NBConnection InvalidConnection;

private:
    /// Checks whether the from-edge is still valid
    NBEdge* checkFrom(const NBEdgeCont& ec);

    /// Checks whether the to-edge is still valid
    NBEdge* checkTo(const NBEdgeCont& ec);

private:
    /// The from- and the to-edges
    NBEdge* myFrom, *myTo;

    /// The names of both edges, needed for verification of validity
    std::string myFromID, myToID;

    /// The lanes; may be -1 if no certain lane was specified
    int myFromLane, myToLane;

    // @brief the index within the controlling tls if this connection is tls-controlled
    int myTlIndex;

};


#endif

/****************************************************************************/

