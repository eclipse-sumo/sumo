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
//#include <guisim/GUIEdgeGrid.h>
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

protected:

void drawPolygon(const Position2DVector &v, double lineWidth, bool close);

public slots:

protected:
    void doPaintGL(int mode, double scale);

    void doInit();

    /// returns the color of the edge
    RGBColor getEdgeColor(GUIEdge *edge) const;

protected:
    /// the vehicle drawer to use
    GUIVehicleDrawer *_vehicleDrawer;

    /// the lane drawer to use
    GUILaneDrawer *_laneDrawer;

    /// The junction drwaer to use
    GUIJunctionDrawer *_junctionDrawer;

    GUIDetectorDrawer *_detectorDrawer;

    /// the coloring scheme of vehicles to use
    VehicleColoringScheme _vehicleColScheme;

    /// the coloring scheme of lanes to use
    LaneColoringScheme _laneColScheme;

    JunctionColoringScheme _junctionColScheme;

    int myTrackedID;

    bool myFontsLoaded;

    LFontRenderer myFontRenderer;

    size_t *_edges2Show, *_junctions2Show, *_detectors2Show;
    size_t _edges2ShowSize, _junctions2ShowSize, _detectors2ShowSize;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIViewTraffic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

