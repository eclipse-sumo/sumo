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
#include <utils/gfx/RGBColor.h>
#include "GUIChooser.h"
#include "GUISUMOView.h"
#include "GUITriangleVehicleDrawer.h"
#include "GUIViewTraffic.h"

using namespace std;

GUIViewTraffic::GUIViewTraffic(GUISUMOView *parent, GUINet &net)
    : QGLWidget(parent, ""), _parent(parent), _net(net),
    _mouseButtonState(NoButton), _rotation(0), _xpos(0), _ypos(0),
    _vehicleDrawer(new GUITriangleVehicleDrawer())
{
    setMinimumSize(100, 30);
    setBaseSize(800, 600);
    setMaximumSize(800, 600);
    // compute the net scale
    double nw = _net.getBoundery().getWidth();
    double nh = _net.getBoundery().getHeight();
    _netScale = (nw < nh ? nh : nw);
    std::pair<double, double> center = _net.getBoundery().getCenter();
    setOffset(0, 0);
}

GUIViewTraffic::~GUIViewTraffic()
{
}


void
GUIViewTraffic::setOffset(double x, double y)
{
    _xpos = x;
    _ypos = y;
}


void
GUIViewTraffic::mouseMoveEvent ( QMouseEvent *e )
{
    int xdiff = _mouseXPosition - e->x();
    int ydiff = _mouseYPosition - e->y();
    switch(_mouseButtonState) {
    case LeftButton:
        move(xdiff, ydiff);
        break;
    case RightButton:
        zoom(ydiff);
        rotate(xdiff);
    }
    _mouseXPosition = e->x();
    _mouseYPosition = e->y();
}


void
GUIViewTraffic::move(int xdiff, int ydiff)
{
    _xpos -= p2m(xdiff);
    _ypos += p2m(ydiff);
    _changed = true;
    update();
}


void
GUIViewTraffic::zoom(int diff)
{
    double zoom = _parent->getZoomingFactor()
        + (double) diff / 100.0 * _parent->getZoomingFactor();
    if(zoom>0) {
        _parent->setZoomingFactor(zoom);
        _changed = true;
        update();
    }
}


void
GUIViewTraffic::rotate(int diff)
{
    if(_parent->allowRotation()) {
        _rotation += (double) diff / 10.0;
        _changed = true;
    }
}

void
GUIViewTraffic::mousePressEvent ( QMouseEvent *e )
{
    _mouseButtonState = e->button();
    _mouseXPosition = e->x();
    _mouseYPosition = e->y();
}


void
GUIViewTraffic::mouseReleaseEvent ( QMouseEvent *e )
{
    _mouseButtonState = e->button();
}



