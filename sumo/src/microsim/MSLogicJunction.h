/****************************************************************************/
/// @file    MSLogicJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// with one ore more logics.
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
#ifndef MSLogicJunction_h
#define MSLogicJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSJunction.h"
#include <bitset>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLogicJunction
 * A junction which may not let all vehicles through, but must perform any
 * kind of an operation to determine which cars are allowed to drive in this
 * step.
 */
class MSLogicJunction : public MSJunction {
public:
    /** Container for incoming lanes. */
    typedef std::vector< MSLane* > LaneCont;

    /// Destructor.
    virtual ~MSLogicJunction();

    /** @brief Container for vehicle requests.
        Each element of this container represents one particular
        link from one lane to another. */
    typedef std::bitset<64> Request;

    /** @brief Container for the request responds.
        The respond is link-bound */
    typedef std::bitset<64> Respond;

    /** @brief Container for link foes */
    typedef std::bitset<64> LinkFoes;

    /** @brief Container for junction-internal lane occupation
        Each element of this container represents one particular
        junction-internal lane */
    typedef std::bitset<64> InnerState;

    /// initialises the junction after the whole net has been loaded
    virtual void postloadInit() throw(ProcessError);

protected:
    /// constructor; this class is virtual
    MSLogicJunction(std::string id, const Position2D &position,
                    LaneCont incoming
#ifdef HAVE_INTERNAL_LANES
                    , LaneCont internal
#endif
                   );

protected:
    /// list of incoming lanes
    LaneCont myIncomingLanes;

#ifdef HAVE_INTERNAL_LANES
    /// list of incoming lanes
    LaneCont myInternalLanes;
#endif
    /** Current request. */
    Request myRequest;

    /** Current inner state */
    InnerState  myInnerState;

    /** Current respond */
    Respond myRespond;

private:
    /// @brief Invalidated copy constructor.
    MSLogicJunction(const MSLogicJunction&);

    /// @brief Invalidated assignment operator.
    MSLogicJunction& operator=(const MSLogicJunction&);

};


#endif

/****************************************************************************/

