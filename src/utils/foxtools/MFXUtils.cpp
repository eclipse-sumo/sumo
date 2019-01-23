/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXUtils.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// Some helper functions for FOX
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/RGBColor.h>
#include "MFXUtils.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
MFXUtils::deleteChildren(FXWindow* w) {
    while (w->numChildren() != 0) {
        FXWindow* child = w->childAtIndex(0);
        delete child;
    }
}


FXbool
MFXUtils::userPermitsOverwritingWhenFileExists(FXWindow* const parent,
        const FXString& file) {
    if (!FXStat::exists(file)) {
        return TRUE;
    }
    int answer =
        FXMessageBox::question(parent, MBOX_YES_NO, "File Exists", "Overwrite '%s'?", file.text());
    if (answer == MBOX_CLICKED_NO) {
        return FALSE;
    }
    return TRUE;
}


FXString
MFXUtils::getDocumentName(const FXString& filename) {
    return FXPath::name(filename);
}


FXString
MFXUtils::getTitleText(const FXString& appname, FXString filename) {
    if (filename.length() == 0) {
        return appname;
    }
    return getDocumentName(filename) + " - " + appname;
}


FXString
MFXUtils::assureExtension(const FXString& filename, const FXString& defaultExtension) {
    FXString ext = FXPath::extension(filename);
    if (ext == "") {
        if (filename.rfind('.') == filename.length() - 1) {
            return filename + defaultExtension;
        }
        return filename + "." + defaultExtension;
    }
    return filename;
}


FXString
MFXUtils::getFilename2Write(FXWindow* parent,
                            const FXString& header, const FXString& extension,
                            FXIcon* icon, FXString& currentFolder) {
    // get the new file name
    FXFileDialog opendialog(parent, header);
    opendialog.setIcon(icon);
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*" + extension);
    if (currentFolder.length() != 0) {
        opendialog.setDirectory(currentFolder);
    }
    if (!opendialog.execute()) {
        return "";
    }
    FXString file = assureExtension(opendialog.getFilename(), extension.after('.')).text();
    if (!userPermitsOverwritingWhenFileExists(parent, file)) {
        return "";
    }
    currentFolder = opendialog.getDirectory();
    return file;
}


RGBColor
MFXUtils::getRGBColor(FXColor col) {
    return RGBColor(FXREDVAL(col), FXGREENVAL(col), FXBLUEVAL(col), FXALPHAVAL(col));
}


FXColor
MFXUtils::getFXColor(const RGBColor& col) {
    return FXRGBA(col.red(), col.green(), col.blue(), col.alpha());
}


/****************************************************************************/

