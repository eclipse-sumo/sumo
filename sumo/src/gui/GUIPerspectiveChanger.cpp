#include <qevent.h>
#include "GUIViewTraffic.h"
#include "GUIPerspectiveChanger.h"

GUIPerspectiveChanger::GUIPerspectiveChanger(GUIViewTraffic &callBack)
    : _callback(callBack)
{
}

GUIPerspectiveChanger::~GUIPerspectiveChanger()
{
}


void 
GUIPerspectiveChanger::setZoomingFactor(double zoom)
{
    _callback._zoomingFactor += zoom;
}


void 
GUIPerspectiveChanger::setPosition(double xpos, double ypos)
{
    _callback._xpos += xpos;
    _callback._ypos += ypos;
}


void 
GUIPerspectiveChanger::setRotation(double rotation)
{
    _callback._rotation += rotation;
}

