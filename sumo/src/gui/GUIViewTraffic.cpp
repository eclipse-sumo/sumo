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
#include <glut.h>
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
#include <qtooltip.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <utils/gfx/RGBColor.h>
#include "QGUIToggleButton.h"
#include "QGUIImageField.h"
#include "QGLObjectToolTip.h"
#include "GUIChooser.h"
#include "GUISUMOView.h"
#include "GUITriangleVehicleDrawer.h"
#include "GUISimpleLaneDrawer.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUIViewTraffic.h"

#include "icons/view_traffic/colour_lane.xpm"
#include "icons/view_traffic/colour_vehicle.xpm"
#include "icons/view_traffic/show_grid.xpm"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIViewTraffic::GUIViewTraffic(GUISUMOView *parent, GUINet &net)
    : QGLWidget(parent, ""), _parent(parent), _net(net),
    _vehicleDrawer(new GUITriangleVehicleDrawer()),
    _laneDrawer(new GUISimpleLaneDrawer()),
    _vehicleColScheme(VCS_BY_SPEED), _laneColScheme(LCS_BLACK),
    _showGrid(false),
    _changer(0),
    _useToolTips(false),
    _noDrawing(0),
    _mouseHotspotX(cursor().hotSpot().x()),
    _mouseHotspotY(cursor().hotSpot().y())
{
    // set window sizes
    setMinimumSize(100, 30);
    setBaseSize(800, 800);
    setMaximumSize(800, 800);
    // compute the net scale
    double nw = _net.getBoundery().getWidth();
    double nh = _net.getBoundery().getHeight();
    _netScale = (nw < nh ? nh : nw);
    // compute the center
    std::pair<double, double> center = _net.getBoundery().getCenter();
    // show the middle at the beginning
    _changer = new GUIDanielPerspectiveChanger(*this);
    _toolTip = new QGLObjectToolTip(this);
    setMouseTracking(true);
}


GUIViewTraffic::~GUIViewTraffic()
{
}


void
GUIViewTraffic::buildViewToolBars(GUISUMOView &v)
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
    icon = QPixmap( show_grid_xpm );
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
GUIViewTraffic::toggleShowGrid()
{
    _showGrid = !_showGrid;
    update();
}

void
GUIViewTraffic::toggleToolTips()
{
    _useToolTips = !_useToolTips;
    update();
}



void
GUIViewTraffic::mouseMoveEvent ( QMouseEvent *e )
{
    _changer->mouseMoveEvent(e);
}


void
GUIViewTraffic::mousePressEvent ( QMouseEvent *e )
{
    _changer->mousePressEvent(e);
}


void
GUIViewTraffic::mouseReleaseEvent ( QMouseEvent *e )
{
    _changer->mouseReleaseEvent(e);
}

