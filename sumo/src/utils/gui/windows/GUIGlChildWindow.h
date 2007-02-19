/****************************************************************************/
/// @file    GUIGlChildWindow.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
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
#ifndef GUIGlChildWindow_h
#define GUIGlChildWindow_h

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

#include <fx.h>
#include <fx3d.h>

#include "GUISUMOAbstractView.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GUIGlChildWindow : public FXMDIChild
{
public:
    GUIGlChildWindow(FXMDIClient* p, FXMDIMenu *mdimenu, const FXString& name,
                     FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
                    );

    virtual ~GUIGlChildWindow();

    virtual FXGLCanvas *getBuildGLCanvas() const;

    virtual bool showLegend() const = 0;
    virtual bool allowRotation() const = 0;

    FXToolBar &getToolBar(GUISUMOAbstractView &v);

protected:
    /// the view
    GUISUMOAbstractView *_view;

    /// The tool bar
    FXToolBar *myToolBar;

protected:
    GUIGlChildWindow()
    { }

};


#endif

/****************************************************************************/

