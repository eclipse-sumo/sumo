/****************************************************************************/
/// @file    RODFEdge.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// An edge within the DFROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2006-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODFEdge_h
#define RODFEdge_h


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
#include <router/ROEdge.h>
#include <utils/geom/Position.h>
#include "RODFDetectorFlow.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFEdge
 */
class RODFEdge : public ROEdge {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] index The numeric id of the edge
     */
    RODFEdge(const std::string& id, RONode* from, RONode* to, unsigned int index, const int priority);


    /// @brief Destructor
    ~RODFEdge();

    void setFlows(const std::vector<FlowDef>& flows);

    const std::vector<FlowDef>& getFlows() const;


private:
    std::vector<FlowDef> myFlows;

private:
    /// @brief Invalidated copy constructor
    RODFEdge(const RODFEdge& src);

    /// @brief Invalidated assignment operator
    RODFEdge& operator=(const RODFEdge& src);

};


#endif

/****************************************************************************/

