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

#include "GNEFileDialog.h"
#include "GNEFileSelector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileDialog::GNEFileDialog(GNEApplicationWindow* applicationWindow, const std::string title, GUIIcon icon,
                             const std::vector<std::string>& extensions, const bool save, const bool multiElements):
    GNEDialog(applicationWindow, title, icon, GNEDialog::Buttons::ACCEPT_CANCEL,
              GNEDialog::OpenType::MODAL, GNEDialog::ResizeMode::RESIZABLE, 500, 300) {
    // create file selector
    myFileSelector = new GNEFileSelector(this, extensions, save, multiElements);
    /*
        // open dialog
        if (opendialog.execute()) {
            // continue depending if we're loading or saving
            if (save) {
                // check if overwritte file
                if (MFXUtils::userPermitsOverwritingWhenFileExists(window, opendialog.getFilename())) {
                    // udpate current folder
                    gCurrentFolder = opendialog.getDirectory();
                    // assureExtension
                    return MFXUtils::assureExtension(opendialog).text();
                } else {
                    // return empty file
                    return "";
                }
            } else {
                // udpate current folder
                gCurrentFolder = opendialog.getDirectory();
                // return file
                if (multi) {
                    FXString* files = opendialog.getFilenames();
                    std::string result;
                    bool first = true;
                    if (files != nullptr) {
                        for (int i = 0; !files[i].empty(); i++) {
                            if (first) {
                                first = false;
                            } else {
                                result += " ";
                            }
                            result += files[i].text();
                        }
                        delete [] files;

                    }
                    return result;
                } else {
                    return opendialog.getFilename().text();
                }
            }
        } else {
            // return empty file
            return "";
        }
    */
    // retarget accept button to file selector
    myAcceptButton->setTarget(myFileSelector);
    myAcceptButton->setSelector(FXFileSelector::ID_ACCEPT);
    // check if we have saved settings in registry
    setWidth(getApp()->reg().readIntEntry("File Dialog", "width", getWidth()));
    setHeight(getApp()->reg().readIntEntry("File Dialog", "height", getHeight()));
    setFileBoxStyle(getApp()->reg().readUnsignedEntry("File Dialog", "style", getFileBoxStyle()));
    showHiddenFiles(getApp()->reg().readUnsignedEntry("File Dialog", "showhidden", showHiddenFiles()));
    // open dialog
    openDialog();
}


GNEFileDialog::~GNEFileDialog() {
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


void
GNEFileDialog::setPattern(const FXString& ptrn) {
    myFileSelector->setPattern(ptrn);
}


FXString
GNEFileDialog::getPattern() const {
    return myFileSelector->getPattern();
}


void
GNEFileDialog::setDirectory(const FXString& path) {
    myFileSelector->setDirectory(path);
}


FXString
GNEFileDialog::getDirectory() const {
    return myFileSelector->getDirectory();
}


void
GNEFileDialog::setCurrentPattern(FXint n) {
    myFileSelector->setCurrentPattern(n);
}


FXint
GNEFileDialog::getCurrentPattern() const {
    return myFileSelector->getCurrentPattern();
}


FXString
GNEFileDialog::getPatternText(FXint patno) const {
    return myFileSelector->getPatternText(patno);
}


void
GNEFileDialog::setPatternText(FXint patno, const FXString& text) {
    myFileSelector->setPatternText(patno, text);
}


FXint
GNEFileDialog::getNumPatterns() const {
    return myFileSelector->getNumPatterns();
}


void
GNEFileDialog::allowPatternEntry(FXbool allow) {
    myFileSelector->allowPatternEntry(allow);
}


FXbool
GNEFileDialog::allowPatternEntry() const {
    return myFileSelector->allowPatternEntry();
}


void
GNEFileDialog::setItemSpace(FXint s) {
    myFileSelector->setItemSpace(s);
}


FXint
GNEFileDialog::getItemSpace() const {
    return myFileSelector->getItemSpace();
}


void
GNEFileDialog::setFileBoxStyle(FXuint style) {
    myFileSelector->setFileBoxStyle(style);
}


FXuint
GNEFileDialog::getFileBoxStyle() const {
    return myFileSelector->getFileBoxStyle();
}


void
GNEFileDialog::setMatchMode(FXuint mode) {
    myFileSelector->setMatchMode(mode);
}


FXuint
GNEFileDialog::getMatchMode() const {
    return myFileSelector->getMatchMode();
}


FXbool
GNEFileDialog::showHiddenFiles() const {
    return myFileSelector->showHiddenFiles();
}


void
GNEFileDialog::showHiddenFiles(FXbool showing) {
    myFileSelector->showHiddenFiles(showing);
}


FXbool
GNEFileDialog::showImages() const {
    return myFileSelector->showImages();
}


void
GNEFileDialog::showImages(FXbool showing) {
    myFileSelector->showImages(showing);
}


FXint
GNEFileDialog::getImageSize() const {
    return myFileSelector->getImageSize();
}


void
GNEFileDialog::setImageSize(FXint size) {
    myFileSelector->setImageSize(size);
}


long
GNEFileDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    getApp()->reg().writeIntEntry("File Dialog", "width", getWidth());
    getApp()->reg().writeIntEntry("File Dialog", "height", getHeight());
    getApp()->reg().writeUnsignedEntry("File Dialog", "style", getFileBoxStyle());
    getApp()->reg().writeUnsignedEntry("File Dialog", "showhidden", showHiddenFiles());
    return 1;
}

/****************************************************************************/
