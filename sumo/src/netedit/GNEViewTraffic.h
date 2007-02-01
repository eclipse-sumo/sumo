/****************************************************************************/
/// @file    GNEViewTraffic.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 15 Dec 2004
/// @version $Id: $
///
// A view on the simulation; this view is a microscopic one
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
#ifndef GNEViewTraffic_h
#define GNEViewTraffic_h
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
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <gui/GUIViewTraffic.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
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



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEViewTraffic
 * Microsocopic view at the simulation
 */
class GNEViewTraffic : public GUIViewTraffic
{
    FXDECLARE(GNEViewTraffic)
public:
    /// constructor
    GNEViewTraffic(FXComposite *p, GUIMainWindow &app,
                   GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis);

    /// constructor
    GNEViewTraffic(FXComposite *p, GUIMainWindow &app,
                   GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis,
                   FXGLCanvas *share);
    /// destructor
    virtual ~GNEViewTraffic();

    /// builds the view toolbars
    void buildViewToolBars(GUIGlChildWindow &);

    long onLeftBtnRelease(FXObject*sender,FXSelector selector,void*data);

    long onCmdEditGraph(FXObject*,FXSelector,void*);

protected:

    GUISUMOViewParent *par;


protected:
    GNEViewTraffic()
    { }
};


#endif

/****************************************************************************/

