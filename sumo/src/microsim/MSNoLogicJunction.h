/****************************************************************************/
/// @file    MSNoLogicJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// logic, e.g. for exits.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
class MSNoLogicJunction  : public MSJunction {
public:
    /// Destructor.
    virtual ~MSNoLogicJunction();

    /** Container for incoming lanes. */
    typedef std::vector< MSLane* > LaneCont;

    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     */
    MSNoLogicJunction(const std::string &id, const Position2D &position,
        const Position2DVector &shape, 
        LaneCont incoming
#ifdef HAVE_INTERNAL_LANES
        , LaneCont internal
#endif
        ) throw();

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

