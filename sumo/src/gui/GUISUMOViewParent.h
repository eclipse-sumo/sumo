/****************************************************************************/
/// @file    GUISUMOViewParent.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @date    Sept 2002
/// @version $Id$
///
// A single child window which contains a view of the simulation area
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUISUMOViewParent_h
#define GUISUMOViewParent_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/windows/GUIGlChildWindow.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUISUMOAbstractView;
class GUIDialog_GLObjChooser;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class GUISUMOViewParent
 * @brief A single child window which contains a view of the simulation area
 *
 * It is made of a tool-bar containing a field to change the type of display,
 *  buttons that allow to choose an artifact and some other view controlling
 *  options.
 *
 * The rest of the window is a canvas that contains the display itself
 */
class GUISUMOViewParent : public GUIGlChildWindow {
    // FOX-declarations
    FXDECLARE(GUISUMOViewParent)

public:
    /// @brief Available view types
    enum ViewType {
        /// @brief plain 2D openGL view (@see GUIViewTraffic)
        VIEW_2D_OPENGL,
        /// @brief plain 3D OSG view (@see GUIOSGView)
        VIEW_3D_OSG
    };


    /** @brief Constructor
     * @param[in] p The MDI-pane this window is shown within
     * @param[in] mdimenu The MDI-menu for alignment
     * @param[in] name The name of the window
     * @param[in] parentWindow The main window
     * @param[in] ic The icon of this window
     * @param[in] opts Window options
     * @param[in] x Initial x-position
     * @param[in] y Initial x-position
     * @param[in] w Initial width
     * @param[in] h Initial height
     */
    GUISUMOViewParent(FXMDIClient* p, FXMDIMenu* mdimenu,
                      const FXString& name, GUIMainWindow* parentWindow,
                      FXIcon* ic = NULL, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);


    /** @brief "Initialises" this window by building the contents
     * @param[in] share A canvas tor get the shared context from
     * @param[in] net The network to show
     * @param[in] vt The view type to use
     * @todo Check whether this could be done in the constructor
     */
    virtual GUISUMOAbstractView* init(FXGLCanvas* share, GUINet& net, ViewType type);


    /// @brief Destructor
    ~GUISUMOViewParent();


    /// @brief Called if the user wants to make a snapshot (screenshot)
    long onCmdMakeSnapshot(FXObject* sender, FXSelector, void*);

    /// @brief Called on a simulation step
    long onSimStep(FXObject* sender, FXSelector, void*);

    /// @brief  locator-callback
    long onCmdLocate(FXObject*, FXSelector, void*);

    /// @brief handle keys
    long onKeyPress(FXObject* o, FXSelector sel, void* data);
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);

    /// @brief true if the object is selected (may include extra logic besides calling gSelected)
    bool isSelected(GUIGlObject* o) const;

    /// @notify about toggled gaming status
    void setToolBarVisibility(const bool value);

protected:
    /// @brief FOX needs this
    GUISUMOViewParent() { }

};


#endif

/****************************************************************************/

