/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
    static void addKey(GNEDemandElement* distribution, const std::string &key, const double value, GNEUndoList* undoList);

    /// @brief remove key
    static void removeKey(GNEDemandElement* distribution, const std::string &key, GNEUndoList* undoList);

    /// @brief edit value
    static void editValue(GNEDemandElement* distribution, const std::string &key, const double newValue, GNEUndoList* undoList);

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

    /// @brief the original key
    const std::string myOrigKey;

    /// @brief the new key (empty means remove)
    const std::string myNewKey;

    /// @brief the original value
    const double myOrigValue;

    /// @brief the new value
    const double myNewValue;

    /**@brief constructor for add/modify key
     * @param[in] distribution The distribution to be modified
     * @param[in] originalKey The original distribution key
     * @param[in] newKey The new distribution key
     * @param[in] originalValue The original distribution value
     * @param[in] newValue The new distribution value
     */
    GNEChange_Distribution(GNEDemandElement* distribution, const std::string &originalKey, const std::string &newKey,
                           const double originalValue, const double newValue);
};
