#include <microsim/MSInductLoop.h>
#include <gui/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUIInductLoop.h"
#include <utils/glutils/GLHelper.h>
#include <qgl.h>

using namespace std;

#define FULL_LENGTH 4
#define HALF_LENGTH 2

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
    myPosition = wrapper.getShape().positionAtLengthPosition(pos);
    myBoundery.add(myPosition.x()+5.5, myPosition.y()+5.5);
    myBoundery.add(myPosition.x()-5.5, myPosition.y()-5.5);
    myRotation = wrapper.getRotation();
    myBegin = wrapper.getBegin();
    myEnd = wrapper.getEnd();
}



GUIInductLoop::MyWrapper::~MyWrapper()
{
}



Boundery
GUIInductLoop::MyWrapper::getBoundery() const
{
    return myBoundery;
}



QGLObjectPopupMenu *
GUIInductLoop::MyWrapper::getPopUpMenu(GUIApplicationWindow *app,
                            GUISUMOAbstractView *parent)
{
    throw 1;
}



GUIGlObjectType
GUIInductLoop::MyWrapper::getType() const
{
    return GLO_DETECTOR;
}



std::string
GUIInductLoop::MyWrapper::microsimID() const
{
    return "bla";
}



void
GUIInductLoop::MyWrapper::insertTableParameter(GUIParameterTableWindow *window,
                                    QListView *table, double *parameter,
                                    QListViewItem **vitems)
{
}


size_t
GUIInductLoop::MyWrapper::getTableParameterNo() const
{
    throw 1;
}


double
GUIInductLoop::MyWrapper::getTableParameter(size_t pos) const
{
    throw 1;
}


void
GUIInductLoop::MyWrapper::fillTableParameter(double *parameter) const
{
}



const char * const
GUIInductLoop::MyWrapper::getTableItem(size_t pos) const
{
    throw 1;
}


bool
GUIInductLoop::MyWrapper::active() const
{
    throw 1;
}


TableType
GUIInductLoop::MyWrapper::getTableType(size_t pos) const
{
    throw 1;
}

const char *
GUIInductLoop::MyWrapper::getTableBeginValue(size_t pos) const
{
    throw 1;
}


void
GUIInductLoop::MyWrapper::drawGL(double scale) const
{
    double width = 2.0 * scale;
    glColor3f(1, 1, 0);
//    if(width>1.0) {
        glPushMatrix();
        glTranslated(myPosition.x(), myPosition.y(), 0);
        glRotated( myRotation, 0, 0, 1 );
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 0);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 0);
        glEnd();
        glBegin( GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, -2);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glTranslated(myPosition.x(), myPosition.y(), 0);
        glRotated( myRotation, 0, 0, 1 );
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 0);
        glVertex2f(-1.0, 2);
        glVertex2f(1.0, 2);
        glVertex2f(1.0, 0);
        glEnd();
        glBegin( GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, 2);
        glEnd();
        glPopMatrix();
/*    } else {
        glBegin( GL_LINES);
        glVertex2f(myBegin.x(), myBegin.y());
        glVertex2f(myEnd.x(), myEnd.y());
        glEnd();
    }*/
}