void
GUIViewTraffic::initializeGL()
{
    _lock.lock();
/*    if(!isVisible()) {
        return;
    }
    // mark that drawing is in process
    _noDrawing++;
    // ...and return when drawing is already
    //  being done
    if(_noDrawing>1) {
        _noDrawing--;
//		_lock.unlock();
        return;
    }*/
	GUINet::lockAlloc();
    _widthInPixels = 800;
    _heightInPixels = 800;
    glViewport( 0, 0, _widthInPixels-1, _heightInPixels-1 );
    glClearColor( 1.0, 1.0, 1.0, 0.0 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable(GL_LINE_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _changer->otherChange();
    QToolTip::add(this, geometry(), "hallo");
	GUINet::unlockAlloc();
    _lock.unlock();
}


void
GUIViewTraffic::resizeGL( int width, int height )
{
    _lock.lock();
/*    if(!isVisible()) {
        return;
    }
    // mark that drawing is in process
    _noDrawing++;
    // ...and return when drawing is already
    //  being done
    if(_noDrawing>1) {
        _noDrawing--;
        return;
    }*/
    _changer->otherChange();
    _lock.unlock();
}


void
GUIViewTraffic::paintGL()
{
    _lock.lock();
    if(!isVisible()) {
		_lock.unlock();
        return;
    }
    // ...and return when drawing is already
    _noDrawing++;
    //  being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    // draw
    glClear(GL_COLOR_BUFFER_BIT);
    applyChanges(1.0, 0, 0);
    doPaintGL(GL_RENDER, 1.0);

    // check whether the select mode shall be
    //  done, too
    if(!_useToolTips) {
        _noDrawing--;
        glFlush();
        swapBuffers();
        _lock.unlock();
        return;
    }

    // initialise the select mode
    const int SENSITIVITY = 2;
    LaneColoringScheme tmp = _laneColScheme;
    _laneColScheme = LCS_BY_SPEED;
    double scale = double(_widthInPixels)/double(SENSITIVITY);
    applyChanges(scale, _toolTipX+_mouseHotspotX, _toolTipY+_mouseHotspotY);
    // paint in select mode
    doPaintGL(GL_SELECT, scale);//GL_SELECT, scale);
//    doPaintGL(GL_RENDER, scale);
    _laneColScheme = tmp;
    glFlush();
    swapBuffers();
    // mark end-of-drawing
    _noDrawing--;
    _lock.unlock();
}


void
GUIViewTraffic::doPaintGL(int mode, double scale)
{
    const int SENSITIVITY = 4;
    const int NB_HITS_MAX = 1000;

    // Prepare the selection mode
    static GLuint hits[NB_HITS_MAX];
    static GLint nb_hits = 0;

    if(mode==GL_SELECT) {
		GUINet::lockAlloc();
        glSelectBuffer(NB_HITS_MAX, hits);
        glInitNames();
		GUINet::unlockAlloc();
    }

    glRenderMode(mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    if(_showGrid) {
        paintGLGrid();
    }
    double x = (_net.getBoundery().getCenter().first - _changer->getXPos()); // center of view
    double xoff = 50.0 / _changer->getZoom() * _netScale; // offset to right
    double y = (_net.getBoundery().getCenter().second - _changer->getYPos()); // center of view
    double yoff = 50.0 / _changer->getZoom() * _netScale; // offset to top
	GUINet::lockAlloc();
    _net._edgeGrid.get(_edges, x, y, xoff, yoff);
	GUINet::unlockAlloc();
    paintGLEdges(_edges, scale);
    // draw vehicles only when they're visible
    if(scale*m2p(3)>1) {
        paintGLVehicles(_edges);
    }
    if(mode==GL_SELECT) {
        // Get the results
        nb_hits = glRenderMode(GL_RENDER);

        // Interpret results
        //  Vehicles should have a greater id
        //  than the previously allocated lanes
        unsigned int idMax = 0;
        for (int i=0; i<nb_hits; ++i) {
            assert (i*4+3<NB_HITS_MAX);
            if (hits[i*4+3] > idMax) {
        	    idMax = hits[i*4+3];
                assert (i*4+3<NB_HITS_MAX);
    	    }
        }
        if(idMax!=0) {
            GUINet::lockAlloc();
            GUIGlObject *object = _net._idStorage.getObjectBlocking(idMax);
            if(object!=0) {
                _toolTip->setObjectTip(object, _toolTipX, _toolTipY);
                _net._idStorage.unblockObject();
            } else {
                _toolTip->myClear();//_toolTip->clear();
            }
            GUINet::unlockAlloc();
        }
    }
    glPopMatrix();
}




size_t
GUIViewTraffic::select()
{
return 0;//!!!
}


void
GUIViewTraffic::paintGLEdges(GUIEdgeGrid::GUIEdgeSet &edges, double scale)
{
    // compute the width of lanes
    double width = m2p(3.0) * scale;
    // initialise drawing
    _laneDrawer->initStep(width);
    // check whether tool-tip information shall be generated
    if(_useToolTips) {
        // go through edges
        for( GUIEdgeGrid::GUIEdgeSet::iterator i=edges.begin(); i!=edges.end(); i++) {
            GUIEdge *edge = static_cast<GUIEdge*>(*i);
            size_t noLanes = edge->nLanes();
            // go through the current edge's lanes
            for(size_t i=0; i<noLanes; i++) {
                _laneDrawer->drawLaneWithTooltips(edge->getLaneGeometry(i),
                    _laneColScheme);
            }
        }
    } else {
        // go through edges
        for( GUIEdgeGrid::GUIEdgeSet::iterator i=edges.begin(); i!=edges.end(); i++) {
            GUIEdge *edge = static_cast<GUIEdge*>(*i);
            size_t noLanes = edge->nLanes();
            // go through the current edge's lanes
            for(size_t i=0; i<noLanes; i++) {
                _laneDrawer->drawLaneNoTooltips(edge->getLaneGeometry(i),
                    _laneColScheme);
            }
        }
    }
    _laneDrawer->closeStep();
}


void
GUIViewTraffic::paintGLGrid()
{
    glBegin( GL_LINES );
    glColor3f(0.5, 0.5, 0.5);
    double ypos = 0;
    double xpos = 0;
    double xend = (_net._edgeGrid.getNoXCells())
        * _net._edgeGrid.getXCellSize();
    double yend = (_net._edgeGrid.getNoYCells())
        * _net._edgeGrid.getYCellSize();
    for(size_t yr=0; yr<_net._edgeGrid.getNoYCells()+1; yr++) {
        glVertex2f(0, ypos);
        glVertex2f(xend, ypos);
        ypos += _net._edgeGrid.getYCellSize();
    }
    for(size_t xr=0; xr<_net._edgeGrid.getNoXCells()+1; xr++) {
        glVertex2f(xpos, 0);
        glVertex2f(xpos, yend);
        xpos += _net._edgeGrid.getXCellSize();
    }
    glEnd();
}


void
GUIViewTraffic::paintGLVehicles(GUIEdgeGrid::GUIEdgeSet &edges)
{
    _vehicleDrawer->initStep();
    glLineWidth (0.1);
    // draw the vehicles
    for(GUIEdgeGrid::GUIEdgeSet::iterator i=edges.begin(); i!=edges.end(); i++) {
        GUIEdge *edge = static_cast<GUIEdge*>((*i));
        size_t noLanes = edge->nLanes();
        for(size_t i=0; i<noLanes; i++) {
            // get the lane
            GUILaneWrapper &laneGeom = edge->getLaneGeometry(i);
            MSLane &lane = edge->getLane(i);
            // retrieve vehicles from lane; disallow simulation
            const MSLane::VehCont &vehicles = lane.getVehiclesSecure();
            /// check whether tool-tip informations shall be generated
            if(_useToolTips) {
                // go through the vehicles
                for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
                    MSVehicle *veh = *v;
                    _vehicleDrawer->drawVehicleWithTooltips(laneGeom,
                        static_cast<GUIVehicle&>(*veh), _vehicleColScheme);
                }
            } else {
                // go through the vehicles
                for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
                    MSVehicle *veh = *v;
                    _vehicleDrawer->drawVehicleNoTooltips(laneGeom,
                        static_cast<GUIVehicle&>(*veh), _vehicleColScheme);
                }
            }
            // allow lane simulation
            lane.releaseVehicles();
        }
    }
    _vehicleDrawer->closeStep();
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
GUIViewTraffic::applyChanges(double scale,
                             size_t xoff, size_t yoff)
{
    _widthInPixels = width();
    _heightInPixels = height();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // rotate first;
    //  This is quite unchecked, so it's meaning and position is quite
    //  unclear
    glRotated(_changer->getRotation(), 0, 0, 1);
    // Fit the view's size to the size of the net
    glScaled(2.0/_netScale, 2.0/_netScale, 0);
    // Apply the zoom and the scale
    double zoom = _changer->getZoom() / 100.0 * scale;
    glScaled(zoom, zoom, 0);
    // Translate to the middle of the net
    glTranslated(
        -(_net.getBoundery().getCenter().first),
        -(_net.getBoundery().getCenter().second),
        0);
    // Translate in dependence to the view position applied by the user
//    glTranslated(_changer->getXPos()/scale, _changer->getYPos()/scale, 0);
    glTranslated(_changer->getXPos(), _changer->getYPos(), 0);
    // Translate to the mouse pointer, when wished
    if(xoff!=0||yoff!=0) {
        double offScale = 1;///* _netScale / */ scale /*/ 2.0*/;
        double absX = (double(xoff)-(double(_widthInPixels)/2.0));
        double absY = (double(yoff)-(double(_heightInPixels)/2.0));
        glTranslated(
            -p2m(absX),
            p2m(absY),
            0);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // apply the widow size
    double xf = -1.0*(800.0-(double) _widthInPixels)/800.0;
    double yf = -1.0*(800.0-(double) _heightInPixels)/800.0;
    glTranslated(xf, yf, 0);
    _changer->applied();
}


void
GUIViewTraffic::displayLegend()
{
    GUINet::lockAlloc();
    size_t length = 1;
    string text = "1";
    while(true) {
        size_t pixelSize = m2p(length);
        if(pixelSize>20) {
            QPainter paint( this );
            paint.setPen( QColor(0, 0, 0) );
            size_t h = height();
            paint.drawLine( 10, h-10, 10+pixelSize, h-10 );
            paint.drawLine( 10, h-10, 10, h-15);
            paint.drawLine( 10+pixelSize, h-10, 10+pixelSize, h-15);
            paint.drawText( 10, h-10, QString("0m"));
            text = text + string("m");
            paint.drawText( 10+pixelSize, h-10, QString(text.c_str()));
	        GUINet::unlockAlloc();
            return;
        }
        length *= 10;
        text = text + string("0");
    }
	GUINet::unlockAlloc();
}


double
GUIViewTraffic::m2p(double meter)
{
    return
        (meter/_netScale * 8.0 * _changer->getZoom());
        //(length/_netScale * 800.0 * _parent->getZoomingFactor()/100.0);
}


double
GUIViewTraffic::p2m(double pixel)
{
    return
        pixel * _netScale / (8.0*_changer->getZoom());
}


void
GUIViewTraffic::recenterView()
{
    _changer->recenterView();
}


void
GUIViewTraffic::centerTo(GUIChooser::ChooseableArtifact type,
                     const std::string &name)
{
    switch(type) {
    case GUIChooser::CHOOSEABLE_ARTIFACT_JUNCTIONS:
        {
            Position2D pos = _net.getJunctionPosition(name);
            centerTo(pos, 50); // !!! the radius should be variable
        }
        break;
    case GUIChooser::CHOOSEABLE_ARTIFACT_EDGES:
        {
            Boundery bound = _net.getEdgeBoundery(name);
            bound.grow(20.0);
            centerTo(bound);
        }
        break;
    case GUIChooser::CHOOSEABLE_ARTIFACT_VEHICLES:
        {
            Position2D pos = _net.getVehiclePosition(name);
            centerTo(pos, 20); // !!! another radius?
        }
        break;
    default:
        // should not happen
        throw 1;
    }
    _changer->otherChange();
}


void
GUIViewTraffic::centerTo(Position2D pos, double radius)
{
    _changer->centerTo(_net.getBoundery(), pos, radius);
}


void
GUIViewTraffic::centerTo(Boundery bound)
{
    _changer->centerTo(_net.getBoundery(), bound);
}


bool
GUIViewTraffic::allowRotation() const
{
    return _parent->allowRotation();
}


/*
size_t
GUIViewTraffic::getObjectAt(size_t xat, size_t yat)
{
    LaneColoringScheme tmp = _laneColScheme;
    _laneColScheme = LCS_BY_SPEED;

    // Make openGL context current
    makeCurrent();

    const int SENSITIVITY = 4;
    const int NB_HITS_MAX = 1000;

    // Prepare the selection mode
    static GLuint hits[NB_HITS_MAX];
    static GLint nb_hits = 0;

    glSelectBuffer(NB_HITS_MAX, hits);
    glRenderMode(GL_RENDER);
    glInitNames();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    */
/*    // scale to check only a part of the viewport
    double xf = 1.0*(800.0-(double) _widthInPixels)/800.0;
    double yf = 1.0*(800.0-(double) _heightInPixels)/800.0;
    glTranslated(-xf, -yf, 0);
    double scale = double(_widthInPixels)/double(SENSITIVITY);
    glScaled(scale, scale, 1);
    glTranslated(xf/scale, yf/scale, 0);*/

/*
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    // recompute position on screen
    glTranslated(_changer->getXPos(), _changer->getYPos(), 0);
    double scale = double(_widthInPixels)/double(SENSITIVITY);
    glScaled(scale, scale, 1);
//    glTranslated(_changer->getXPos()/scale, _changer->getYPos()/scale, 0);
    // Render scene with objects ids
    double x = (_net.getBoundery().getCenter().first - _changer->getXPos()); // center of view
    double xoff = 50.0 / _changer->getZoom() * _netScale; // offset to right
    double y = (_net.getBoundery().getCenter().second + _changer->getYPos()); // center of view
    double yoff = 50.0 / _changer->getZoom() * _netScale; // offset to top
    GUIEdgeGrid::GUIEdgeSet &edges = _net._edgeGrid.get(x, y, xoff, yoff);
    paintGLEdges(edges, 1.0);
    // draw vehicles only when they're visible
    if(m2p(3)>1) {
        paintGLVehicles(edges);
    }
    glFlush();

    _laneColScheme = tmp;

    // Get the results
    nb_hits = glRenderMode(GL_RENDER);

    // Interpret results
    unsigned int zMin = UINT_MAX;
    size_t selected = 0;
    for (int i=0; i<nb_hits; ++i)
        {
        if (hits[i*4+1] < zMin)
	    {
	    zMin = hits[i*4+1];
	    selected = hits[i*4+3];
	    }
    }
    cout << nb_hits << " object" << ((nb_hits>1)?"s":"") << " under the cursor";
    if (selected>0)
        cout << ", selected = " << selected;
    cout << endl << flush;
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    _toolTip->setObjectTip(selected, xat, yat);
    return selected;
}
*/

void
GUIViewTraffic::setTooltipPosition(size_t x, size_t y)
{
    _toolTipX = x;
    _toolTipY = y;
}


void
GUIViewTraffic::paintEvent ( QPaintEvent *e )
{
/*    // do not paint anything when the canvas is
    //  not visible
    if(!isVisible()) {
        return;
    }
    // mark that drawing is in process
    _noDrawing++;
    // ...and return when drawing is already
    //  being done
    if(_noDrawing>1) {
        _noDrawing--;
        return;
    }
    makeCurrent();
    glDraw();*/
    QGLWidget::paintEvent(e);
    if(_parent->showLegend()) {
        displayLegend();
    }
}

void
GUIViewTraffic::makeCurrent()
{
	GUINet::lockAlloc();
    _lock.lock();
    QGLWidget::makeCurrent();
	_lock.unlock();
	GUINet::unlockAlloc();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIViewTraffic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


