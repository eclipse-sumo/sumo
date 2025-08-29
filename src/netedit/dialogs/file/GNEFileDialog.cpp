/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFileDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog used for opening/saving files
/****************************************************************************/

#include <utils/gui/div/GUIIOGlobals.h>

#include "GNEFileDialog.h"
#include "GNEFileSelector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileDialog::GNEFileDialog(GNEApplicationWindow* applicationWindow, const std::string title, GUIIcon icon,
                             const std::vector<std::string>& extensions, GNEFileDialog::OpenMode openMode):
    GNEDialog(applicationWindow, title, icon, GNEDialog::Buttons::ACCEPT_CANCEL,
              GNEDialog::OpenType::MODAL, GNEDialog::ResizeMode::RESIZABLE, 500, 300) {
    // create file selector
    myFileSelector = new GNEFileSelector(this, extensions, openMode);
    // retarget accept button to file selector
    myAcceptButton->setTarget(myFileSelector);
    myAcceptButton->setSelector(FXFileSelector::ID_ACCEPT);
    // check if we have saved settings in registry
    setWidth(getApp()->reg().readIntEntry("GNEFileDialog", "width", getWidth()));
    setHeight(getApp()->reg().readIntEntry("GNEFileDialog", "height", getHeight()));
    myFileSelector->setFileBoxStyle(getApp()->reg().readUnsignedEntry("GNEFileDialog", "style", myFileSelector->getFileBoxStyle()));
    myFileSelector->showHiddenFiles((getApp()->reg().readUnsignedEntry("GNEFileDialog", "showhidden", myFileSelector->showHiddenFiles()) == 1) ? TRUE : FALSE);
    // set initial directory
    if (gCurrentFolder.length() > 0) {
        myFileSelector->setDirectory(gCurrentFolder);
    }
    // open dialog
    openDialog();
}


GNEFileDialog::~GNEFileDialog() {
    getApp()->reg().writeIntEntry("GNEFileDialog", "width", getWidth());
    getApp()->reg().writeIntEntry("GNEFileDialog", "height", getHeight());
    getApp()->reg().writeUnsignedEntry("GNEFileDialog", "style", myFileSelector->getFileBoxStyle());
    getApp()->reg().writeUnsignedEntry("GNEFileDialog", "showhidden", myFileSelector->showHiddenFiles());
}


void
GNEFileDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // not yet finish
}


void
GNEFileDialog::setFilename(const FXString& path) {
    myFileSelector->setFilename(path);
}


std::string
GNEFileDialog::getFilename() const {
    return myFileSelector->getFilename();
}


std::vector<std::string>
GNEFileDialog::getFilenames() const {
    return myFileSelector->getFilenames();
}


std::string
GNEFileDialog::getDirectory() const {
    return myFileSelector->getDirectory();
}


long
GNEFileDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // udpate current folder
    gCurrentFolder = myFileSelector->getDirectory().c_str();
    // close dialog accepting changes
    return closeDialogAccepting();
}

/****************************************************************************/
