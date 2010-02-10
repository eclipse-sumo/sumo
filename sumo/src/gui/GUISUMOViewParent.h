/****************************************************************************/
/// @file    GUISUMOViewParent.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single child window which contains a view of the simulation area
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
#include "dialogs/GUIDialog_GLObjChooser.h"
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/windows/GUIGlChildWindow.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUISUMOAbstractView;
class GUIApplicationWindow;


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
    GUISUMOViewParent(FXMDIClient* p, FXMDIMenu *mdimenu,
                      const FXString& name, GUIMainWindow *parentWindow,
                      FXIcon* ic=NULL, FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);


    /** @brief "Initialises" this window by building the contents
     * @param[in] share A canvas tor get the shared context from
     * @param[in] net The network to show
     * @todo Check whether this could be done in the constructor
     */
    virtual GUISUMOAbstractView* init(FXGLCanvas *share, GUINet &net);


    /// @brief Destructor
    ~GUISUMOViewParent();


    /// @brief Called if the user wants to make a snapshot (screenshot)
    long onCmdMakeSnapshot(FXObject*sender,FXSelector,void*);

    /// @brief Called on a simulation step
    long onSimStep(FXObject*sender,FXSelector,void*);


    /// @name locator-callbacks
    //@{

    /// @brief opens a locator for junctions
    long onCmdLocateJunction(FXObject*,FXSelector,void*);
    /// @brief opens a locator for edges
    long onCmdLocateEdge(FXObject*,FXSelector,void*);
    /// @brief opens a locator for vehicles
    long onCmdLocateVehicle(FXObject*,FXSelector,void*);
    /// @brief opens a locator for traffic lights logics
    long onCmdLocateTLS(FXObject*,FXSelector,void*);
    /// @brief opens a locator for additional structures
    long onCmdLocateAdd(FXObject *sender,FXSelector,void*);
    /// @brief opens a locator for shapes
    long onCmdLocateShape(FXObject*,FXSelector,void*);
    //@}


    /** @brief Centers the view onto the given artifact
     * @param[in] o The object to center the view on
     */
    void setView(const GUIGlObject * const o) throw();


    /** @brief Returns the main window
     * @return This view's parent
     */
    GUIMainWindow *getParent() throw() {
        return myParent;
    }


protected:
    /** @brief Builds a locator for the given type
     * @param[in] type gl-object type of objects to build the locator for
     * @param[in] icon The icon to use
     * @param[in] title The title to use
     */
    void showLocator(GUIGlObjectType type, FXIcon *icon, FXString title);


protected:
    /// @brief The parent window
    GUIMainWindow *myParent;


protected:
    /// @brief FOX needs this
    GUISUMOViewParent() { }

};


#endif

/****************************************************************************/

