/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_EnableAttribute.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
/// @version $Id$
///
// A network change in which the attribute of some object is modified
/****************************************************************************/
#ifndef GNEChange_EnableAttribute_h
#define GNEChange_EnableAttribute_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNENetElement;
class GNEAdditional;
class GNEDemandElement;
class GNEShape;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_EnableAttribute
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange_EnableAttribute : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_EnableAttribute)

public:
    /**@brief Constructor
     * @param[in] ac The attribute-carrier to be modified
     * @param[in] net Net in which AC is saved
     * @param[in] originalAttributes original set of attributes
     * @param[in] newAttributes new set of attributes
     */
    GNEChange_EnableAttribute(GNEAttributeCarrier* ac, GNENet* net, const int originalAttributes, const int newAttributes);

    /// @brief Destructor
    ~GNEChange_EnableAttribute();

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
    /**@brief the net to which all operations shall be applied
     * @note we are not responsible for the pointer
     */
    GNEAttributeCarrier* myAC;

    /// @brief original attributes
    const int myOriginalAttributes;

    /// @brief original attributes
    const int myNewAttributes;
};

#endif
/****************************************************************************/
