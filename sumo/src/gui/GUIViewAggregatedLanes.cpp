//---------------------------------------------------------------------------//
//                        GUIViewAggregatedLanes.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.19  2003/08/14 13:44:14  dkrajzew
// tls/row - drawer added
//
// Revision 1.18  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.17  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.16  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.15  2003/06/06 10:33:47  dkrajzew
// changes due to moving the popup-menus into a subfolder
//
// Revision 1.14  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.13  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.12  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged;
// additional parameter of maximum display size added
//
// Revision 1.11  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed;
// new gl-object concept implemented; comments added
//
// Revision 1.10  2003/04/07 10:15:16  dkrajzew
// glut reinserted
//
// Revision 1.9  2003/04/04 15:13:20  roessel
// Commented out #include <glut.h>
// Added #include <qcursor.h>
//
// Revision 1.8  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged;
// comments added; tootip button added
//
// Revision 1.7  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.6  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.5  2003/03/12 16:55:19  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/03/03 15:10:20  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <qgl.h>
#include <qevent.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include "QGUIToggleButton.h"
#include "QGUIImageField.h"
#include "QGLObjectToolTip.h"
#include "GUIChooser.h"
#include "GUISUMOViewParent.h"
#include "drawerimpl/GUIAggregatedLaneDrawer.h"
#include "drawerimpl/GUIROWDrawer_FG.h"
#include "drawerimpl/GUIJunctionDrawer_nT.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUIViewAggregatedLanes.h"
#include "GUIApplicationWindow.h"

#include "icons/view_traffic/colour_lane.xpm"
#include "icons/view_traffic/colour_vehicle.xpm"
#include "icons/view_traffic/show_grid.xpm"
#include "icons/view_traffic/show_tooltips.xpm"
#include "icons/view_traffic/show_geometry.xpm"


#ifndef WIN32
#include "GUIViewAggregatedLanes.moc"
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIViewAggregatedLanes::GUIViewAggregatedLanes(GUIApplicationWindow &app,
                               GUISUMOViewParent &parent,
                               GUINet &net)
    : GUISUMOAbstractView(app, parent, net),
    _laneDrawer(new GUIAggregatedLaneDrawer(_net.myEdgeWrapper)),
    _junctionDrawer(new GUIJunctionDrawer_nT(_net.myJunctionWrapper)),
    _detectorDrawer(new GUIDetectorDrawer(_net.myDetectorWrapper)),
    _rowDrawer(new GUIROWDrawer_FG(_net.myEdgeWrapper)),
    _laneColScheme(LCS_BLACK),
    myFontsLoaded(false)
{
    _edges2ShowSize = (MSEdge::dictSize()>>5) + 1;
    _edges2Show = new size_t[_edges2ShowSize];
    clearUsetable(_edges2Show, _edges2ShowSize);
    _junctions2ShowSize = (MSJunction::dictSize()>>5) + 1;
    _junctions2Show = new size_t[_junctions2ShowSize];
    clearUsetable(_junctions2Show, _junctions2ShowSize);
    _detectors2ShowSize = (net.getDetectorWrapperNo()>>5) + 1;
    _detectors2Show = new size_t[_detectors2ShowSize];
    clearUsetable(_detectors2Show, _detectors2ShowSize);
}


GUIViewAggregatedLanes::~GUIViewAggregatedLanes()
{
}


