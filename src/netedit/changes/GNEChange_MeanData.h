/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    GNEChange_MeanData.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// A network change in which a mean data set is created or deleted
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_MeanData
 * A network change in which a mean data set is created or deleted
 */
class GNEChange_MeanData : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_MeanData)

public:
    /**@brief Constructor for creating/deleting an mean data set
     * @param[in] mean data The mean data set to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_MeanData(GNEMeanData* meanData, bool forward);

    /// @brief Destructor
    ~GNEChange_MeanData();

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
    /**@brief full information regarding the mean data set that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEMeanData* myMeanData;
};

/****************************************************************************/
