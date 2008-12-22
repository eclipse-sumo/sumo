/****************************************************************************/
/// @file    MSRightOfWayJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// A junction with right-of-way - rules
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
#ifndef MSRightOfWayJunction_h
#define MSRightOfWayJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"
#include <bitset>
#include <vector>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSJunctionLogic;
class findCompetitor;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRightOfWayJunction
 * @brief A junction with right-of-way - rules
 * 
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
class MSRightOfWayJunction : public MSLogicJunction
{
public:
    /** Use this constructor only. */
    MSRightOfWayJunction(std::string id, const Position2D &position,
                         LaneCont incoming,
#ifdef HAVE_INTERNAL_LANES
                         LaneCont internal,
#endif
                         MSJunctionLogic* logic);

    /// Destructor.
    virtual ~MSRightOfWayJunction();

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    bool clearRequests();

    /** Sets the information which vehicles may drive */
    virtual bool setAllowed();

    void postloadInit() throw(ProcessError);

protected:
    /// Search for deadlock-situations and eleminate them.
    virtual void deadlockKiller();

    /** the type of the junction (its logic) */
    MSJunctionLogic* myLogic;

private:
    /// @brief Invalidated copy constructor.
    MSRightOfWayJunction(const MSRightOfWayJunction&);

    /// Invalidated assignment operator.
    MSRightOfWayJunction& operator=(const MSRightOfWayJunction&);

};


#endif

/****************************************************************************/

