#ifndef GUISUMOAbstractView_h
#define GUISUMOAbstractView_h
//---------------------------------------------------------------------------//
//                        GUISUMOAbstractView.h -
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
// Revision 1.6  2003/08/15 12:19:16  dkrajzew
// legend display patched
//
// Revision 1.5  2003/08/14 13:42:43  dkrajzew
// definition of the tls/row - drawer added
//
// Revision 1.4  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.3  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.2  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.1  2003/05/20 09:25:13  dkrajzew
// new view hierarchy; some debugging done
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
#include <utils/qutils/NewQMutex.h>
#include <utils/glutils/lfontrenderer.h>
//#include <guisim/GUIEdgeGrid.h>
#include "GUIChooser.h"
#include "GUIGlObjectTypes.h"


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
class QGLObjectPopupMenu;
class GUIApplicationWindow;
class GUIJunctionWrapper;
class GUIDetectorWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUISUMOAbstractView
 * This class is meant to be pure virtual later;
 * It shall be the main class to inherit views of the simulation (micro-
 * or macroscopic ones) from it.
 */
class GUISUMOAbstractView : public QGLWidget {
    /// is a q-object
    Q_OBJECT

public:
    /// constructor
    GUISUMOAbstractView(GUIApplicationWindow &app,
        GUISUMOViewParent &parent, GUINet &net);

    /// destructor
    virtual ~GUISUMOAbstractView();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUISUMOViewParent &) = 0;

    /// recenters the view
    void recenterView();

    /// centers to the chosen artifact
    void centerTo(GUIGlObjectType type, const std::string &name);

    /// meter-to-pixels conversion method
    double m2p(double meter);

    /// pixels-to-meters conversion method
    double p2m(double pixel);

    std::pair<double, double> canvas2World(double x, double y);

    /// Returns the information whether rotation is allowd
    bool allowRotation() const;

    /// Returns the gl-id of the object under the given coordinates
    void setTooltipPosition(size_t x, size_t y, size_t mouseX, size_t mouseY);

    /// A reimplementation due to some internal reasons
    void makeCurrent();

    /// A method that updates the tooltip
    void updateToolTip();

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    /** @brief A handler for events
        We are mainly interested in double-clicks */
    bool event( QEvent *e );


public slots:
    /** toggles the drwaing of the grid */
    void toggleShowGrid();

    /// toggles whether tooltips shall be shown or not
    void toggleToolTips();

public:
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

    /**
     * JunctionColoringScheme
     * This enumeration holds the possible vehicle colouring schemes
     */
    enum JunctionColoringScheme {
	    /// colouring by vehicle speed
        VCS_BY_TYPE = 0
    };

public:
    /**
     * GUIVehicleDrawer
     * Classes derived from this are meant to be used fro vehicle drawing
     */
    class GUIVehicleDrawer {
    public:
	    /// constructor
        GUIVehicleDrawer(std::vector<GUIEdge*> &edges)
            : myEdges(edges) { }

	    /// destructor
        virtual ~GUIVehicleDrawer() { }

        virtual void drawGLVehicles(size_t *onWhich, size_t maxEdges,
            bool showToolTips,
            VehicleColoringScheme scheme) = 0;

    protected:
        std::vector<GUIEdge*> &myEdges;
    };

    /**
     * GUILaneDrawer
     * Classes derived from this are meant to be used fro vehicle drawing
     */
    class GUILaneDrawer {
    public:
	    /// constructor
        GUILaneDrawer(std::vector<GUIEdge*> &edges)
            : myEdges(edges) { }

	    /// destructor
        virtual ~GUILaneDrawer() { }

        virtual void drawGLLanes(size_t *which, size_t maxEdges,
            bool showToolTips, double width,
            LaneColoringScheme scheme) = 0;

    protected:

        std::vector<GUIEdge*> &myEdges;
    };

    /**
     * GUIROWRulesDrawer
     * Classes derived from this are meant to be used for drawing
     * of ROW-rules
     */
    class GUIROWRulesDrawer {
    public:
	    /// constructor
        GUIROWRulesDrawer(std::vector<GUIEdge*> &edges)
            : myEdges(edges) { }

	    /// destructor
        virtual ~GUIROWRulesDrawer() { }

        virtual void drawGLROWs(size_t *which, size_t maxEdges,
            bool showToolTips, double width) = 0;

    protected:

        std::vector<GUIEdge*> &myEdges;
    };

    /**
     * GUIJunctionDrawer
     * Classes derived from this are meant to be used for drawing
     * junctions
     */
    class GUIJunctionDrawer {
    public:
	    /// constructor
        GUIJunctionDrawer(std::vector<GUIJunctionWrapper*> &junctions)
            : myJunctions(junctions) { }

	    /// destructor
        virtual ~GUIJunctionDrawer() { }

        virtual void drawGLJunctions(size_t *which, size_t maxJunctions,
            bool showToolTips, JunctionColoringScheme scheme) = 0;

    protected:

        std::vector<GUIJunctionWrapper*> &myJunctions;
    };

    class GUIDetectorDrawer {
    public:
	    /// constructor
        GUIDetectorDrawer(std::vector<GUIDetectorWrapper*> &detectors)
            : myDetectors(detectors) { }

	    /// destructor
        ~GUIDetectorDrawer() { }

        void drawGLDetectors(size_t *which, size_t maxDetectors,
            bool showToolTips, double scale
            /*, JunctionColoringScheme scheme*/);

    protected:

        std::vector<GUIDetectorWrapper*> &myDetectors;
    };

    class ViewSettings {
    public:
        ViewSettings();
        ViewSettings(double x, double y,
            double xoff, double yoff);
        ~ViewSettings();
        bool differ(double x, double y, double xoff, double yoff);
        void set(double x, double y, double xoff, double yoff);
    private:
        double myX, myY, myXOff, myYOff;
    };

