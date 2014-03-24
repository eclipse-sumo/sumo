/****************************************************************************/
/// @file    MFXUtils.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// Some helper functions for FOX
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2006-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/RGBColor.h>
#include "MFXUtils.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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

