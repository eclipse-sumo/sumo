/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlChildWindow.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
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
    GUIGlChildWindow(FXMDIClient* p, GUIMainWindow* parentWindow,
                     FXMDIMenu* mdimenu, const FXString& name,
                     FXIcon* ic = NULL, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    virtual ~GUIGlChildWindow();

    virtual FXGLCanvas* getBuildGLCanvas() const;
    virtual void create();

    GUISUMOAbstractView* getView() const {
        return myView;
    }

    long onCmdRecenterView(FXObject*, FXSelector, void*);
    long onCmdEditViewport(FXObject*, FXSelector, void*);
    long onCmdEditViewScheme(FXObject*, FXSelector, void*);
    long onCmdShowToolTips(FXObject* sender, FXSelector, void*);
    long onCmdZoomStyle(FXObject* sender, FXSelector, void*);
    long onCmdChangeColorScheme(FXObject*, FXSelector sel, void*);

    void buildNavigationToolBar();
    void buildColoringToolBar();
    void buildScreenshotToolBar();


    FXToolBar& getNavigationToolBar(GUISUMOAbstractView& v);
    FXPopup* getLocatorPopup();
    FXComboBox& getColoringSchemesCombo();

    /** @brief Centers the view onto the given artifact
     * @param[in] id The id of the object to center the view on
     */
    void setView(GUIGlID id);


    /** @brief Returns the main window
     * @return This view's parent
     */
    GUIMainWindow* getParent() {
        return myParent;
    }


    /// @brief true if the object is selected (may include extra logic besides calling gSelected)
    virtual bool isSelected(GUIGlObject* o) const;


protected:
    /// the view
    GUISUMOAbstractView* myView;

    /// The tool bar
    FXToolBar* myNavigationToolBar;

    /// The locator menu
    FXPopup* myLocatorPopup;
    FXMenuButton* myLocatorButton;

    FXVerticalFrame* myContentFrame;

    FXComboBox* myColoringSchemes;

    /// @brief The parent window
    GUIMainWindow* myParent;


protected:
    GUIGlChildWindow() { }

};


#endif

/****************************************************************************/

