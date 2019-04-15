/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Additional.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// A network change in which a additional element is created or deleted
/****************************************************************************/
#ifndef GNEChange_Additional_h
#define GNEChange_Additional_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNELane;
class GNEShape;
class GNEAdditional;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Additional
 * A network change in which a additional element is created or deleted
 */
class GNEChange_Additional : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Additional)

public:
    /**@brief Constructor for creating/deleting an additional element
     * @param[in] additional The additional element to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Additional(GNEAdditional* additional, bool forward);

    /// @brief Destructor
    ~GNEChange_Additional();

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
    /**@brief full information regarding the additional element that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEAdditional* myAdditional;

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

    /// @brief reference to vector of edge childs
    const std::vector<GNEEdge*>& myEdgeChilds;

    /// @brief reference to vector of lane childs
    const std::vector<GNELane*>& myLaneChilds;

    /// @brief reference to vector of shape childs
    const std::vector<GNEShape*>& myShapeChilds;

    /// @brief reference to vector of additional childs
    const std::vector<GNEAdditional*>& myAdditionalChilds;

    /// @brief reference to vector of demand element childs
    const std::vector<GNEDemandElement*>& myDemandElementChilds;
};

#endif
/****************************************************************************/
