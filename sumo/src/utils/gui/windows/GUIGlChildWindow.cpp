/****************************************************************************/
/// @file    GUIGlChildWindow.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id: $
///
//
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

#include "GUIGlChildWindow.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// member method definitions
// ===========================================================================
GUIGlChildWindow::GUIGlChildWindow(FXMDIClient* p,
                                   FXMDIMenu *mdimenu, const FXString& name,
                                   FXIcon* ic, FXPopup* /*pup*/,
                                   FXuint opts,FXint /*x*/,FXint /*y*/,FXint /*w*/,FXint /*h*/)
        : FXMDIChild(p, name, ic, mdimenu, opts, 10, 10, 300, 200),
        _view(0)
{}


GUIGlChildWindow::~GUIGlChildWindow()
{}


FXGLCanvas *
GUIGlChildWindow::getBuildGLCanvas() const
{
    return _view;
}


FXToolBar &
GUIGlChildWindow::getToolBar(GUISUMOAbstractView &)
{
    return *myToolBar;
}



/****************************************************************************/

