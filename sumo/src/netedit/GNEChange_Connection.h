/****************************************************************************/
/// @file    GNEChange_Connection.h
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// A network change in which a single connection is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChange_Connection_h
#define GNEChange_Connection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <string>
#include <utils/foxtools/fxexdefs.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Connection
 * A network change in which a single lane is created or deleted
 */
class GNEChange_Connection : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Connection)

public:
    /**@brief Constructor for creating/deleting an edge
     * @param[in] edge The edge on which to apply changes
     * @param[in] lane The lane to be deleted or 0 if a lane should be created
     * @param[in] laneAttrs The attributes of the lane to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Connection(GNEEdge* edge, unsigned int fromLane,
                         const std::string& toEdgeID, unsigned int toLane,
                         bool mayDefinitelyPass, bool forward);

    /// @brief Destructor
    ~GNEChange_Connection();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}


private:
    // @name full information regarding the lane that is to be created/deleted
    // @briefwe assume shared responsibility for the pointers (via reference counting)
    /// @{
    // @brief we need the edge because it is the target of our change commands
    GNEEdge* myEdge;

    /// @brief the lane from which the connection originates
    unsigned int myFromLane;

    /// @brief the id of the target edge
    const std::string myToEdgeID;

    /// @brief the target lane of the connection
    unsigned int myToLane;
    /// @}

    /// @brief whether this connection never yields
    bool myPass;
};

#endif
/****************************************************************************/
