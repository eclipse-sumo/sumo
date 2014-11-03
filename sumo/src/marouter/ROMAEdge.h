/****************************************************************************/
/// @file    ROMAEdge.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#ifndef ROMAEdge_h
#define ROMAEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <utils/common/ValueTimeLine.h>
#include <utils/common/SUMOVehicleClass.h>
#include <router/ROEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROLane;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROMAEdge
 * @brief A basic edge for routing applications
 *
 * The edge contains two time lines, one for the travel time and one for a second
 *  measure which may be used for computing the costs of a route. After loading
 *  the weights, it is needed to call "buildTimeLines" in order to initialise
 *  these time lines.
 */
class ROMAEdge : public ROEdge {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] index The numeric id of the edge
     */
    ROMAEdge(const std::string& id, RONode* from, RONode* to, unsigned int index, const int priority);


    /// Destructor
    virtual ~ROMAEdge();

    /** @brief Adds information about a connected edge.
     *
     * In addition to ROEdge::addSuccessor it keeps track of left turns.
     *
     * @param[in] s The edge to add
     * @todo What about vehicle-type aware connections?
     */
    virtual void addSuccessor(ROEdge* s, std::string dir = "");

    void setHelpFlow(const SUMOReal flow) {
        myHelpFlow = flow;
    }

    SUMOReal getHelpFlow() const {
        return myHelpFlow;
    }

private:
    std::set<ROMAEdge*> myLeftTurns;
    SUMOReal myHelpFlow;

private:
    /// @brief Invalidated copy constructor
    ROMAEdge(const ROMAEdge& src);

    /// @brief Invalidated assignment operator
    ROMAEdge& operator=(const ROMAEdge& src);

};


#endif

/****************************************************************************/
