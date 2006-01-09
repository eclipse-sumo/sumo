#ifndef GUIViewTraffic_h
#define GUIViewTraffic_h
//---------------------------------------------------------------------------//
//                        GUIViewTraffic.h -
//  A view on the simulation; this view is a microscopic one
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
// Revision 1.26  2006/01/09 11:50:20  dkrajzew
// new visualization settings implemented
//
// Revision 1.25  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.24  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.23  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.22  2005/05/04 07:50:16  dkrajzew
// ported to fox1.4
//
// Revision 1.21  2004/12/15 09:20:17  dkrajzew
// made guisim independent of giant/netedit
//
// Revision 1.20  2004/12/12 17:23:58  agaubatz
// Editor Tool Widgets included
//
// Revision 1.19  2004/11/23 10:11:34  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.18  2004/08/02 11:55:35  dkrajzew
// using coloring schemes stored in a container
//
// Revision 1.17  2004/07/02 08:31:35  dkrajzew
// detector drawer now also draw other additional items; removed some memory
//  leaks;
//  some further drawing options (mainly for the online-router added)
//
// Revision 1.16  2004/06/17 13:06:55  dkrajzew
// Polygon visualisation added
//
// Revision 1.15  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.14  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.13  2003/09/05 14:55:11  dkrajzew
// lighter drawer implementations
//
// Revision 1.12  2003/08/14 13:44:14  dkrajzew
// tls/row - drawer added
//
// Revision 1.11  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.10  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.9  2003/07/16 15:18:24  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.8  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.7  2003/05/20 09:23:55  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.6  2003/04/16 09:50:05  dkrajzew
// centering of the network debugged; additional parameter of maximum display
//  size added
//
// Revision 1.4  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.3  2003/02/07 10:34:15  dkrajzew
// files updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/foxtools/FXMutex.h>
#include "GUISUMOViewParent.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <utils/gui/drawer/GUIBaseLaneDrawer.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class GUIEdge;
class GUIPerspectiveChanger;
class GUIBaseVehicleDrawer;
class GUIBaseDetectorDrawer;
class GUIBaseJunctionDrawer;
class GUIBaseROWDrawer;



/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIViewTraffic
 * Microsocopic view at the simulation
 */
class GUIViewTraffic : public GUISUMOAbstractView {
  FXDECLARE(GUIViewTraffic)
public:
    /// constructor
    GUIViewTraffic(FXComposite *p, GUIMainWindow &app,
        GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis);

    /// constructor
    GUIViewTraffic(FXComposite *p, GUIMainWindow &app,
        GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis,
        FXGLCanvas *share);
    void init(GUINet &net) ;

    void create();

    /// destructor
    virtual ~GUIViewTraffic();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow &);

    void track(int id);

    long onCmdShowFullGeom(FXObject*,FXSelector,void*);
    long onCmdChangeColorScheme(FXObject*,FXSelector sel,void*);

    void centerTo(GUIGlObject *o);

    /// shows a vehicle route
    void showRoute(GUIVehicle *v, int index=-1);

    /// hides a vehicle route
    void hideRoute(GUIVehicle *v, int index=-1);

    /// Returns the information whether the route of the given vehicle is shown
    bool amShowingRouteFor(GUIVehicle *v, int index=-1);

    virtual long onCmdEditView(FXObject*,FXSelector,void*);

protected:
    void draw(const MSRoute &r);
public:

protected:
    void doPaintGL(int mode, SUMOReal scale);

    void doInit();

    /// returns the color of the edge
    RGBColor getEdgeColor(GUIEdge *edge) const;

    void drawRoute(const VehicleOps &vo, int routeNo, SUMOReal darken);

protected:
    /** @brief Instances of the vehicle drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseVehicleDrawer* myVehicleDrawer[8];

    /** @brief Instances of the lane drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseLaneDrawer<GUIEdge, GUIEdge, GUILaneWrapper> *myLaneDrawer[8];

    /** @brief Instances of the junction drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseJunctionDrawer *myJunctionDrawer[8];

    /** @brief Instances of the detectors drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseDetectorDrawer *myDetectorDrawer[8];

    /** @brief Instances of the right of way drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseROWDrawer *myROWDrawer[8];

    /// The coloring scheme of junctions to use
    JunctionColoringScheme _junctionColScheme;

    int myTrackedID;

    /// Information whether the full or the simle geometry shall be used
    bool myUseFullGeom;

    /** @brief Pointers to tables holding the information which of the items are visible
        All vehicles on visible edges will be drawn */
    size_t *_edges2Show, *_junctions2Show, *_additional2Show;

    /// The absolut numbers of the array sizes
    size_t _edges2ShowSize, _junctions2ShowSize, _additional2ShowSize;

    GUIColoringSchemesMap<GUILaneWrapper> myLaneColoringSchemes;

	GUINet *_net;

protected:
    GUIViewTraffic() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif

// Local Variables:
// mode:C++
// End:

