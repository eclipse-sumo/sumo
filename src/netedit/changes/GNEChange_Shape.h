/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Shape.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2017
/// @version $Id$
///
// A network change in which a single Shape is created or deleted
/****************************************************************************/
#ifndef GNEChange_Shape_h
#define GNEChange_Shape_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEShape;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_Shape
* A network change in which a single poly is created or deleted
*/
class GNEChange_Shape : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Shape)

public:
    /**@brief Constructor
    * @param[in] shape the shape to be changed
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_Shape(GNEShape* shape, bool forward);

    /// @brief Destructor
    ~GNEChange_Shape();

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
    /// @brief pointer to shape
    GNEShape* myShape;

    /// @brief reference to vector of edge parents
    const std::vector<GNEEdge*>& myEdgeParents;

    /// @brief reference to vector of lane parents
    const std::vector<GNELane*>& myLaneParents;

    /// @brief reference to vector of shape parents
    const std::vector<GNEShape*>& myShapeParents;

    /// @brief reference to vector of additional parents
    const std::vector<GNEAdditional*>& myAdditionalParents;

    /// @brief reference to vector of demand element parents
    const std::vector<GNEDemandElement*>& myDemandElementParents;

    /// @brief reference to vector of edge children
    const std::vector<GNEEdge*>& myEdgeChildren;

    /// @brief reference to vector of lane children
    const std::vector<GNELane*>& myLaneChildren;

    /// @brief reference to vector of shape children
    const std::vector<GNEShape*>& myShapeChildren;

    /// @brief reference to vector of additional children
    const std::vector<GNEAdditional*>& myAdditionalChildren;

    /// @brief reference to vector of demand element children
    const std::vector<GNEDemandElement*>& myDemandElementChildren;
};

#endif
/****************************************************************************/