protected:
    /// derived from QGLWidget, this method initialises the openGL canvas
    void initializeGL();

    /// called when the canvas has been resized
    void resizeGL( int, int );

    /// performs the painting of the simulation
    void paintGL();

    virtual void paintEvent ( QPaintEvent * );

    virtual void timerEvent ( QTimerEvent * ) ;


public slots:
    /** called when the mouse has been moved over the window
        (this causes a change of the display) */
    virtual void mouseMoveEvent ( QMouseEvent * );

    /** called when a mouse has been pressed */
    virtual void mousePressEvent ( QMouseEvent * );

    /** called when a mouse has been released */
    virtual void mouseReleaseEvent ( QMouseEvent * );

protected:
    virtual void doPaintGL(int mode, double scale) = 0;

    virtual void doInit() = 0;

    /// paints a grid
    void paintGLGrid();

    /** applies the changes arised from window resize or movement */
    void applyChanges(double scale,
        size_t xoff, size_t yoff);

    /// draws the legend
    void displayLegend();

    /// centers the view to the given position and size
    void centerTo(Position2D pos, double radius);

    /// centers the given boundery
    void centerTo(Boundery bound);

    /// returns the id of the object under the cursor using GL_SELECT
    unsigned int getObjectUnderCursor();

    /// invokes the tooltip for the given object
    void showToolTipFor(unsigned int id);

    void clearUsetable(size_t *_edges2Show, size_t _edges2ShowSize);

protected:
    GUIApplicationWindow &_app;

    /// the parent window
    GUISUMOViewParent &_parent;

    /// the network used (stored here for a faster access)
    GUINet &_net;

    /// the sizes of the window
    int _widthInPixels, _heightInPixels;

    /// the scale of the net (the maximum size, either width or height)
    double _netScale;

    /// information whether the grid shall be displayed
    bool _showGrid;

    /// The perspective changer
    GUIPerspectiveChanger *_changer;

    /// Information whether too-tip informations shall be generated
    bool _useToolTips;

    /// Information how many times the drawing method was called at once
    size_t _noDrawing;

    /// The used tooltip-class
    QGLObjectToolTip *_toolTip;

    /// position to display the tooltip at
    size_t _toolTipX, _toolTipY;

    /// The current mouse position (if the mouse is over this canvas)
    size_t _mouseX, _mouseY;

    /// Offset to the mouse-hotspot from the mouse position
    int _mouseHotspotX, _mouseHotspotY;

    /// A lock for drawing operations
    NewQMutex _lock;

    /// _widthInPixels / _heightInPixels
    double _ratio;

    /// Additional scaling factor for meters-to-pixels conversion
    double _addScl;

    /// The timer id
    int _timer;

    /// The reason (mouse state) of the timer
    int _timerReason;

    /// The current popup-menu
    QGLObjectPopupMenu *_popup;

    /// the description of the viewport
    ViewSettings myViewSettings;

    /// The openGL-font drawer
    LFontRenderer myFontRenderer;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUISUMOAbstractView.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

