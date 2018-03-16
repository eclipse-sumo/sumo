/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_VariableSpeedSignItem.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
/// @version $Id$
///
// A change in the values of Rerouters in netedit
/****************************************************************************/
#ifndef GNEChange_VariableSpeedSignItem_h
#define GNEChange_VariableSpeedSignItem_h


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
#include <utils/gui/globjects/GUIGlObject.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVariableSpeedSignStep;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_VariableSpeedSignItem
* A change to the network selection
*/
class GNEChange_VariableSpeedSignItem : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_VariableSpeedSignItem)

public:
    /**@brief Constructor
    * @param[in] variableSpeedSignStep Variable Speed Sign interval to be added
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_VariableSpeedSignItem(GNEVariableSpeedSignStep* variableSpeedSignStep, bool forward);

    /// @brief Destructor
    ~GNEChange_VariableSpeedSignItem();

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
    /// @brief modified Variable Speed Sign interval
    GNEVariableSpeedSignStep* myVariableSpeedSignStep;
};

#endif
/****************************************************************************/
