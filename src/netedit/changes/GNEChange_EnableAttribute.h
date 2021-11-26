/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEChange_EnableAttribute.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// A network change in which the attribute of some object is modified
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEChange.h"

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
    /**@brief constructor
     * @param[in] ac The attribute-carrier to be modified
     * @param[in] key The attribute key
     * @param[in] value The new value
     */
    GNEChange_EnableAttribute(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value);

    /**@brief constructor
     * @param[in] ac The attribute-carrier to be modified
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] previousParameters previous values (used by flows)
     */
    GNEChange_EnableAttribute(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value, const int previousParameters);

    /// @brief Destructor
    ~GNEChange_EnableAttribute();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    std::string undoName() const;

    /// @brief get Redo name
    std::string redoName() const;

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

    /// @brief The attribute name
    const SumoXMLAttr myKey;

    /// @brief the original value
    const bool myOrigValue;

    /// @brief the new value
    const bool myNewValue;

    /// @brief previous values (used by flows)
    const int myPreviousParameters;
};
