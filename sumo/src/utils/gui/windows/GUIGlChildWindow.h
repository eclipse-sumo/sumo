/****************************************************************************/
/// @file    GUIGlChildWindow.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIGlChildWindow_h
#define GUIGlChildWindow_h



// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
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
class GUIGlChildWindow : public FXMDIChild {
    FXDECLARE(GUIGlChildWindow)
public:
    GUIGlChildWindow(FXMDIClient* p, FXMDIMenu *mdimenu, const FXString& name,
                     FXIcon* ic=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
                    );

    virtual ~GUIGlChildWindow();

    virtual FXGLCanvas* getBuildGLCanvas() const;
    virtual void create();

    GUISUMOAbstractView* getView() const throw() {
        return myView;
    }

    long onCmdRecenterView(FXObject*,FXSelector,void*);
    long onCmdEditViewport(FXObject*,FXSelector,void*);
    long onCmdEditViewScheme(FXObject*,FXSelector,void*);
    long onCmdShowToolTips(FXObject*sender,FXSelector,void*);
    long onCmdChangeColorScheme(FXObject*,FXSelector sel,void*);

    virtual void buildNavigationToolBar();
    virtual void buildColoringToolBar();
    virtual void buildScreenshotToolBar();


    FXToolBar &getNavigationToolBar(GUISUMOAbstractView &v);
    FXPopup *getLocatorPopup();
    FXComboBox &getColoringSchemesCombo();

protected:
    /// the view
    GUISUMOAbstractView *myView;

    /// The tool bar
    FXToolBar *myNavigationToolBar;

    /// The locator menu
    FXPopup *myLocatorPopup;
    FXMenuButton *myLocatorButton;

    FXVerticalFrame *myContentFrame;

    FXComboBox *myColoringSchemes;


protected:
    GUIGlChildWindow() { }

};


#endif

/****************************************************************************/

