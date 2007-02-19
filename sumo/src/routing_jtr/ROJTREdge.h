/****************************************************************************/
/// @file    ROJTREdge.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// An edge the router may route through
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
#ifndef ROJTREdge_h
#define ROJTREdge_h
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

#include <string>
#include <map>
#include <vector>
#include <utils/router/FloatValueTimeLine.h>
#include <router/ROEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTREdge
 * A router's edge extended by the definition about the probability a
 *  vehicle's probabilities to choose a certain following edge over time.
 */
class ROJTREdge : public ROEdge
{
public:
    /// Constructor
    ROJTREdge(const std::string &id, int index);

    /// Desturctor
    ~ROJTREdge();

    /// Adds information about a connected edge
    void addFollower(ROEdge *s);

    /// adds the information about the percentage of using a certain follower
    void addFollowerProbability(ROJTREdge *follower,
                                SUMOTime begTime, SUMOTime endTime, SUMOReal probability);

    /// Returns the next edge to use
    ROJTREdge *chooseNext(SUMOTime time) const;

    /// Sets the turning definition defaults
    void setTurnDefaults(const std::vector<SUMOReal> &defs);

private:
    /// Definition of a map that stores the probabilities of using a certain follower over time
    typedef std::map<ROJTREdge*, FloatValueTimeLine*> FollowerUsageCont;

    /// Storage for the probabilities of using a certain follower over time
    FollowerUsageCont myFollowingDefs;

    /// The defaults for turnings
    std::vector<SUMOReal> myParsedTurnings;

private:
    /// we made the copy constructor invalid
    ROJTREdge(const ROJTREdge &src);

    /// we made the assignment operator invalid
    ROJTREdge &operator=(const ROJTREdge &src);

};


#endif

/****************************************************************************/

