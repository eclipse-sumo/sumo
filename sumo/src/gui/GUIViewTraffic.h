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
#include <utils/qutils/NewQMutex.h>
#include <guisim/GUIEdgeGrid.h>
#include "GUIChooser.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class QGLObjectToolTip;
class MSVehicle;
class GUINet;
class QPaintEvent;
class QResizeEvent;
class GUISUMOView;
class GUIVehicle;
class GUILaneWrapper;
class GUIEdge;
class GUIPerspectiveChanger;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This class is meant to be pure virtual later;
 * It shall be the main class to inherit views of the simulation (micro-
 * or macroscopic ones) from it.
 */
class GUIViewTraffic : public QGLWidget {
    /// is a q-object
    Q_OBJECT
public:
    /// constructor
    GUIViewTraffic(GUISUMOView *parent, GUINet &net);

    /// destructor
    virtual ~GUIViewTraffic();

    /// builds the view toolbars
    void buildViewToolBars(GUISUMOView &);

    /// recenters the view
    void recenterView();

    /// centers to the chosen artifact
    void centerTo(GUIChooser::ChooseableArtifact type,
        const std::string &name);

    /// meter-to-pixels conversion method
    double m2p(double meter);

    /// pixels-to-meters conversion method
    double p2m(double pixel);

    /// Returns the information whether rotation is allowd
    bool allowRotation() const;

    /// Returns the gl-id of the object under the given coordinates
    void setTooltipPosition(size_t x, size_t y, size_t mouseX, size_t mouseY);

    void makeCurrent();

    void updateToolTip();

public slots:
    /** changes the vehicle colouring scheme to the on stored under the given
        index */
    void changeVehicleColoringScheme(int index);

    /** changes the lane colouring scheme to the on stored under the given
        index */
    void changeLaneColoringScheme(int index);

    /** toggles the drwaing of the grid */
    void toggleShowGrid();

    void toggleToolTips();

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
    /// derived from QGLWidget, this method initialises the openGL canvas
    void initializeGL();

    /// called when the canvas has been resized
    void resizeGL( int, int );

    /// performs the painting of the simulation
    void paintGL();

    /// retrieves object under mouse, displays a tooltip with name
    size_t select();

    virtual void paintEvent ( QPaintEvent * );

public slots:
    /** called when the mouse has been moved over the window
        (this causes a change of the display) */
    virtual void mouseMoveEvent ( QMouseEvent * );

    /** called when a mouse has been pressed */
    virtual void mousePressEvent ( QMouseEvent * );

    /** called when a mouse has been released */
    virtual void mouseReleaseEvent ( QMouseEvent * );

private:
    void doPaintGL(int mode, double scale);

    /// paints a grid
    void paintGLGrid();

    /// paints the edges
    void paintGLEdges(GUIEdgeGrid::GUIEdgeSet &edges,
        double scale);

    /// paints the vehicles
    void paintGLVehicles(GUIEdgeGrid::GUIEdgeSet &edges);

    /// draws a single vehicle
    void drawSingleGLVehicle(MSVehicle *vehicle,
        std::pair<Position2D, Position2D> &pos, double length);

    /** applies the changes arised from window resize or movement */
    void applyChanges(double scale,
        size_t xoff, size_t yoff);

    /// draws the legend
    void displayLegend();

    /// centers the view to the given position and size
    void centerTo(Position2D pos, double radius);

    /// centers the given boundery
    void centerTo(Boundery bound);

    /// returns the color of the edge
    RGBColor getEdgeColor(GUIEdge *edge) const;

private:
    /// the parent window
    GUISUMOView *_parent;

    /// the network used (stored here for a faster access)
    GUINet &_net;

    /// the sizes of the window
    int _widthInPixels, _heightInPixels;

    /// the scale of the net (the maximum size, either width or height)
    double _netScale;

    /// the vehicle drawer to use
    GUIVehicleDrawer *_vehicleDrawer;

    /// the lane drawer to use
    GUILaneDrawer *_laneDrawer;

    /// the coloring scheme of vehicles to use
    VehicleColoringScheme _vehicleColScheme;

    /// the coloring scheme of lanes to use
    LaneColoringScheme _laneColScheme;

    /// information whether the grid shall be displayed
    bool _showGrid;

    /// The perspective changer
    GUIPerspectiveChanger *_changer;

    /// Information whether too-tip informations shall be generated
    bool _useToolTips;

    size_t _noDrawing;

    QGLObjectToolTip *_toolTip;

    size_t _toolTipX, _toolTipY;

    size_t _mouseX, _mouseY;

    GUIEdgeGrid::GUIEdgeSet _edges;

    int _mouseHotspotX, _mouseHotspotY;

    NewQMutex _lock;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIViewTraffic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

