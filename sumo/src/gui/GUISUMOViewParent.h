/****************************************************************************/
/// @file    GUISUMOViewParent.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A window that controls the display(s) of the simulation
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
#ifndef GUISUMOViewParent_h
#define GUISUMOViewParent_h
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
 * This class represents a single view on the application. It is made of a
 * tool-bar containing a field to change the type of display, buttons that
 * allow to choose an artifact and some other view controlling options.
 * The rest of the window is a canvas that contains the display itself
 */
class GUISUMOViewParent : public GUIGlChildWindow
{
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

    long onCmdRecenterView(FXObject*,FXSelector,void*);
    long onCmdShowLegend(FXObject*,FXSelector,void*);
    long onCmdMakeSnapshot(FXObject*sender,FXSelector,void*);
    long onCmdAllowRotation(FXObject*,FXSelector,void*);
    long onCmdLocateJunction(FXObject*,FXSelector,void*);
    long onCmdLocateEdge(FXObject*,FXSelector,void*);
    long onCmdLocateVehicle(FXObject*,FXSelector,void*);
    long onCmdLocateAdd(FXObject *sender,FXSelector,void*);
    long onSimStep(FXObject*sender,FXSelector,void*);

    /// centers the view onto the given artifact
    void setView(GUIGlObject *o);

    /// returns the zooming factor
    SUMOReal getZoomingFactor() const;

    /// sets the zooming factor
    void setZoomingFactor(SUMOReal val);

    /// returns the information whether the legend shall be shown
    bool showLegend() const;

    /// information whether the canvas may be rotated
    bool allowRotation() const;

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    GUIMainWindow *getParent()
    {
        return myParent;
    }

protected:
    /** builds the toolbar  */
    virtual void buildToolBar(FXComposite *c);

    /** builds the edit frame  */
//	void buildEditFrame(FXComposite *c);

    /** build the artifact choosing toolbar */
    void buildTrackingTools();

protected:
    /// The parent window
    GUIMainWindow *myParent;

private:
    /// the zooming factor
    SUMOReal _zoomingFactor;

    /// information whether the legend shall be shown
    bool _showLegend;

    /// information whether the canvas may be rotated
    bool _allowRotation;

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
    GUIDialog_GLObjChooser *_chooser;

protected:
    GUISUMOViewParent()
    { }

};


#endif

/****************************************************************************/

