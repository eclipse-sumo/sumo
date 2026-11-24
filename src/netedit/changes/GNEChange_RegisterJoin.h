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
/// @file    GNEChange_RegisterJoin.h
/// @author  Jakob Erdmann
/// @date    June 2024
///
// Record joined junctions in NBNodeCont
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEChange.h"


class NBNodeCont;
class NBNode;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_RegisterJoin
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange_RegisterJoin : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_RegisterJoin)

public:
    /// @brief record cluster membership and the resulting junction ID in the undo list
    static void registerJoin(const std::set<NBNode*, ComparatorIdLess>& cluster, const std::string& id,
                             NBNodeCont& nc, GNEUndoList* undoList);

    /// @brief Destructor
    ~GNEChange_RegisterJoin();

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
    std::set<std::string> myNodeIDs;
    const std::string myJoinedID;
    NBNodeCont& myNC;

    /// @brief constructor
    GNEChange_RegisterJoin(const std::set<NBNode*, ComparatorIdLess>& cluster, const std::string& id, NBNodeCont& nc);
};
