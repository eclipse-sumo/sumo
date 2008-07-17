/****************************************************************************/
/// @file    MFXUtils.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// Some helper functions for FOX
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MFXUtils.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MFXUtils::deleteChildren(FXWindow *w) throw()
{
    while (w->numChildren()!=0) {
        FXWindow *child = w->childAtIndex(0);
        delete child;
    }
}


FXbool
MFXUtils::userPermitsOverwritingWhenFileExists(FXWindow * const parent,
        const FXString &file) throw()
{
    if (!FXFile::exists(file)) {
        return TRUE;
    }
    int answer =
        FXMessageBox::question(parent, MBOX_YES_NO, "File Exists", "Overwrite '%s'?", file.text());
    if (answer==MBOX_CLICKED_NO) {
        return FALSE;
    }
    return TRUE;
}


FXString
MFXUtils::getDocumentName(const FXString &filename) throw()
{
    FXString file = FXFile::name(filename);
    return file.before('.');
}


FXString 
MFXUtils::getTitleText(const FXString &appname, FXString filename) throw()
{
    if(filename.length()==0) {
        return appname;
    }
    return getDocumentName(filename) + " - " + appname;
}



/****************************************************************************/

