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
/// @file    GNEFileDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog used for opening/saving files
/****************************************************************************/

#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEFileDialog.h"
#include "GNEFileSelector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileDialog::GNEFileDialog(GNEApplicationWindow* applicationWindow, const std::string elementFile,
                             const std::vector<std::string>& extensions, GNEFileDialog::OpenMode openMode,
                             GNEFileDialog::ConfigType configType):
    GNEDialog(applicationWindow, TLF("Save % as", elementFile), GUIIcon::SAVE,
              DialogType::FILE, GNEDialog::Buttons::ACCEPT_CANCEL, GNEDialog::OpenType::MODAL,
              GNEDialog::ResizeMode::RESIZABLE, 500, 300) {
    // update title and icon if we are opening
    if (openMode != GNEFileDialog::OpenMode::SAVE) {
        updateIcon(GUIIcon::OPEN);
        updateTitle(TLF("Open %", elementFile));
    }
    // create file selector
    myFileSelector = new GNEFileSelector(this, extensions, openMode, configType);
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
    // open dialog without focusing the button
    openDialog(myFileSelector->getFilenameTextField());
}


GNEFileDialog::~GNEFileDialog() {
    getApp()->reg().writeIntEntry("GNEFileDialog", "width", getWidth());
    getApp()->reg().writeIntEntry("GNEFileDialog", "height", getHeight());
    getApp()->reg().writeUnsignedEntry("GNEFileDialog", "style", myFileSelector->getFileBoxStyle());
    getApp()->reg().writeUnsignedEntry("GNEFileDialog", "showhidden", myFileSelector->showHiddenFiles());
}


void
GNEFileDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    if (dialogArgument->getCustomAction().size() > 0) {
        myFileSelector->setPath(dialogArgument->getCustomAction());
    }
}


std::string
GNEFileDialog::getFilename() const {
    return assureExtension(myFileSelector->getFilename());
}


std::vector<std::string>
GNEFileDialog::getFilenames() const {
    std::vector<std::string> filenames;
    // assure extension for each file
    for (auto& filename : myFileSelector->getFilenames()) {
        filenames.push_back(assureExtension(filename));
    }
    return filenames;
}


std::string
GNEFileDialog::getDirectory() const {
    return myFileSelector->getDirectory();
}


long
GNEFileDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // update current folder
    gCurrentFolder = myFileSelector->getDirectory().c_str();
    // close dialog accepting changes
    return closeDialogAccepting();
}


std::string
GNEFileDialog::assureExtension(const std::string& filename) const {
    // get group of extensions selected in comboBox
    const auto& extensions = myFileSelector->getFileExtension();
    // iterate all groups of extensions
    for (const auto& extension : extensions) {
        // iterate over all extension to check if is the same extension
        if (StringUtils::endsWith(filename, extension)) {
            return filename;
        }
    }
    // in this point, we have to give an extension (if exist)
    if (extensions.size() > 0) {
        return (filename + extensions.front());
    } else {
        return filename;
    }
}

/****************************************************************************/