void
GUIViewAggregatedLanes::buildViewToolBars(GUISUMOViewParent &v)
{
    // build coloring tools
    QToolBar *_coloringTools = new QToolBar( &v, "view settings" );
    v.addToolBar( _coloringTools, tr( "View Settings" ), QMainWindow::Top, FALSE );
    // add vehicle coloring scheme chooser
    QPixmap icon( colour_vehicle_xpm );
    QGUIImageField *field = new QGUIImageField( icon, "Change Vehicle colouring Scheme",
        QString::null, _coloringTools, "change vehicle colouring scheme");
    QComboBox *combo = new QComboBox( _coloringTools, "vehicle coloring scheme");
    combo->insertItem(QString("by speed"));
    combo->insertItem(QString("specified"));
    combo->insertItem(QString("random#1"));
    combo->insertItem(QString("random#2"));
    combo->insertItem(QString("lanechange#1"));
    combo->insertItem(QString("lanechange#2"));
    combo->insertItem(QString("lanechange#3"));
    combo->insertItem(QString("waiting#1"));
    connect( combo, SIGNAL(highlighted(int)),
        this, SLOT(changeVehicleColoringScheme(int)) );
    _coloringTools->addSeparator();
    // add lane coloring scheme chooser
    icon = QPixmap( colour_lane_xpm );
    field = new QGUIImageField( icon, "Change Lane colouring Scheme",
        QString::null, _coloringTools, "change lane colouring scheme");
    combo = new QComboBox( _coloringTools, "lane coloring scheme");
    combo->insertItem(QString("black"));
    combo->insertItem(QString("by purpose"));
    combo->insertItem(QString("by speed"));
    connect( combo, SIGNAL(highlighted(int)),
        this, SLOT(changeLaneColoringScheme(int)) );
    _coloringTools->addSeparator();
    // add toggle button for grid on/off
    icon = QPixmap( show_grid_xpm );
    QGUIToggleButton *toggle = new QGUIToggleButton( icon, "Show Grid",
        QString::null, this, SLOT(toggleShowGrid()), _coloringTools,
        "show grid", _showGrid );
    // add toggle button for tool-tips on/off
    icon = QPixmap( show_tooltips_xpm );
    toggle = new QGUIToggleButton( icon, "Show Tool Tips",
        QString::null, this, SLOT(toggleToolTips()), _coloringTools,
        "show tool tips", _useToolTips );
}


void
GUIViewAggregatedLanes::changeLaneColoringScheme(int index)
{
    // set the lane coloring scheme in dependec to
    //  the chosen item
    switch(index) {
    case 0:
        _laneColScheme = LCS_BLACK;
        break;
    case 1:
        _laneColScheme = LCS_BY_PURPOSE;
        break;
    case 2:
        _laneColScheme = LCS_BY_SPEED;
        break;
    default:
        _laneColScheme = LCS_BLACK;
        break;
    }
    update();
}


void
GUIViewAggregatedLanes::doPaintGL(int mode, double scale)
{
    glRenderMode(mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

	const Boundery &nb = _net.getBoundery();
    double x = (nb.getCenter().first - _changer->getXPos()); // center of view
    double xoff = 50.0 / _changer->getZoom() * _netScale
        / _addScl; // offset to right
    double y = (nb.getCenter().second - _changer->getYPos()); // center of view
    double yoff = 50.0 / _changer->getZoom() * _netScale
        / _addScl; // offset to top
/*!!!    _net._edgeGrid.get(_edges, x, y, xoff, yoff);
    paintGLEdges(_edges, scale);*/

    if(myViewSettings.differ(x, y, xoff, yoff)) {
        clearUsetable(_edges2Show, _edges2ShowSize);
        clearUsetable(_junctions2Show, _junctions2ShowSize);
        _net._grid.get(GLO_LANE|GLO_JUNCTION|GLO_DETECTOR, x, y, xoff, yoff,
            _edges2Show, _junctions2Show, _detectors2Show, 0);
        myViewSettings.set(x, y, xoff, yoff);
    }
    double width = m2p(3.0) * scale;
    _junctionDrawer->drawGLJunctions(_junctions2Show, _junctions2ShowSize,
        _junctionColScheme);
    _laneDrawer->drawGLLanes(_edges2Show, _edges2ShowSize,
        width, _laneColScheme);
    _rowDrawer->drawGLROWs(_edges2Show, _edges2ShowSize,
        width);
    _detectorDrawer->drawGLDetectors(_detectors2Show, _detectors2ShowSize,
        scale/*, width, _laneColScheme*/);
    glPopMatrix();
}



RGBColor
GUIViewAggregatedLanes::getEdgeColor(GUIEdge *edge) const
{
    switch(edge->getPurpose()) {
    case GUIEdge::EDGEFUNCTION_NORMAL:
        return RGBColor(0, 0, 0);
    case GUIEdge::EDGEFUNCTION_SOURCE:
        return RGBColor(0, 0.2, 0);
    case GUIEdge::EDGEFUNCTION_SINK:
        return RGBColor(0.2, 0, 0);
    case GUIEdge::EDGEFUNCTION_INTERNAL:
        return RGBColor(0, 0, 0.2);
    default:
        break;
    }
    throw 1;
}

void
GUIViewAggregatedLanes::doInit()
{
    if(!myFontsLoaded) {
        if(_app.myFonts.has("std")) {
            myFontRenderer.add(_app.myFonts.get("std"));
        }
        myFontsLoaded = true;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIViewAggregatedLanes.icc"
//#endif

// Local Variables:
// mode:C++
// End:


