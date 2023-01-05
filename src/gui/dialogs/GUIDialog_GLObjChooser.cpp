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
/// @file    GUIDialog_GLObjChooser.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/
#include <config.h>

#include <string>
#include <vector>
#include <fxkeys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <gui/GUISUMOViewParent.h>
#include "GUIDialog_GLObjChooser.h"


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_GLObjChooser::GUIDialog_GLObjChooser(GUISUMOViewParent* SUMOViewParent, int messageId,
        FXIcon* icon, const FXString& title, const std::vector<GUIGlID>& ids, GUIGlObjectStorage& glStorage) :
    GUIDialog_ChooserAbstract(SUMOViewParent, messageId, icon, title, ids, glStorage),
    mySUMOViewParent(SUMOViewParent) {
}


GUIDialog_GLObjChooser::~GUIDialog_GLObjChooser() {
    mySUMOViewParent->eraseGLObjChooser(this);
}

/****************************************************************************/
