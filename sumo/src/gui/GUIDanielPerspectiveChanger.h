#ifndef GUIDanielPerspectiveChanger_h
#define GUIDanielPerspectiveChanger_h

class Position2D;
class Boundery;

#include <qnamespace.h>
#include <qevent.h>
#include "GUIPerspectiveChanger.h"

class GUIDanielPerspectiveChanger :
        public GUIPerspectiveChanger {
public:
    GUIDanielPerspectiveChanger(GUIViewTraffic &callBack);
    ~GUIDanielPerspectiveChanger();
    void mouseMoveEvent ( QMouseEvent *e );
    void mousePressEvent ( QMouseEvent *e );
    void mouseReleaseEvent ( QMouseEvent *e );
    virtual double getRotation() const;
    virtual double getXPos() const;
    virtual double getYPos() const;
    virtual double getZoom() const;
    void recenterView();
    void centerTo(const Boundery &netBoundery,
        const Position2D &pos, double radius);
    void centerTo(const Boundery &netBoundery,
        const Boundery &bound);
    int getMouseXPosition() const;
    int getMouseYPosition() const;
private:
    void move(int xdiff, int ydiff);
    void zoom(int diff);
    void rotate(int diff);
private:
    /// the current center of the view
    double _xpos, _ypos;

    /// the sizes of the window
    int _widthInPixels, _heightInPixels;

    /// the current mouse state
    Qt::ButtonState _mouseButtonState;

    /// the current mouse position
    int _mouseXPosition, _mouseYPosition;

    /// the scale of the net (the maximum size, either width or height)
    double _netScale;

    /// the current rotation
    double _rotation;

    /// the current zoom factor
    double _zoom;

};

#endif
