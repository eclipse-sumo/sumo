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
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
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
 * A router's edge extended by the definition about the propability a
 *  vehicle's propabilities to choose a certain following edge over time.
 */
class ROJPEdge : public ROEdge {
public:
    /// Constructor
	ROJPEdge(const std::string &id);

    /// Desturctor
	~ROJPEdge();

    /// Adds information about a connected edge
    void addFollower(ROEdge *s);

    /// adds the information about the percentage of using a certain follower
    void addFollowerPropability(ROJPEdge *follower,
        unsigned int begTime, unsigned int endTime, float percentage);

    /// Returns the next edge to use
    ROJPEdge *chooseNext(unsigned int time) const;

    /// Sets the turning definition defaults
    void setTurnDefaults(const std::vector<float> &defs);

private:
    /// Definition of a map that stores the propabilities of using a certain follower over time
    typedef std::map<ROJPEdge*, FloatValueTimeLine*> FollowerUsageCont;

    /// Storage for the propabilities of using a certain follower over time
    FollowerUsageCont myFollowingDefs;

    /// The defaults for turnings
    std::vector<float> myParsedTurnings;

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

