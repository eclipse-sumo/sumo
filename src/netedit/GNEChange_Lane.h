/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Lane.h
/// @author  Jakob Erdmann
/// @date    April 2011
/// @version $Id$
///
// A network change in which a single lane is created or deleted
/****************************************************************************/
#ifndef GNEChange_Lane_h
#define GNEChange_Lane_h


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
#include <netbuild/NBEdge.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEEdge;
class GNELane;
class GNEAdditional;
class GNEShape;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Lane
 * A network change in which a single lane is created or deleted
 */
class GNEChange_Lane : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Lane)

public:
    /**@brief Constructor for creating/deleting an edge
     * @param[in] edge The edge on which to apply changes
     * @param[in] lane The lane to be deleted or 0 if a lane should be created
     * @param[in] laneAttrs The attributes of the lane to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     * @param[in] recomputeConnections Whether to recompute all connections for the affected edge
     */
    GNEChange_Lane(GNEEdge* edge, GNELane* lane, const NBEdge::Lane& laneAttrs, bool forward, bool recomputeConnections=true);

    /// @brief Destructor
    ~GNEChange_Lane();

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
    /// @brief we need the edge because it is the target of our change commands
    GNEEdge* myEdge;

    /// @brief we need to preserve the lane because it maybe the target of GNEChange_Attribute commands
    GNELane* myLane;

    /// @brief we need to preserve the attributes explicitly because they are not contained withing GNELane itself
    const NBEdge::Lane myLaneAttrs;

    /// @brief we need to preserve the list of additionals vinculated with this lane
    std::vector<GNEAdditional*> myAdditionalChilds;

    /// @brief we need to preserve the list of shapes vinculated with this lane
    std::vector<GNEShape*> myShapeChilds;

    /// @brief we need to preserve the list of additional sets in which this lane is a child
    std::vector<GNEAdditional*> myAdditionalParents;

    /// @bried whether to recompute connection when adding a new lane
    bool myRecomputeConnections;
};

#endif
/****************************************************************************/
