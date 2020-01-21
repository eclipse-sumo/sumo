/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_GenericData.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A network change in which a generic data element is created or deleted
/****************************************************************************/
#ifndef GNEChange_GenericData_h
#define GNEChange_GenericData_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNELane;
class GNEShape;
class GNEAdditional;
class GNEGenericData;
class GNEXMLChild;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_GenericData
 * A network change in which a generic data element is created or deleted
 */
class GNEChange_GenericData : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_GenericData)

public:
    /**@brief Constructor for creating/deleting an generic data element
     * @param[in] generic data The generic data element to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_GenericData(GNEGenericData* genericData, bool forward);

    /// @brief Destructor
    ~GNEChange_GenericData();

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
    /**@brief full information regarding the generic data element that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEGenericData* myGenericData;
};

#endif
/****************************************************************************/
