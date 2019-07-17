/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Edge.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single edge is created or deleted
/****************************************************************************/
#ifndef GNEChange_Edge_h
#define GNEChange_Edge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <vector>
#include <map>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEEdge;
class GNELane;
class GNEAdditional;
class GNERerouter;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Edge
 * A network change in which a single edge is created or deleted
 */
class GNEChange_Edge : public GNEChange {
    // @brief FOX Declaration
    FXDECLARE_ABSTRACT(GNEChange_Edge)

public:
    /**@brief Constructor for creating/deleting an edge
     * @param[in] edge The edge to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Edge(GNEEdge* edge, bool forward);

    /// @brief Destructor
    ~GNEChange_Edge();

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
    /**@brief full information regarding the edge that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEEdge* myEdge;

    /// @brief reference to vector of shape parents
    const std::vector<GNEShape*>& myEdgeShapeParents;

    /// @brief reference to vector of additional parents
    const std::vector<GNEAdditional*>& myEdgeAdditionalParents;

    /// @brief reference to vector of demand element parents
    const std::vector<GNEDemandElement*>& myEdgeDemandElementParents;

    /// @brief reference to vector of shape children
    const std::vector<GNEShape*>& myEdgeShapeChildren;

    /// @brief reference to vector of additional children
    const std::vector<GNEAdditional*>& myEdgeAdditionalChildren;

    /// @brief vector of references to vector of demand element children (used by lanes)
    const std::vector<GNEDemandElement*>& myEdgeDemandElementChildren;

    /// @brief vector of references to vector of shape parents (used by lanes)
    std::vector<std::vector<GNEShape*> > myLaneShapeParents;

    /// @brief vector of references to vector of additional parents (used by lanes)
    std::vector<std::vector<GNEAdditional*> > myLaneAdditionalParents;

    /// @brief vector of references to vector of demand element parents (used by lanes)
    std::vector<std::vector<GNEDemandElement*> > myLaneDemandElementParents;

    /// @brief vector of references to vector of shape children (used by lanes)
    std::vector<std::vector<GNEShape*> > myLaneShapeChildren;

    /// @brief vector of references to vector of additional children (used by lanes)
    std::vector<std::vector<GNEAdditional*> > myLaneAdditionalChildren;

    /// @brief vector of references to vector of demand element children (used by lanes)
    std::vector<std::vector<GNEDemandElement*> > myLaneDemandElementChildren;
};

#endif
/****************************************************************************/
