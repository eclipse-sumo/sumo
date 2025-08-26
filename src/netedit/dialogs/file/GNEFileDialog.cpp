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
// FOX callback mapping
// ===========================================================================

// Object implementation
FXIMPLEMENT(GNEFileDialog, FXDialogBox, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileDialog::GNEFileDialog(FXWindow* owner, const FXString& name, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXDialogBox(owner, name, opts | DECOR_TITLE | DECOR_BORDER | DECOR_RESIZE | DECOR_CLOSE, x, y, w, h, 0, 0, 0, 0, 4, 4) {
    myFileSelector = new GNEFileSelector(this, NULL, 0, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    /*
    myFileSelector->acceptButton()->setTarget(this);
    myFileSelector->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
    myFileSelector->cancelButton()->setTarget(this);
    myFileSelector->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
    */
    setWidth(getApp()->reg().readIntEntry("File Dialog", "width", getWidth()));
    setHeight(getApp()->reg().readIntEntry("File Dialog", "height", getHeight()));
    setFileBoxStyle(getApp()->reg().readUnsignedEntry("File Dialog", "style", getFileBoxStyle()));
    showHiddenFiles(getApp()->reg().readUnsignedEntry("File Dialog", "showhidden", showHiddenFiles()));
}


GNEFileDialog::GNEFileDialog(FXApp* a, const FXString& name, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXDialogBox(a, name, opts | DECOR_TITLE | DECOR_BORDER | DECOR_RESIZE | DECOR_CLOSE, x, y, w, h, 0, 0, 0, 0, 4, 4) {
    myFileSelector = new GNEFileSelector(this, NULL, 0, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    /*
    myFileSelector->acceptButton()->setTarget(this);
    myFileSelector->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
    myFileSelector->cancelButton()->setTarget(this);
    myFileSelector->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
    */
    setWidth(getApp()->reg().readIntEntry("File Dialog", "width", getWidth()));
    setHeight(getApp()->reg().readIntEntry("File Dialog", "height", getHeight()));
    setFileBoxStyle(getApp()->reg().readUnsignedEntry("File Dialog", "style", getFileBoxStyle()));
    showHiddenFiles(getApp()->reg().readUnsignedEntry("File Dialog", "showhidden", showHiddenFiles()));
}


GNEFileDialog::~GNEFileDialog() {
    myFileSelector = (GNEFileSelector*) - 1L;
}


void
GNEFileDialog::hide() {
    FXDialogBox::hide();
    getApp()->reg().writeIntEntry("File Dialog", "width", getWidth());
    getApp()->reg().writeIntEntry("File Dialog", "height", getHeight());
    getApp()->reg().writeUnsignedEntry("File Dialog", "style", getFileBoxStyle());
    getApp()->reg().writeUnsignedEntry("File Dialog", "showhidden", showHiddenFiles());
}


void
GNEFileDialog::setFilename(const FXString& path) {
    myFileSelector->setFilename(path);
}


FXString
GNEFileDialog::getFilename() const {
    return myFileSelector->getFilename();
}


FXString*
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
GNEFileDialog::setPatternList(const FXString& patterns) {
    myFileSelector->setPatternList(patterns);
}


FXString
GNEFileDialog::getPatternList() const {
    return myFileSelector->getPatternList();
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
GNEFileDialog::setSelectMode(FXuint mode) {
    myFileSelector->setSelectMode(mode);
}


FXuint
GNEFileDialog::getSelectMode() const {
    return myFileSelector->getSelectMode();
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


void
GNEFileDialog::showReadOnly(FXbool show) {
    myFileSelector->showReadOnly(show);
}


FXbool
GNEFileDialog::shownReadOnly() const {
    return myFileSelector->shownReadOnly();
}


void
GNEFileDialog::setReadOnly(FXbool state) {
    myFileSelector->setReadOnly(state);
}


FXbool
GNEFileDialog::getReadOnly() const {
    return myFileSelector->getReadOnly();
}


FXString
GNEFileDialog::getOpenFilename(FXWindow* owner, const FXString& caption, const FXString& path, const FXString& patterns, FXint initial) {
    GNEFileDialog opendialog(owner, caption);
    FXString filename;
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setFilename(path);
    opendialog.setPatternList(patterns);
    opendialog.setCurrentPattern(initial);
    if (opendialog.execute()) {
        filename = opendialog.getFilename();
        if (FXStat::isFile(filename)) {
            return filename;
        }
    }
    return FXString::null;
}


FXString
GNEFileDialog::getSaveFilename(FXWindow* owner, const FXString& caption, const FXString& path, const FXString& patterns, FXint initial) {
    GNEFileDialog savedialog(owner, caption);
    savedialog.setSelectMode(SELECTFILE_ANY);
    savedialog.setFilename(path);
    savedialog.setPatternList(patterns);
    savedialog.setCurrentPattern(initial);
    if (savedialog.execute()) {
        return savedialog.getFilename();
    }
    return FXString::null;
}


FXString*
GNEFileDialog::getOpenFilenames(FXWindow* owner, const FXString& caption, const FXString& path, const FXString& patterns, FXint initial) {
    GNEFileDialog opendialog(owner, caption);
    opendialog.setSelectMode(SELECTFILE_MULTIPLE);
    opendialog.setFilename(path);
    opendialog.setPatternList(patterns);
    opendialog.setCurrentPattern(initial);
    if (opendialog.execute()) {
        return opendialog.getFilenames();
    }
    return NULL;
}


FXString
GNEFileDialog::getOpenDirectory(FXWindow* owner, const FXString& caption, const FXString& path) {
    GNEFileDialog dirdialog(owner, caption);
    FXString dirname;
    dirdialog.setSelectMode(SELECTFILE_DIRECTORY);
    dirdialog.setFilename(path);
    if (dirdialog.execute()) {
        dirname = dirdialog.getFilename();
        if (FXStat::isDirectory(dirname)) {
            return dirname;
        }
    }
    return FXString::null;
}

/****************************************************************************/
