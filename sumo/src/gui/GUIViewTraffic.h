#ifndef GUIViewTraffic_h
#define GUIViewTraffic_h
//---------------------------------------------------------------------------//
//                        GUIViewTraffic.h -
//  A view on the simulation; this views is a microscopic one
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
// Revision 1.8  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.7  2003/05/20 09:23:55  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.6  2003/04/16 09:50:05  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.4  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.3  2003/02/07 10:34:15  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <qgl.h>
#include <qevent.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/Position2D.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/qutils/NewQMutex.h>
#include <utils/glutils/lfontrenderer.h>
#include <guisim/GUIEdgeGrid.h>
#include "GUISUMOViewParent.h"
#include "GUISUMOAbstractView.h"
#include "GUIChooser.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class QGLObjectToolTip;
class MSVehicle;
class GUINet;
class QPaintEvent;
class QResizeEvent;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class GUIEdge;
class GUIPerspectiveChanger;
class QTimerEvent;
class QPopupMenu;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIViewTraffic
 * Microsocopic view at the simulation
 */
class GUIViewTraffic
    : public GUISUMOAbstractView {

    /// is a q-object
    Q_OBJECT

public:
    /// constructor
    GUIViewTraffic(GUIApplicationWindow *app,
        GUISUMOViewParent *parent, GUINet &net);

    /// destructor
    virtual ~GUIViewTraffic();

    /// builds the view toolbars
    void buildViewToolBars(GUISUMOViewParent &);

    void track(int id);

public slots:
    /** changes the vehicle colouring scheme to the on stored under the given
        index */
    void changeVehicleColoringScheme(int index);

    /** changes the lane colouring scheme to the on stored under the given
        index */
    void changeLaneColoringScheme(int index);

public:
    /**
     * VehicleColoringScheme
     * This enumeration holds the possible vehicle colouring schemes
     */
    enum VehicleColoringScheme {
	    /// colouring by vehicle speed
        VCS_BY_SPEED = 0,
	    /// use the colour specified in the input
        VCS_SPECIFIED = 1,
	    /// use random scheme 1
        VCS_RANDOM1 = 2,
	    /// use random scheme 2
        VCS_RANDOM2 = 3,
	    /// use lanechanging scheme 1
        VCS_LANECHANGE1 = 4,
	    /// use lanechanging scheme 2
        VCS_LANECHANGE2 = 5,
        /// use lanechanging scheme 3
        VCS_LANECHANGE3 = 6,
        /// use waiting scheme 1
        VCS_WAITING1 = 7
    };

    /**
     * LaneColoringScheme
     * This enumeration holds the possible lane colouring schemes
     */
    enum LaneColoringScheme {
        /// all lanes will be black
        LCS_BLACK = 0,
	    /** colouring by purpose of the edge the lane lies in
            (sources:blue, sinks:red, normal:black) */
        LCS_BY_PURPOSE = 1,
        /// use the lane's speed
        LCS_BY_SPEED = 2
    };

public:
    /**
     * GUIVehicleDrawer
     * Classes derived from this are meant to be used fro vehicle drawing
     */
    class GUIVehicleDrawer {
    public:
	    /// constructor
        GUIVehicleDrawer() { }

	    /// destructor
        virtual ~GUIVehicleDrawer() { }

	    /// called before the first vehicle is drawn
        virtual void initStep() = 0;

	    /// draws a single vehicle; no tool-tip informations (faster)
        virtual void drawVehicleNoTooltips(const GUILaneWrapper &lane,
            const GUIVehicle &veh, VehicleColoringScheme scheme) = 0;

	    /// draws a single vehicle; tool-tip informations shall be generated
        virtual void drawVehicleWithTooltips(const GUILaneWrapper &lane,
            const GUIVehicle &veh, VehicleColoringScheme scheme) = 0;

	    /// ends the drawing of vehicles
        virtual void closeStep() = 0;
    };

    /**
     * GUILaneDrawer
     * Classes derived from this are meant to be used fro vehicle drawing
     */
    class GUILaneDrawer {
    public:
	    /// constructor
        GUILaneDrawer() { }

	    /// destructor
        virtual ~GUILaneDrawer() { }

	    /// called before the first vehicle is drawn
        virtual void initStep(const double &width) = 0;

	    /// draws a single vehicle; no tool-tip informations (faster)
        virtual void drawLaneNoTooltips(const GUILaneWrapper &lane,
            LaneColoringScheme scheme) = 0;

	    /// draws a single vehicle; tool-tip informations shall be generated
        virtual void drawLaneWithTooltips(const GUILaneWrapper &lane,
            LaneColoringScheme scheme) = 0;

	    /// ends the drawing of vehicles
        virtual void closeStep() = 0;
    };

protected:

void drawPolygon(const Position2DVector &v, double lineWidth, bool close);

public slots:

protected:
    void doPaintGL(int mode, double scale);

    void doInit();

    /// paints the edges
    void paintGLEdges(GUIEdgeGrid::GUIEdgeSet &edges,
        double scale);

    /// paints the vehicles
    void paintGLVehicles(GUIEdgeGrid::GUIEdgeSet &edges);

    /// draws a single vehicle
    void drawSingleGLVehicle(MSVehicle *vehicle,
        std::pair<Position2D, Position2D> &pos, double length);

    /// returns the color of the edge
    RGBColor getEdgeColor(GUIEdge *edge) const;

protected:
    /// the vehicle drawer to use
    GUIVehicleDrawer *_vehicleDrawer;

    /// the lane drawer to use
    GUILaneDrawer *_laneDrawer;

    /// the coloring scheme of vehicles to use
    VehicleColoringScheme _vehicleColScheme;

    /// the coloring scheme of lanes to use
    LaneColoringScheme _laneColScheme;

    int myTrackedID;

    bool myFontsLoaded;

    LFontRenderer myFontRenderer;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIViewTraffic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

