/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXGroupBoxModule.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"

#include <string>

/// @brief class declaration
class GNEFrame;

/// @brief MFXGroupBoxModule (based on FXGroupBox)
class MFXGroupBoxModule : protected FXVerticalFrame {
    FXDECLARE(MFXGroupBoxModule)

public:
    /// @brief GroupBoxModule options
    enum Options {
        NOTHING =       1 << 0, // Basic GroupBox
        COLLAPSIBLE =   1 << 1, // Collapsible groupBox
        EXTENSIBLE =    1 << 2, // Extensible groupBox
        SAVE =          1 << 3, // Save contents
        LOAD =          1 << 4, // Load contents
    };

    /// @brief constructor for frames
    MFXGroupBoxModule(GNEFrame* frame, const std::string& text, const int options = Options::COLLAPSIBLE);

    /// @brief constructor for fix dialogs
    MFXGroupBoxModule(FXVerticalFrame* contentFrame, const std::string& text, const int options = Options::NOTHING);

    /// @brief destructor
    ~MFXGroupBoxModule();

    /// @brief set text
    void setText(const std::string& text);

    /// @brief get collapsable frame (used by all elements that will be collapsed if button is toggled)
    FXVerticalFrame* getCollapsableFrame();

    /// @brief draw MFXGroupBoxModule
    long onPaint(FXObject*, FXSelector, void*);

    /// @brief collapse GroupBoxModule
    long onCmdCollapseButton(FXObject*, FXSelector, void*);

    /// @brief extends GroupBoxModule
    long onCmdExtendButton(FXObject*, FXSelector, void*);

    /// @brief reset GroupBoxModule
    long onCmdResetButton(FXObject*, FXSelector, void*);

    /// @brief update reset GroupBoxModule
    long onUpdResetButton(FXObject*, FXSelector, void*);

    /// @brief save contents
    long onCmdSaveButton(FXObject*, FXSelector, void*);

    /// @brief load contents
    long onCmdLoadButton(FXObject*, FXSelector, void*);

protected:
    /// @brief FOX need this
    MFXGroupBoxModule();

    /// @brief save contents (can be reimplemented in children)
    virtual bool saveContents() const;

    /// @brief load contents (can be reimplemented in children)
    virtual bool loadContents() const;

    /// @brief enable or disable save buttons
    void toggleSaveButton(const bool value);

private:
    /// @brief GroupBoxModule options
    const int myOptions;

    /// @brief GNEFrame in which this GroupBox is placed
    GNEFrame* myFrameParent = nullptr;

    /// @brief vertical collapsable frame
    FXVerticalFrame* myCollapsableFrame = nullptr;

    /// @brief label used in non collapsable MFXGroupBoxModule
    FXLabel* myLabel = nullptr;

    /// @brief button for collapse elements
    FXButton* myCollapseButton = nullptr;

    /// @brief button for extend elements
    FXButton* myExtendButton = nullptr;

    /// @brief button for reset frame width
    FXButton* myResetWidthButton = nullptr;

    /// @brief button for save elements
    FXButton* mySaveButton = nullptr;

    /// @brief button for load elements
    FXButton* myLoadButton = nullptr;

    /// @brief flag to check if this groupbox is collapsed
    bool myCollapsed;
};