void
GUIViewTraffic::initializeGL()
{
    _widthInPixels = 800;
    _heightInPixels = 600;
    glViewport( 0, 0, _widthInPixels-1, _heightInPixels-1 );
    glClearColor( 1.0, 1.0, 1.0, 0.0 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable(GL_LINE_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    paintGL();
    _changed = true;
}


void
GUIViewTraffic::resizeGL( int width, int height )
{
    _changed = true;
}

void
GUIViewTraffic::paintGL()
{
//    cout << "update" << endl;
    if(_changed) {
        applyChanges();
    }
    glClear(GL_COLOR_BUFFER_BIT);
    paintGLEdges();
    // draw vehicles only when they're visible
    //if(m2p(3)>1) {
        paintGLVehicles();        
    //}
    swapBuffers();
}

/*
void
GUIViewTraffic::updateGL()
{
    paintGL();
}*/


void
GUIViewTraffic::paintGLEdges() 
{
    // draw the edges
    glLineWidth (m2p(3.0));
    glColor3f( 1, 0, 0 );
    glBegin( GL_LINES );
    for(MSEdge::DictType::iterator i=MSEdge::myDict.begin(); i!=MSEdge::myDict.end(); i++) {
        GUIEdge *edge = static_cast<GUIEdge*>((*i).second);
        // get the color in dependence to the type of the edge
        RGBColor c = getEdgeColor(edge);
        glColor3f( c.red(), c.green(), c.blue() );
        size_t noLanes = edge->nLanes();
        for(size_t i=0; i<noLanes; i++) {
            const GUILane &lane = edge->getLane(i);
            const Position2D &begin = lane.getBegin();
            const Position2D &end = lane.getEnd();
            glVertex2f(begin.x(), begin.y());
            glVertex2f(end.x(), end.y());
        }
    }
    glEnd();
}


void
GUIViewTraffic::paintGLVehicles() 
{
    _vehicleDrawer->initStep();
    glLineWidth (0.1);
    // draw the vehicles
    for(MSEdge::DictType::iterator i=MSEdge::myDict.begin(); i!=MSEdge::myDict.end(); i++) {
        GUIEdge *edge = static_cast<GUIEdge*>((*i).second);
        size_t noLanes = edge->nLanes();
        for(size_t i=0; i<noLanes; i++) {
            GUILane &lane = edge->getLane(i);
            const MSLane::VehCont &vehicles = lane.getVehiclesLocked();
            for(MSLane::VehCont::const_iterator v=vehicles.begin(); v!=vehicles.end(); v++) {
                MSVehicle *veh = *v;
                _vehicleDrawer->drawVehicle(lane, static_cast<GUIVehicle&>(*veh));
            }
            lane.unlockVehicles();
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
GUIViewTraffic::applyChanges()
{
    _widthInPixels = width();
    _heightInPixels = height();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glRotated(_rotation, 0, 0, 1);
    glScaled(2.0/_netScale, 2.0/_netScale, 0);
    double zoom = _parent->getZoomingFactor() / 100.0;
    glScaled(zoom, zoom, 0);
    glTranslated(
        -(_net.getBoundery().getCenter().first),
        (_net.getBoundery().getCenter().second),
        0);
    glTranslated(_xpos, _ypos, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double xf = -1.0*(800.0-(double) _widthInPixels)/800.0;
    double yf = -1.0*(600.0-(double) _heightInPixels)/600.0;
    glTranslated(xf, yf, 0);
    _changed = false;
}

void
GUIViewTraffic::displayLegend()
{
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
            return;
        }
        length *= 10;
        text = text + string("0");
    }
}


long
GUIViewTraffic::m2p(double meter) 
{
    return (size_t) 
        (meter/_netScale * 8.0 * _parent->getZoomingFactor());
        //(length/_netScale * 800.0 * _parent->getZoomingFactor()/100.0);
}

double
GUIViewTraffic::p2m(long pixel) 
{
    return
        (double) pixel * _netScale / (8.0*_parent->getZoomingFactor());
}



void
GUIViewTraffic::recenterView()
{
    _rotation = 0;
    _xpos = 0;
    _ypos = 0;
    _changed = true;
}

void
GUIViewTraffic::paintEvent ( QPaintEvent *e )
{
    QGLWidget::paintEvent(e);
    if(_parent->showLegend()) {
        displayLegend();
    }
}
/*
GUINet *
GUIViewTraffic::getNet() const
{
    return _net;
}*/


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
    _changed = true;
    update();
}


void 
GUIViewTraffic::centerTo(Position2D pos, double radius)
{
    _xpos = -(pos.x()
        - _net.getBoundery().getCenter().first);
    _ypos = pos.y()
        - _net.getBoundery().getCenter().second;
    _parent->setZoomingFactor(
        _net.getBoundery().getWidth() < _net.getBoundery().getHeight() ? 
        25.0 * _net.getBoundery().getWidth() / radius :
        25.0 * _net.getBoundery().getHeight() / radius
            );
}


void 
GUIViewTraffic::centerTo(Boundery bound)
{
    _xpos = -(bound.getCenter().first
        - _net.getBoundery().getCenter().first);
    _ypos = (+(bound.getCenter().second
        - _net.getBoundery().getCenter().second));
    _parent->setZoomingFactor(
        bound.getWidth() > bound.getHeight() ? 
        100.0 * _net.getBoundery().getWidth() / bound.getWidth() :
        100.0 * _net.getBoundery().getHeight() / bound.getHeight()
        );
}
