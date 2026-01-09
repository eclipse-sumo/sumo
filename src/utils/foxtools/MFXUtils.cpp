/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2026 German Aerospace Center (DLR) and others.
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
/// @file    MFXUtils.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
///
// Some helper functions for FOX
/****************************************************************************/
#include <config.h>

#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>

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
        FXMessageBox::question(parent, MBOX_YES_NO, TL("File Exists"), TL("Overwrite '%s'?"), file.text());
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
MFXUtils::assureExtension(const FXFileDialog& openDialog) {
    const auto extensions = parseExtensions(openDialog.getPatternText(openDialog.getCurrentPattern()));
    const auto filename = openDialog.getFilename();
    // iterate over all extension to check if is the same extension
    for (const auto& extension : extensions) {
        if (extension.length() < filename.length()) {
            bool sameExtension = true;
            for (auto i = 0; i < extension.length(); i++) {
                if (filename[i + filename.length() - extension.length()] != extension[i]) {
                    sameExtension = false;
                }
            }
            if (sameExtension) {
                return filename;
            }
        }
    }
    // in this point, we have to give an extension (if exist)
    if (extensions.size() > 0) {
        return filename + "." + extensions.front();
    } else {
        return filename;
    }
}


std::vector<FXString>
MFXUtils::parseExtensions(FXString patternText) {
    std::vector<FXString> extensions;
    // first take elementes between parentheses
    patternText = patternText.after('(');
    patternText = patternText.before(')');
    // check files extension
    if (patternText != "*") {
        // split extensions
        const auto extensionsStr = StringTokenizer(patternText.text(), ",").getVector();
        for (const auto& extensionStr : extensionsStr) {
            FXString extension = extensionStr.c_str();
            extensions.push_back(extension.after('.'));
        }
    }
    return extensions;
}


FXString
MFXUtils::getFilename2Write(FXWindow* parent, const FXString& header, const FXString& extensions,
                            FXIcon* icon, FXString& currentFolder) {
    // get the new file name
    FXFileDialog opendialog(parent, header);
    opendialog.setIcon(icon);
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList(extensions);
    if (currentFolder.length() != 0) {
        opendialog.setDirectory(currentFolder);
    }
    if (!opendialog.execute()) {
        return "";
    }
    const auto filename = assureExtension(opendialog);
    if (!userPermitsOverwritingWhenFileExists(parent, filename)) {
        return "";
    }
    currentFolder = opendialog.getDirectory();
    return filename;
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
