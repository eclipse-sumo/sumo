/****************************************************************************/
/// @file    GUIUserIO.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-10-12
/// @version $Id$
///
// Some OS-dependant functions to ease cliboard manipulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include "GUIUserIO.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


#ifdef WIN32
#include <windows.h>
#endif

// ===========================================================================
// method definitions
// ===========================================================================
#ifdef WIN32
void
GUIUserIO::copyToClipboard(const FXApp &app, const std::string &text) throw() {
    OpenClipboard((HWND) app.getRootWindow()->getFirst()->id());
    ::EmptyClipboard();
    HGLOBAL clipBuffer = GlobalAlloc(GMEM_DDESHARE, text.length()+1);
    char *buff = (char*)GlobalLock(clipBuffer);
    strcpy(buff, text.c_str());
    ::SetClipboardData(CF_TEXT, buff);
    CloseClipboard();
}


#else

void
GUIUserIO::copyToClipboard(const FXApp &, const std::string &) throw() {}

#endif

/****************************************************************************/

