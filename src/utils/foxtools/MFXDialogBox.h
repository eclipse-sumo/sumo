/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    MFXDialogBox.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2025
///
// Custom FXDialogBox that supports internal tests
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"


// ===========================================================================
// class definitions
// ===========================================================================

class MFXDialogBox : public FXTopWindow {
    FXDECLARE(MFXDialogBox)

protected:
    MFXDialogBox() {}

private:
    MFXDialogBox(const MFXDialogBox&);
    MFXDialogBox& operator=(const MFXDialogBox&);

public:
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onCmdAccept(FXObject*, FXSelector, void*);
    long onCmdCancel(FXObject*, FXSelector, void*);

public:

    /// Construct free-floating dialog
    MFXDialogBox(FXApp* a, const FXString& name, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = 10, FXint pr = 10, FXint pt = 10, FXint pb = 10, FXint hs = 4, FXint vs = 4);

    /// Construct dialog which will always float over the owner window
    MFXDialogBox(FXWindow* owner, const FXString& name, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = 10, FXint pr = 10, FXint pt = 10, FXint pb = 10, FXint hs = 4, FXint vs = 4);

    /// Run modal invocation of the dialog
    virtual FXuint execute(FXuint placement = PLACEMENT_CURSOR);
};
