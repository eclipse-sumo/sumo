#ifndef GUIViewTraffic_h
#define GUIViewTraffic_h

#include <string>
#include <qgl.h>
#include <qevent.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/Position2D.h>
#include <utils/gfx/RGBColor.h>
#include "GUIChooser.h"

class MSVehicle;
class GUINet;
class QPaintEvent;
class GUISUMOView;
class GUIVehicle;
class GUILane;
class GUIEdge;

class GUIViewTraffic : public QGLWidget {
    Q_OBJECT
public:
    class GUIVehicleDrawer {
    public:
        GUIVehicleDrawer() { }
        virtual ~GUIVehicleDrawer() { } 
        virtual void initStep() = 0;
        virtual void drawVehicle(const GUILane &lane, 
            const GUIVehicle &veh) = 0;
        virtual void closeStep() = 0;
    };

    GUIViewTraffic(GUISUMOView *parent, GUINet &net);
    virtual ~GUIViewTraffic();
    void setOffset(double x, double y);
    void recenterView();
    void centerTo(GUIChooser::ChooseableArtifact type, 
        const std::string &name);
    long m2p(double meter);
    double p2m(long pixel);
protected:
    void initializeGL();
    void resizeGL( int, int );
    void paintGL();
protected:
    virtual void paintEvent ( QPaintEvent * ) ;
    //void updateGL();
//    virtual void paintEvent ( QPaintEvent * );
public slots:
    //virtual void updateGL();
    virtual void mouseMoveEvent ( QMouseEvent * );
    virtual void mousePressEvent ( QMouseEvent * );
    virtual void mouseReleaseEvent ( QMouseEvent * );
//    virtual void update(int x, int y, int w, int h );

private:
    void paintGLEdges();
    void paintGLVehicles();
    void drawSingleGLVehicle(MSVehicle *vehicle, 
        std::pair<Position2D, Position2D> &pos, double length);
    void applyChanges();
    void move(int xdiff, int ydiff);
    void zoom(int diff);
    void rotate(int diff);
    void displayLegend();
    void centerTo(Position2D pos, double radius);
    void centerTo(Boundery bound);
    RGBColor getEdgeColor(GUIEdge *edge) const;

    GUISUMOView *_parent;
    GUINet &_net;
    double _xpos, _ypos;
    int _widthInPixels, _heightInPixels;
    ButtonState _mouseButtonState;
    int _mouseXPosition, _mouseYPosition;
    double _netScale;
    bool _changed;
    double _rotation;
    GUIVehicleDrawer *_vehicleDrawer;
};

#endif
