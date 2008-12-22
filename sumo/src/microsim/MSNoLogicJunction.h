/****************************************************************************/
/// @file    MSNoLogicJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// logic, e.g. for exits.
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
#ifndef MSNoLogicJunction_h
#define MSNoLogicJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <bitset>
#include "MSJunction.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSNoLogicJunction
 * This junctions let all vehicles past through so they only should be used on
 * junctions where incoming vehicles are no foes to each other (may drive
 * simultaneously).
 */
class MSNoLogicJunction  : public MSJunction
{
public:
    /// Destructor.
    virtual ~MSNoLogicJunction();

    /** Container for incoming lanes. */
    typedef std::vector< MSLane* > LaneCont;

    /** Use this constructor only. */
    MSNoLogicJunction(std::string id, const Position2D &position,
                      LaneCont incoming
#ifdef HAVE_INTERNAL_LANES
                      , LaneCont internal
#endif
                     );

    /** Here, do nothing. */
    bool clearRequests();

    /** @brief does nothing
        Implementation of MSJunction */
    bool setAllowed() {
        return true;
    };

    /** Initialises the junction after the net was completely loaded */
    void postloadInit() throw(ProcessError);

private:
    /** Lanes incoming to the junction */
    LaneCont myIncomingLanes;

#ifdef HAVE_INTERNAL_LANES
    /** The junctions internal lanes */
    LaneCont myInternalLanes;
#endif

private:
    /// @brief Invalidated copy constructor.
    MSNoLogicJunction(const MSNoLogicJunction&);

    /// @brief Invalidated assignment operator.
    MSNoLogicJunction& operator=(const MSNoLogicJunction&);

    /** @brief a dump container
        Request-setting vehicles may write into this container and responds
        are read from it. As responds are always true, this container is
        set to true for all links.
        This dump is also used as the mask for incoming non-first vehicles */
    static std::bitset<64> myDump;

};


#endif

/****************************************************************************/

