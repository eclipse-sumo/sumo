/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Children.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2019
/// @version $Id$
///
// A network change used to modify sorting of hierarchical element childrens
/****************************************************************************/
#ifndef GNEChange_Children_h
#define GNEChange_Children_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Children
 * A network change in which a additional element is created or deleted
 */
class GNEChange_Children : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Children)

public:
    // @brief operation over demand element child
    enum Operation {
        MOVE_FRONT = 0, // Move element one position front
        MOVE_BACK = 1   // Move element one position back
    };

    /**@brief Constructor for creating/deleting an additional element
     * @param[in] additional The additional element to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Children(GNEDemandElement* demandElementParent, GNEDemandElement* demandElementChild, const Operation operation);

    /// @brief Destructor
    ~GNEChange_Children();

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
    /**@brief full information regarding the demand element parent element that will be modified
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEDemandElement* myDemandElementParent;

    /// @brief demand element which position will be edited edited
    GNEDemandElement* myDemandElementChild;

    // @brief Operation to be apply
    const Operation myOperation;

    /// @brief copy of demand element childrens before apply operation
    const std::vector<GNEDemandElement*> myOriginalElementChildren;

    /// @brief element childrens after apply operation
    std::vector<GNEDemandElement*> myEditedElementChildren;
};

#endif
/****************************************************************************/
