/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <netbuild/NBEdge.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAdditional;
class GNEViewNet;
class GNEDetectorEntry;
class GNEDetectorExit;
class GNEDetectorE3;
class GNELane;
class GNEEdge;

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

    /// @brief pointer to lane (used by additionals with lane parent)
    GNELane* myLaneParent;

    /// @brief pointer to edge (used by additionals with edge parent)
    GNEEdge* myEdgeParent;

    /// @brief pointer to additional (used by additional with parents, for example Entry/exits)
    GNEAdditional* myAdditionalParent;

    /// @brief list of Edge childs (used by Rerouters)
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief list of Edge childs (used by VSS)
    std::vector<GNELane*> myLaneChilds;
};

#endif
/****************************************************************************/
