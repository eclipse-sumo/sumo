/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_DemandElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// A network change in which a demand element element is created or deleted
/****************************************************************************/
#ifndef GNEChange_DemandElement_h
#define GNEChange_DemandElement_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDemandElement;
class GNEViewNet;
class GNEEdge;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_DemandElement
 * A network change in which a demand element element is created or deleted
 */
class GNEChange_DemandElement : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_DemandElement)

public:
    /**@brief Constructor for creating/deleting an demand element element
     * @param[in] demand element The demand element element to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_DemandElement(GNEDemandElement* demandElement, bool forward);

    /// @brief Destructor
    ~GNEChange_DemandElement();

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
    /**@brief full information regarding the demand element element that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEDemandElement* myDemandElement;

    /// @brief pointer to edge parents (used by demandElements with edge parents)
    std::vector<GNEEdge*> myEdgeParents;

    /// @brief pointer to demand element parents (used by demand elements with parents)
    std::vector<GNEDemandElement*> myDemandElementParents;
};

#endif
/****************************************************************************/
