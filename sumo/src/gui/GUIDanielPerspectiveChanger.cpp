#include <iostream> // !!! debug only

#include <qevent.h>
#include <qnamespace.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/Position2D.h>
#include "GUIViewTraffic.h"
#include "GUIPerspectiveChanger.h"
#include "GUIDanielPerspectiveChanger.h"


using namespace std;

GUIDanielPerspectiveChanger::GUIDanielPerspectiveChanger(GUIViewTraffic &callBack)
    : GUIPerspectiveChanger(callBack),
    _mouseButtonState(Qt::NoButton), _rotation(0), _xpos(0), _ypos(0), _zoom(100)
{
}


GUIDanielPerspectiveChanger::~GUIDanielPerspectiveChanger()
{
}


void
GUIDanielPerspectiveChanger::mouseMoveEvent ( QMouseEvent *e )
{
    _callback.setTooltipPosition(e->x(), e->y());
    int xdiff = _mouseXPosition - e->x();
    int ydiff = _mouseYPosition - e->y();
    switch(_mouseButtonState) {
    case Qt::LeftButton:
        move(xdiff, ydiff);
        break;
    case Qt::RightButton:
        zoom(ydiff);
        rotate(xdiff);
    }
    _mouseXPosition = e->x();
    _mouseYPosition = e->y();
}


void
GUIDanielPerspectiveChanger::move(int xdiff, int ydiff)
{
    _xpos -= _callback.p2m(xdiff);
    _ypos += _callback.p2m(ydiff);
    _changed = true;
    _callback.update();
}


void
GUIDanielPerspectiveChanger::zoom(int diff)
{
    double zoom = _zoom
        + (double) diff / 100.0 * _zoom;
    if(zoom>0.01&&zoom<10000000.0) {
        _zoom = zoom;
        _changed = true;
        _callback.update();
    }
}


void
GUIDanielPerspectiveChanger::rotate(int diff)
{
    if(_callback.allowRotation()) {
        _rotation += (double) diff / 10.0;
        _changed = true;
        _callback.update();
    }
}

void
GUIDanielPerspectiveChanger::mousePressEvent ( QMouseEvent *e )
{
    _mouseButtonState = e->button();
    _mouseXPosition = e->x();
    _mouseYPosition = e->y();
}


void
GUIDanielPerspectiveChanger::mouseReleaseEvent ( QMouseEvent *e )
{
    _mouseButtonState = Qt::NoButton;
}


double
GUIDanielPerspectiveChanger::getRotation() const
{
    return _rotation;
}


double
GUIDanielPerspectiveChanger::getXPos() const
{
    return _xpos;
}


double
GUIDanielPerspectiveChanger::getYPos() const
{
    return _ypos;
}


double
GUIDanielPerspectiveChanger::getZoom() const
{
    return _zoom;
}


void
GUIDanielPerspectiveChanger::recenterView()
{
    _rotation = 0;
    _xpos = 0;
    _ypos = 0;
    _zoom = 100;
    _changed = true;
}



void
GUIDanielPerspectiveChanger::centerTo(const Boundery &netBoundery,
                                      const Position2D &pos, double radius)
{
    _xpos = -(pos.x() - netBoundery.getCenter().first);
    _ypos = pos.y() - netBoundery.getCenter().second;
    _zoom =
        netBoundery.getWidth() < netBoundery.getHeight() ?
        25.0 * netBoundery.getWidth() / radius :
        25.0 * netBoundery.getHeight() / radius;
    _changed = true;
}


void
GUIDanielPerspectiveChanger::centerTo(const Boundery &netBoundery,
                                      const Boundery &bound)
{
    _xpos = -(bound.getCenter().first
        - netBoundery.getCenter().first);
    _ypos = (+(bound.getCenter().second
        - netBoundery.getCenter().second));
    _zoom =
        bound.getWidth() > bound.getHeight() ?
        100.0 * netBoundery.getWidth() / bound.getWidth() :
        100.0 * netBoundery.getHeight() / bound.getHeight();
    _changed = true;
}



int
GUIDanielPerspectiveChanger::getMouseXPosition() const
{
    return _mouseXPosition;
}


int
GUIDanielPerspectiveChanger::getMouseYPosition() const
{
    return _mouseYPosition;
}


