//---------------------------------------------------------------------------//
//                        GUIViewTraffic.cpp -
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
#include "GUITriangleVehicleDrawer.h"
#include "GUISimpleLaneDrawer.h"
#include "GUISimpleJunctionDrawer.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUIViewTraffic.h"
#include "GUIApplicationWindow.h"

#include "icons/view_traffic/colour_lane.xpm"
#include "icons/view_traffic/colour_vehicle.xpm"
#include "icons/view_traffic/show_grid.xpm"
#include "icons/view_traffic/show_tooltips.xpm"


#ifndef WIN32
#include "GUIViewTraffic.moc"
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIViewTraffic::GUIViewTraffic(GUIApplicationWindow *app,
                               GUISUMOViewParent *parent,
                               GUINet &net)
    : GUISUMOAbstractView(app, parent, net),
    _vehicleDrawer(new GUITriangleVehicleDrawer(_net.myEdgeWrapper)),
    _laneDrawer(new GUISimpleLaneDrawer(_net.myEdgeWrapper)),
    _junctionDrawer(new GUISimpleJunctionDrawer(_net.myJunctionWrapper)),
    _detectorDrawer(new GUIDetectorDrawer(_net.myDetectorWrapper)),
    _vehicleColScheme(VCS_BY_SPEED), _laneColScheme(LCS_BLACK),
    myTrackedID(-1), myFontsLoaded(false)
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


GUIViewTraffic::~GUIViewTraffic()
{
}


void
GUIViewTraffic::buildViewToolBars(GUISUMOViewParent &v)
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
GUIViewTraffic::changeVehicleColoringScheme(int index)
{
    // set the vehicle coloring scheme in dependec to
    //  the chosen item
    switch(index) {
    case 0:
        _vehicleColScheme = VCS_BY_SPEED;
        break;
    case 1:
        _vehicleColScheme = VCS_SPECIFIED;
        break;
    case 2:
        _vehicleColScheme = VCS_RANDOM1;
        break;
    case 3:
        _vehicleColScheme = VCS_RANDOM2;
        break;
    case 4:
        _vehicleColScheme = VCS_LANECHANGE1;
        break;
    case 5:
        _vehicleColScheme = VCS_LANECHANGE2;
        break;
    case 6:
        _vehicleColScheme = VCS_LANECHANGE3;
        break;
    case 7:
        _vehicleColScheme = VCS_WAITING1;
        break;
    default:
        _vehicleColScheme = VCS_BY_SPEED;
        break;
    }
}


void
GUIViewTraffic::changeLaneColoringScheme(int index)
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
GUIViewTraffic::doPaintGL(int mode, double scale)
{
    if(_showGrid) {
        paintGLGrid();
    }
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
        _useToolTips, _junctionColScheme);
    _laneDrawer->drawGLLanes(_edges2Show, _edges2ShowSize,
        _useToolTips, width, _laneColScheme);
    _detectorDrawer->drawGLDetectors(_detectors2Show, _detectors2ShowSize,
        _useToolTips, scale/*, width, _laneColScheme*/);

/*
	Position2DVector tmp;
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymin()));
	tmp.push_front(Position2D(nb.xmax(), nb.getCenter().second/2.0));
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymax()));
	tmp.push_front(Position2D(nb.xmin(), nb.getCenter().second/2.0));
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymin()));
	tmp.push_front(Position2D(nb.xmax(), nb.getCenter().second/2.0));
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymax()));

	drawPolygon(tmp, 20, true);
*/
    // draw vehicles only when they're visible
    if(scale*m2p(3)>1) {
        _vehicleDrawer->drawGLVehicles(_edges2Show, _edges2ShowSize,
            _useToolTips, _vehicleColScheme);
        //paintGLVehicles(_edges);
    }
    glPopMatrix();

    glFlush();
    if(mode==GL_RENDER) {
        swapBuffers();
    }
}


/*
void
GUIViewTraffic::drawPolygon(const Position2DVector &v, const Position2D &center, bool close)
{
	glBegin(GL_POLYGON);
	const Position2DVector::ContType &l = v.getCont();
	for(Position2DVector::ContType ::const_iterator i=l.begin(); i!=l.end(); i++) {
		const Position2D &p = *i;
		glVertex2f(p.x(), p.y());
	}
	glEnd();
}
*/


void
GUIViewTraffic::drawPolygon(const Position2DVector &v, double lineWidth, bool close)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
	glBegin(GL_TRIANGLE_STRIP);
	const Position2DVector::ContType &l = v.getCont();
    Position2DVector::ContType ::const_iterator i = l.begin();
    Position2D p1 = *i++;
    Position2D p2 = *i++;
    size_t pos = 0;
	for(; i!=l.end()-1; i++) {
        Position2D p3 = *i;
        double x1, x2, y1, y2;
        if(pos!=0) {
            double alpha1 = atan2(p1.x()-p2.x(), p1.y()-p2.y());
            double alpha2 = atan2(p2.x()-p3.x(), p2.y()-p3.y());
            x1 = p1.x()-cos(alpha1)*lineWidth+cos((alpha1+alpha2)/2.0)*lineWidth;
            y1 = p1.y()+sin(alpha1)*lineWidth-sin((alpha1+alpha2)/2.0)*lineWidth;
            x2 = p1.x()+cos(alpha1)*lineWidth-cos((alpha1+alpha2)/2.0)*lineWidth;
            y2 = p1.y()-sin(alpha1)*lineWidth+sin((alpha1+alpha2)/2.0)*lineWidth;
        } else {
            double alpha = atan2(p1.x()-p2.x(), p1.y()-p2.y());
            x1 = p1.x()-cos(alpha)*lineWidth;
            y1 = p1.y()+sin(alpha)*lineWidth;
            x2 = p1.x()+cos(alpha)*lineWidth;
            y2 = p1.y()-sin(alpha)*lineWidth;
        }
        glColor3f(
            pos==0||pos==3 ? 0 : 1,
            pos==1||pos==4 ? 0 : 1,
            pos==2||pos==5 ? 0 : 1);
        pos++;
		glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        p1 = p2;
        p2 = p3;
	}
	glEnd();
}


RGBColor
GUIViewTraffic::getEdgeColor(GUIEdge *edge) const
{
    switch(edge->getPurpose()) {
    case GUIEdge::EDGEFUNCTION_NORMAL:
        return RGBColor(0, 0, 0);
    case GUIEdge::EDGEFUNCTION_SOURCE:
        return RGBColor(0, 0.2, 0);
    case GUIEdge::EDGEFUNCTION_SINK:
        return RGBColor(0.2, 0, 0);
    default:
        break;
    }
    throw 1;
}


void
GUIViewTraffic::track(int id)
{
    myTrackedID = id;
}


void
GUIViewTraffic::doInit()
{
    if(!myFontsLoaded) {
        if(_app->myFonts.has("std")) {
            myFontRenderer.add(_app->myFonts.get("std"));
        }
        myFontsLoaded = true;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIViewTraffic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


