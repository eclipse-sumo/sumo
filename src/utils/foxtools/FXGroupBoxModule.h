/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2022 German Aerospace Center (DLR) and others.
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
    /// @brief GroupBoxModule options
    enum Options {
        NOTHING =       1 << 0, // Collapsible groupBox
        COLLAPSIBLE =   1 << 1, // Collapsible groupBox
        SAVE =          1 << 2, // Save contents
        LOAD =          1 << 3, // Load contents
    };

    /// @brief constructor
    FXGroupBoxModule(FXVerticalFrame* contentFrame, const std::string &text, const int options = Options::COLLAPSIBLE);

    /// @brief destructor
    ~FXGroupBoxModule();

    /// @brief set text
    void setText(const std::string& text);

    /// @brief get collapsable frame (used by all elements that will be collapsed if button is toogled)
    FXVerticalFrame* getCollapsableFrame();

    /// @brief draw FXGroupBoxModule
    long onPaint(FXObject*,FXSelector,void*);

    /// @brief collapse GroupBoxModule
    long onCmdCollapseButton(FXObject*,FXSelector,void*);

    /// @brief save contents
    long onCmdSaveButton(FXObject*,FXSelector,void*);

    /// @brief load contents
    long onCmdLoadButton(FXObject*,FXSelector,void*);

protected:
    /// @brief FOX need this
    FXGroupBoxModule();

    /// @brief save contents (can be reimplemented in children)
    virtual bool saveContents() const;

    /// @brief load contents (can be reimplemented in children)
    virtual bool loadContents() const;

    /// @brief enable or disable save buttons
    void toogleSaveButton(const bool value);

private:
    /// @brief GroupBoxModule options
    const int myOptions;

    /// @brief vertical collapsable frame
    FXVerticalFrame *myCollapsableFrame = nullptr;

    /// @brief label used in non collapsable FXGroupBoxModule
    FXLabel *myLabel = nullptr;

    /// @brief button for collapse elements
    FXButton *myCollapseButton = nullptr;

    /// @brief button for save elements
    FXButton *mySaveButton = nullptr;

    /// @brief button for load elements
    FXButton *myLoadButton = nullptr;

    /// @brief flag to check if this groupbox is collapsed
    bool myCollapsed;
};
