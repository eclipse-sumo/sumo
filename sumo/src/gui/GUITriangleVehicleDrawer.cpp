#include <guisim/GUIVehicle.h>
#include <guisim/GUILane.h>
#include <glut.h>
#include "GUIViewTraffic.h"
#include "GUITriangleVehicleDrawer.h"

GUITriangleVehicleDrawer::GUITriangleVehicleDrawer()
{
}


GUITriangleVehicleDrawer::~GUITriangleVehicleDrawer()
{
}


void 
GUITriangleVehicleDrawer::initStep()
{
}


void 
GUITriangleVehicleDrawer::drawVehicle(const GUILane &lane, 
                                      const GUIVehicle &vehicle)
{
    glLineWidth (0.1);
    glMatrixMode( GL_MODELVIEW );
    const Position2D &laneEnd = lane.getBegin();
    const Position2D &laneDir = lane.getDirection();
    double posX = laneEnd.x() - laneDir.x() * vehicle.pos();
    double posY = laneEnd.y() - laneDir.y() * vehicle.pos();
    glTranslated(posX, posY, 0);
    glRotated(lane.getRotation(), 0, 0, 1);
    glBegin( GL_TRIANGLES );
    setVehicleColor(vehicle);
    glVertex2f(0, 0);
    glVertex2f(0+vehicle.length(), 0-1.25);
    glVertex2f(0+vehicle.length(), 0+1.25);
    glEnd();
    glRotated(-lane.getRotation(), 0, 0, 1);
    glTranslated(-posX, -posY, 0);
}


void 
GUITriangleVehicleDrawer::closeStep()
{
//    glEnd();
}


void 
GUITriangleVehicleDrawer::setVehicleColor(const GUIVehicle &vehicle)
{
    double speed = vehicle.speed();
    double maxSpeed = 30; // !!!
    double fact = speed / maxSpeed / 2.0;
    glColor3f(1.0-fact, 0.5, 0.5+fact);
    if(vehicle.id()=="794") {
        glColor3f(1.0, 1.0, 0);
    } else if(vehicle.id()=="720") {
        glColor3f(1.0, 0, 1.0);
    } 

}
