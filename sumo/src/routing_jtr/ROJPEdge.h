#ifndef ROJPEdge_h
#define ROJPEdge_h
//---------------------------------------------------------------------------//
//                        ROJPEdge.h -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.6  2005/09/23 06:04:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:34  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.2  2004/07/02 09:40:36  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing (lane index added)
//
// Revision 1.1  2004/02/06 08:43:46  dkrajzew
// new naming applied to the folders (jp-router is now called jtr-router)
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
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
#include <map>
#include <vector>
#include <utils/router/FloatValueTimeLine.h>
#include <router/ROEdge.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROJPEdge
 * A router's edge extended by the definition about the probability a
 *  vehicle's probabilities to choose a certain following edge over time.
 */
class ROJPEdge : public ROEdge {
public:
    /// Constructor
    ROJPEdge(const std::string &id, int index);

    /// Desturctor
    ~ROJPEdge();

    /// Adds information about a connected edge
    void addFollower(ROEdge *s);

    /// adds the information about the percentage of using a certain follower
    void addFollowerProbability(ROJPEdge *follower,
        SUMOTime begTime, SUMOTime endTime, SUMOReal percentage);

    /// Returns the next edge to use
    ROJPEdge *chooseNext(SUMOTime time) const;

    /// Sets the turning definition defaults
    void setTurnDefaults(const std::vector<SUMOReal> &defs);

private:
    /// Definition of a map that stores the probabilities of using a certain follower over time
    typedef std::map<ROJPEdge*, FloatValueTimeLine*> FollowerUsageCont;

    /// Storage for the probabilities of using a certain follower over time
    FollowerUsageCont myFollowingDefs;

    /// The defaults for turnings
    std::vector<SUMOReal> myParsedTurnings;

private:
    /// we made the copy constructor invalid
    ROJPEdge(const ROJPEdge &src);

    /// we made the assignment operator invalid
    ROJPEdge &operator=(const ROJPEdge &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

