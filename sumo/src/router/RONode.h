/****************************************************************************/
/// @file    RONode.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Base class for nodes used by the router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#ifndef RONode_h
#define RONode_h


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
#include <utils/common/Named.h>
#include <utils/geom/Position.h>

// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONode
 * @brief Base class for nodes used by the router
 */
class RONode : public Named {
public:
    /** @brief Constructor
     * @param[in] id The id of the node
     */
    RONode(const std::string& id);


    /// @brief Destructor
    ~RONode();


    /** @brief Sets the position of the node
     * @param[in] p The node's position
     */
    void setPosition(const Position& p);


    /** @brief Returns the position of the node
     * @return This node's position
     */
    const Position& getPosition() {
        return myPosition;
    }


    inline const std::vector<const ROEdge*>& getIncoming() const {
        return myIncoming;
    }

    inline const std::vector<const ROEdge*>& getOutgoing() const {
        return myOutgoing;
    }

    void addIncoming(ROEdge* edge) {
        myIncoming.push_back(edge);
    }

    void addOutgoing(ROEdge* edge) {
        myOutgoing.push_back(edge);
    }

private:
    /// @brief This node's position
    Position myPosition;

    /// @brief incoming edges
    std::vector<const ROEdge*> myIncoming;
    /// @brief outgoing edges
    std::vector<const ROEdge*> myOutgoing;


private:
    /// @brief Invalidated copy constructor
    RONode(const RONode& src);

    /// @brief Invalidated assignment operator
    RONode& operator=(const RONode& src);

};


#endif

/****************************************************************************/

