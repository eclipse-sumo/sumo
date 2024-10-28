/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_Distribution.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// A change in which the distribution attribute of some object is modified
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Distribution
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange_Distribution : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Distribution)

public:
    /// @brief add new key
    static void addKey(GNEDemandElement* distribution, const GNEDemandElement* key, const double value, GNEUndoList* undoList);

    /// @brief remove key
    static void removeKey(GNEDemandElement* distribution, const GNEDemandElement* key, GNEUndoList* undoList);

    /// @brief edit value
    static void editValue(GNEDemandElement* distribution, const GNEDemandElement* key, const double newValue, GNEUndoList* undoList);

    /// @brief Destructor
    ~GNEChange_Distribution();

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
    /**@brief the distribution to which all operations shall be applied
     * @note we are not responsible for the pointer
     */
    GNEDemandElement* myDistribution;

    /// @brief the key
    const GNEDemandElement* myKey;

    /// @brief the original value
    const double myOriginalProbability;

    /// @brief the new value
    const double myNewProbability;

    /// @brief flag for check if we're adding or removing key
    const bool myAddKey;

    /// @brief flag for check if we're editing value
    const bool myEditingProbability;

    /**@brief constructor for add/modify key
     * @param[in] distribution The distribution to be modified
     * @param[in] key to add/remove
     * @param[in] value key value
     * @param[in] addKey flag to check if we're adding or removing key
     */
    GNEChange_Distribution(GNEDemandElement* distribution, const GNEDemandElement* key, const double value, const bool addKey);

    /**@brief constructor for add/modify key
     * @param[in] distribution The distribution to be modified
     * @param[in] key to add/remove
     * @param[in] originalValue The original distribution value
     * @param[in] newValue The new distribution value
     */
    GNEChange_Distribution(GNEDemandElement* distribution, const GNEDemandElement* key, const double originalValue, const double newValue);
};
