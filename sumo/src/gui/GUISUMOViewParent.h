/****************************************************************************/
/// @file    GUISUMOViewParent.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single child window which contains a view of the simulation area
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
 * The rest of the window is a canvas that contains the display itself
 */
class GUISUMOViewParent : public GUIGlChildWindow {
    // FOX-declarations
    FXDECLARE(GUISUMOViewParent)
public:
    enum ViewType {
        MICROSCOPIC_VIEW
#ifdef HAVE_MESOSIM
        ,EDGE_MESO_VIEW
#endif
    };

    /// constructor
    GUISUMOViewParent(FXMDIClient* p, FXGLCanvas *share,
                      FXMDIMenu *mdimenu, const FXString& name,
                      GUINet &net, GUIMainWindow *parentWindow, ViewType view,
                      FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
                     );

    virtual void init(ViewType view, FXGLCanvas *share, GUINet &net);


    /// destructor
    ~GUISUMOViewParent();

    virtual void create();

    long onCmdMakeSnapshot(FXObject*sender,FXSelector,void*);
    long onSimStep(FXObject*sender,FXSelector,void*);

    /// @name locator-callbacks
    //@{

    /// opens a locator for junctions
    long onCmdLocateJunction(FXObject*,FXSelector,void*);
    /// opens a locator for edges
    long onCmdLocateEdge(FXObject*,FXSelector,void*);
    /// opens a locator for vehicles
    long onCmdLocateVehicle(FXObject*,FXSelector,void*);
    /// opens a locator for traffic lights logics
    long onCmdLocateTLS(FXObject*,FXSelector,void*);
    /// opens a locator for additional structures
    long onCmdLocateAdd(FXObject *sender,FXSelector,void*);
    /// opens a locator for shapes
    long onCmdLocateShape(FXObject*,FXSelector,void*);
    //@}

    /// centers the view onto the given artifact
    void setView(GUIGlObject *o);

    /// information whether the canvas may be rotated
//    bool allowRotation() const;

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    GUIMainWindow *getParent() {
        return myParent;
    }

protected:
    /** build the artifact choosing toolbar */
    void buildTrackingTools();

    /// Instantiates an object locator
    void showLocator(GUIGlObjectType type);

protected:
    /// The parent window
    GUIMainWindow *myParent;

private:
    /// information whether the canvas may be rotated
    //  bool myAllowRotation;

    FXGroupBox  *groupBox;
    FXTextField *JunctionNameTextField;
    FXTextField *xPosTextField;
    FXTextField *yPosTextField;
    FXTextField *LaneNameTextField;
    FXTextField *LaneStartTextField;
    FXTextField *LaneEndTextField;
    FXTextField *MaxSpeedTextField;
    FXTextField *LengthTextField;
    /// the artifact chooser
    GUIDialog_GLObjChooser *myChooser;

//    FXVerticalFrame *myContentFrame;

protected:
    GUISUMOViewParent() { }

};


#endif

/****************************************************************************/

