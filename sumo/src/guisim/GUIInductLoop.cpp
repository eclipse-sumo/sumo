#include <microsim/MSInductLoop.h>
#include <gui/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUIInductLoop.h"
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <microsim/logging/UIntParametrisedDblFuncBinding.h>
#include <microsim/logging/DoubleFunctionBinding.h>
#include <qgl.h>

using namespace std;

GUIInductLoop::GUIInductLoop(const std::string &id, MSLane* lane,
                             double position,
                             MSNet::Time deleteDataAfterSeconds)
    : MSInductLoop(id, lane, position, deleteDataAfterSeconds)
{
}



GUIInductLoop::~GUIInductLoop()
{
}




GUIDetectorWrapper *
GUIInductLoop::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
                                    GUILaneWrapper &wrapper)
{
    return new MyWrapper(*this, idStorage, wrapper, posM);
}



GUIInductLoop::MyWrapper::MyWrapper(GUIInductLoop &detector,
                                    GUIGlObjectStorage &idStorage,
                                    GUILaneWrapper &wrapper, double pos)
    : GUIDetectorWrapper(idStorage, string("induct loop:")+detector.getId()),
    myDetector(detector)
{
    const Position2DVector &v = wrapper.getShape();
    myFGPosition = v.positionAtLengthPosition(pos);
    Line2D l(v.getBegin(), v.getEnd());
    double sgPos = pos / v.length() * l.length();
    mySGPosition = l.getPositionAtDistance(sgPos);
    myBoundery.add(myFGPosition.x()+5.5, myFGPosition.y()+5.5);
    myBoundery.add(myFGPosition.x()-5.5, myFGPosition.y()-5.5);
    myBoundery.add(mySGPosition.x()+5.5, mySGPosition.y()+5.5);
    myBoundery.add(mySGPosition.x()-5.5, mySGPosition.y()-5.5);
    myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    mySGRotation = -l.atan2DegreeAngle();
//    myBegin = wrapper.getBegin();
//    myEnd = wrapper.getEnd();
}



GUIInductLoop::MyWrapper::~MyWrapper()
{
}



Boundery
GUIInductLoop::MyWrapper::getBoundery() const
{
    return myBoundery;
}



GUIParameterTableWindow *
GUIInductLoop::MyWrapper::getParameterWindow(GUIApplicationWindow &app,
                                             GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("flow [veh/h]", true,
        new UIntParametrisedDblFuncBinding<GUIInductLoop>(
            &(getLoop()), &GUIInductLoop::getFlow, 1));
    ret->mkItem("mean speed [m/s]", true,
        new UIntParametrisedDblFuncBinding<GUIInductLoop>(
            &(getLoop()), &GUIInductLoop::getMeanSpeed, 1));
    ret->mkItem("occupancy [%]", true,
        new UIntParametrisedDblFuncBinding<GUIInductLoop>(
            &(getLoop()), &GUIInductLoop::getOccupancy, 1));
    ret->mkItem("mean vehicle length [m]", true,
        new UIntParametrisedDblFuncBinding<GUIInductLoop>(
            &(getLoop()), &GUIInductLoop::getMeanVehicleLength, 1));
    ret->mkItem("empty time [s]", true,
        new DoubleFunctionBinding<GUIInductLoop>(
            &(getLoop()), &GUIInductLoop::getTimestepsSinceLastDetection));
    // close building
    ret->closeBuilding();
    return ret;
}




GUIGlObjectType
GUIInductLoop::MyWrapper::getType() const
{
    return GLO_DETECTOR;
}



std::string
GUIInductLoop::MyWrapper::microsimID() const
{
    return myDetector.getId();
}



bool
GUIInductLoop::MyWrapper::active() const
{
    return true;
}


void
GUIInductLoop::MyWrapper::drawGL_SG(double scale) const
{
    double width = 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
    glTranslated(mySGPosition.x(), mySGPosition.y(), 0);
    glRotated( mySGRotation, 0, 0, 1 );
    glBegin( GL_QUADS );
    glVertex2f(0-1.0, 2);
    glVertex2f(-1.0, -2);
    glVertex2f(1.0, -2);
    glVertex2f(1.0, 2);
    glEnd();
    glBegin( GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2f(0, 2-.1);
    glVertex2f(0, -2+.1);
    glEnd();


    // outline
    if(width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // !!!
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
    }

    // position indicator
    if(width>1) {
        glRotated( 90, 0, 0, -1 );
        glColor3f(1, 1, 1);
        glBegin( GL_LINES);
        glVertex2f(0, 1.7);
        glVertex2f(0, -1.7);
        glEnd();
    }
    glPopMatrix();
}


void
GUIInductLoop::MyWrapper::drawGL_FG(double scale) const
{
    double width = 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated( myFGRotation, 0, 0, 1 );
    glBegin( GL_QUADS );
    glVertex2f(0-1.0, 2);
    glVertex2f(-1.0, -2);
    glVertex2f(1.0, -2);
    glVertex2f(1.0, 2);
    glEnd();
    glBegin( GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2f(0, 2-.1);
    glVertex2f(0, -2+.1);
    glEnd();


    // outline
    if(width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // !!!
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
    }

    // position indicator
    if(width>1) {
        glRotated( 90, 0, 0, -1 );
        glColor3f(1, 1, 1);
        glBegin( GL_LINES);
        glVertex2f(0, 1.7);
        glVertex2f(0, -1.7);
        glEnd();
    }
    glPopMatrix();
}


Position2D
GUIInductLoop::MyWrapper::getPosition() const
{
    return myFGPosition;
}


GUIInductLoop &
GUIInductLoop::MyWrapper::getLoop()
{
    return myDetector;
}


