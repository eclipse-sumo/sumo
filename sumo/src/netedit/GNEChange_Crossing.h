/****************************************************************************/
/// @file    GNEChange_Crossing.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// A network change in which a single crossing is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChange_Crossing_h
#define GNEChange_Crossing_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <vector>
#include <map>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;
class NBEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Crossing
 * A network change in which a single crossing is created or deleted
 */
class GNEChange_Crossing : public GNEChange {
    // @brief FOX Declaration
    FXDECLARE_ABSTRACT(GNEChange_Crossing)

public:
    /**@brief Constructor for creating/deleting an crossing
     * @param[in] JunctionParent GNEJunction in which the crossing will be created/deleted
     * @param[in] edges vector of edges of crossing
     * @param[in] width value with the width of crossing
     * @param[in] priority boolean with the priority of crossing
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Crossing(GNEJunction* junctionParent, const std::vector<NBEdge*>& edges, SUMOReal width, bool priority, bool forward);

    /// @brief Destructor
    ~GNEChange_Crossing();

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
    /// @brief full information regarding the Junction in which GNECRossing is created
    GNEJunction* myJunctionParent;

    /// @brief vector to save all edges of GNECrossing
    std::vector<NBEdge*> myEdges;

    /// @brief width of GNECrossing
    SUMOReal myWidth;

    /// @brief priority of GNECrossing
    bool myPriority;
};

#endif
/****************************************************************************/
