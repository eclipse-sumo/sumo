#ifndef GUIViewAggregatedLanes_h
#define GUIViewAggregatedLanes_h
//---------------------------------------------------------------------------//
//                        GUIViewAggregatedLanes.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                :
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
// Revision 1.9  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 07:49:52  dkrajzew
// ported to fox1.4
//
// Revision 1.5  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.4  2004/08/02 11:55:35  dkrajzew
// using coloring schemes stored in a container
//
// Revision 1.3  2004/07/02 08:30:59  dkrajzew
// detector drawer now also draw other additional items; removed some memory leaks
//
// Revision 1.2  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.1  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
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
#include <utils/foxtools/FXMutex.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include "GUISUMOViewParent.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>

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
class GUIBaseROWDrawer;
class GUIBaseDetectorDrawer;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIViewAggregatedLanes
 * Microsocopic view at the simulation
 */
class GUIViewAggregatedLanes
    : public GUISUMOAbstractView {
    FXDECLARE(GUIViewAggregatedLanes)
public:
    /// constructor
    GUIViewAggregatedLanes(FXComposite *p, GUIMainWindow &app,
        GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis);

    /// constructor
    GUIViewAggregatedLanes(FXComposite *p, GUIMainWindow &app,
        GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis,
        FXGLCanvas *share);

    /// destructor
    virtual ~GUIViewAggregatedLanes();

    void create();

    /// builds the view toolbars
    void buildViewToolBars(GUIGlChildWindow &);

    long onCmdColourLanes(FXObject*,FXSelector,void*);
    long onCmdAggChoose(FXObject*,FXSelector,void*);
    long onCmdShowFullGeom(FXObject*,FXSelector,void*);
    long onCmdAggMemory(FXObject*,FXSelector,void*);



protected:
    void doPaintGL(int mode, SUMOReal scale);

    void doInit();

    /// returns the color of the edge
    RGBColor getEdgeColor(GUIEdge *edge) const;


private:
    void init(GUINet &net);


protected:
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
//    GUIBaseDetectorDrawer *myDetectorDrawer[8];

    /** @brief Instances of the right of way drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseROWDrawer *myROWDrawer[8];


    /// the coloring scheme of lanes to use
    GUIBaseColorer<GUILaneWrapper> *myLaneColorer;

    JunctionColoringScheme _junctionColScheme;

    size_t *_edges2Show, *_junctions2Show, *_additional2Show;
    size_t _edges2ShowSize, _junctions2ShowSize, _additional2ShowSize;

    FXPopup *myLaneColoring;
    FXComboBox *myAggregationLength;
    FXRealSpinDial *myRememberingFactor;

    /// Information whether the full or the simle geometry shall be used
    bool myUseFullGeom;


    GUIColoringSchemesMap<GUISUMOAbstractView::LaneColoringScheme, GUILaneWrapper>
        myLaneColoringSchemes;

    GUINet *_net;


protected:
    GUIViewAggregatedLanes() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif

// Local Variables:
// mode:C++
// End:

