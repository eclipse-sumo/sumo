/****************************************************************************/
/// @file    MSInternalJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junction.
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
#ifndef MSInternalJunction_h
#define MSInternalJunction_h


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
 * @class MSInternalJunction
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
#ifdef HAVE_INTERNAL_LANES
class MSInternalJunction : public MSLogicJunction
{
public:
    /** Use this constructor only. */
    MSInternalJunction(std::string id, const Position2D &position,
                       LaneCont incoming,
                       LaneCont internal);

    /// Destructor.
    virtual ~MSInternalJunction();

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    bool clearRequests();
    void postloadInit() throw(ProcessError);

    /** Sets the information which vehicles may drive */
    virtual bool setAllowed();

private:
    /// @brief Invalidated copy constructor.
    MSInternalJunction(const MSInternalJunction&);

    /// @brief Invalidated assignment operator.
    MSInternalJunction& operator=(const MSInternalJunction&);

};


#endif
#endif

/****************************************************************************/

