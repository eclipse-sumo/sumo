/****************************************************************************/
/// @file    RODFEdge.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// An edge within the DFROUTER
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
#include <utils/geom/Position2D.h>
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
     * @param[in] index The numeric id of the edge
     */
    RODFEdge(const std::string &id, unsigned int index) throw();


    /// @brief Destructor
    ~RODFEdge() throw();

    void setFlows(const std::vector<FlowDef> &flows);

    const std::vector<FlowDef> &getFlows() const;


private:
    std::vector<FlowDef> myFlows;

private:
    /// @brief Invalidated copy constructor
    RODFEdge(const RODFEdge &src);

    /// @brief Invalidated assignment operator
    RODFEdge &operator=(const RODFEdge &src);

};


#endif

/****************************************************************************/

