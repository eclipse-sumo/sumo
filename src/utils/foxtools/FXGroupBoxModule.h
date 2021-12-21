/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXGroupBoxModule.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"

/// @brief FXGroupBoxModule (based on FXGroupBox)
class FXGroupBoxModule : protected FXVerticalFrame {
    FXDECLARE(FXGroupBoxModule)

public:
    /// @brief constructor
    FXGroupBoxModule(FXVerticalFrame* contentFrame, const std::string &text, const bool collapsible = true);

    /// @brief destructor
    ~FXGroupBoxModule();

    /// @brief set text
    void setText(const std::string& text);

    /// @brief get collapsable frame (used by all elements that will be collapsed if button is toogled)
    FXVerticalFrame* getCollapsableFrame();

    /// @brief draw FXGroupBoxModule
    long onPaint(FXObject*,FXSelector,void*);

    /// @brief draw FXGroupBoxModule
    long onCollapseButton(FXObject*,FXSelector,void*);

protected:
    /// @brief FOX need this
    FXGroupBoxModule();

private:
    /// @brief vertical collapsable frame
    FXVerticalFrame *myCollapsableFrame = nullptr;

    /// @brief label used in non collapsable FXGroupBoxModule
    FXLabel *myLabel = nullptr;

    /// @brief button for collapse elements
    FXButton *myCollapseButton = nullptr;

    /// @brief flag to check if this groupbox is collapsed
    bool myCollapsed;
};
