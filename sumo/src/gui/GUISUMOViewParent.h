#ifndef GUISUMOViewParent_h
#define GUISUMOViewParent_h
//---------------------------------------------------------------------------//
//                        GUISUMOViewParent.h -
//  A window that controls the display(s) of the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.2  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.1  2003/05/20 09:25:14  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.4  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.3  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <fx.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include "dialogs/GUIDialog_GLObjChooser.h"
#include "GUIGlObjectTypes.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class GUISUMOAbstractView;
class GUIApplicationWindow;


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * This class represents a single view on the application. It is made of a
 * tool-bar containing a field to change the type of display, buttons that
 * allow to choose an artifact and some other view controlling options.
 * The rest of the window is a canvas that contains the display itself
 */
class GUISUMOViewParent : public FXMDIChild
{
    // FOX-declarations
    FXDECLARE(GUISUMOViewParent)
public:
    enum ViewType {
        MICROSCOPIC_VIEW,
        LANE_AGGREGATED_VIEW
    };

    /// constructor
    /*
    GUISUMOViewParent( FXApp *a, FXMDIClient* p, FXMDIMenu *mdimenu,
        const FXString& name,
        GUINet &net, GUIApplicationWindow *parentWindow, ViewType view,
        FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
        );
*/
    GUISUMOViewParent( FXMDIClient* p, FXGLCanvas *share,
        FXMDIMenu *mdimenu, const FXString& name,
        GUINet &net, GUIApplicationWindow *parentWindow, ViewType view,
        FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
        );
    void init(ViewType view, FXGLCanvas *share, GUINet &net);


    FXGLCanvas *getBuildGLCanvas() const;

    /// destructor
    ~GUISUMOViewParent();

    void create();

    long onCmdRecenterView(FXObject*,FXSelector,void*);
    long onCmdShowLegend(FXObject*,FXSelector,void*);
    long onCmdAllowRotation(FXObject*,FXSelector,void*);
    long onCmdLocateJunction(FXObject*,FXSelector,void*);
    long onCmdLocateEdge(FXObject*,FXSelector,void*);
    long onCmdLocateVehicle(FXObject*,FXSelector,void*);
    long onSimStep(FXObject*sender,FXSelector,void*);
/*
    long onLeftBtnRelease(FXObject*sender,FXSelector,void*ptr);
    long onRightBtnRelease(FXObject*sender,FXSelector,void*ptr);
    long onMouseMove(FXObject*sender,FXSelector,void*ptr);
*/
    /// centers the view onto the given artifact
    void setView(GUIGlObjectType type, const std::string &name);

    /// returns the zooming factor
    double getZoomingFactor() const;

    /// sets the zooming factor
    void setZoomingFactor(double val);

    /// returns the information whether the legend shall be shown
    bool showLegend() const;

    /// information whether the canvas may be rotated
    bool allowRotation() const;

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    FXToolBar &getToolBar(GUISUMOAbstractView &v);

private:
    /** builds the toolbar  */
    void buildToolBar(FXComposite *c);

    /** build the artifact choosing toolbar */
    void buildTrackingTools();

    /** build the GUIDialog_GLObjChooser which contains the given values */
    void showValues(GUIGlObjectType type,
        std::vector<std::string> &names);

private:
    /// the zooming factor
    double _zoomingFactor;

    /// the view used
    GUISUMOAbstractView *_view;

    /// information whether the legend shall be shown
    bool _showLegend;

    /// information whether the canvas may be rotated
    bool _allowRotation;

    /// the artifact chooser
    GUIDialog_GLObjChooser *_chooser;

    /// The parent window
    GUIApplicationWindow *myParent;

    /// The tool bar
    FXToolBar *myToolBar;

   /// The thing that makes the toolbar float
//    FXToolBarShell *myToolBarDrag;

protected:
    GUISUMOViewParent() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

