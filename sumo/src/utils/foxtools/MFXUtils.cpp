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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MFXUtils.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
MFXUtils::deleteChildren(FXWindow *w)
{
    while (w->numChildren()!=0) {
        FXWindow *child = w->childAtIndex(0);
        delete child;
    }
}


FXbool
MFXUtils::userPermitsOverwritingWhenFileExists(FXWindow *parent,
                                               FXString &file)
{
    if(!FXFile::exists(file)) {
        return TRUE;
    }
    int answer = 
        FXMessageBox::question(parent, MBOX_YES_NO, "File Exists", 
            "Overwrite '%s'?", file);
    if(answer==MBOX_CLICKED_NO) {
        return FALSE;
    }
    return TRUE;
}



/****************************************************************************/

