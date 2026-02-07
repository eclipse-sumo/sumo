/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_TAZSourceSink.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2025
///
// A network change in which a TAZ SourceSink element is created or deleted
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEChange.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_TAZSourceSink
 * A network change in which a sourceSink element is created or deleted
 */
class GNEChange_TAZSourceSink : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_TAZSourceSink)

public:
    /**@brief Constructor for creating/deleting an sourceSink element
     * @param[in] sourceSink The sourceSink element to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_TAZSourceSink(GNETAZSourceSink* sourceSink, bool forward);

    /// @brief Destructor
    ~GNEChange_TAZSourceSink();

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
    /**@brief full information regarding the sourceSink element that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNETAZSourceSink* mySourceSink;
};
