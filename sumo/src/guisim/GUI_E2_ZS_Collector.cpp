#include <microsim/MSInductLoop.h>
#include <gui/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUI_E2_ZS_Collector.h"
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeomHelper.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <microsim/logging/UIntParametrisedDblFuncBinding.h>
#include <microsim/logging/DoubleFunctionBinding.h>
#include <qgl.h>

using namespace std;

GUI_E2_ZS_Collector::GUI_E2_ZS_Collector( std::string id,
		MSLane* lane, MSUnit::Meters startPos, MSUnit::Meters detLength,
		MSUnit::Seconds haltingTimeThreshold,
		MSUnit::MetersPerSecond haltingSpeedThreshold,
		MSUnit::Meters jamDistThreshold,
		MSUnit::Seconds deleteDataAfterSeconds)
    : MS_E2_ZS_Collector(id, lane, startPos, detLength, haltingTimeThreshold,
			haltingSpeedThreshold, jamDistThreshold, deleteDataAfterSeconds)
{
}



GUI_E2_ZS_Collector::~GUI_E2_ZS_Collector()
{
}



GUIDetectorWrapper *
GUI_E2_ZS_Collector::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
											GUILaneWrapper &wrapper)
{
    return new MyWrapper(*this, idStorage, wrapper);
}



GUI_E2_ZS_Collector::MyWrapper::MyWrapper(GUI_E2_ZS_Collector &detector,
                                    GUIGlObjectStorage &idStorage,
                                    GUILaneWrapper &wrapper)
    : GUIDetectorWrapper(idStorage, string("induct loop:")+detector.getId()),
    myDetector(detector)
{
    const Position2DVector &v = wrapper.getShape();
    Line2D l(v.getBegin(), v.getEnd());
    mySGPosition = l.getPositionAtDistance(detector.getStartPos());
	Position2D endPos = l.getPositionAtDistance(detector.getEndPos());
    mySGRotation = -l.atan2DegreeAngle();
	mySGLength = GeomHelper::distance(mySGPosition, endPos);

	// build geometry
	myFullGeometry = v.getSubpart(detector.getStartPos(), detector.getEndPos());
    //
    myShapeRotations.reserve(myFullGeometry.size()-1);
    myShapeLengths.reserve(myFullGeometry.size()-1);
    for(size_t i=0; i<myFullGeometry.size()-1; i++) {
        const Position2D &f = myFullGeometry.at(i);
        const Position2D &s = myFullGeometry.at(i+1);
        myShapeLengths.push_back(GeomHelper::distance(f, s));
        myShapeRotations.push_back(atan2((s.x()-f.x()), (f.y()-s.y()))*180.0/3.14159265);
    }
	//
	myBoundery = myFullGeometry.getBoxBoundery();
	myBoundery.add(mySGPosition);
	myBoundery.add(endPos);
}



GUI_E2_ZS_Collector::MyWrapper::~MyWrapper()
{
}



Boundery
GUI_E2_ZS_Collector::MyWrapper::getBoundery() const
{
    return myBoundery;
}



GUIParameterTableWindow *
GUI_E2_ZS_Collector::MyWrapper::getParameterWindow(GUIApplicationWindow &app,
                                             GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    myMkExistingItem(*ret, "density [?]",
        MS_E2_ZS_Collector::DENSITY);
    myMkExistingItem(*ret, "jam lengths [veh]",
        MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_VEHICLES);
    myMkExistingItem(*ret, "jam length [m]",
        MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_METERS);
    myMkExistingItem(*ret, "jam len sum [veh]",
        MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_VEHICLES);
    myMkExistingItem(*ret, "jam len sum [m]",
        MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_METERS);
    myMkExistingItem(*ret, "queue length [veh]",
        MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES);
    myMkExistingItem(*ret, "queue length [m]",
        MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS);
    myMkExistingItem(*ret, "vehicles [veh]",
        MS_E2_ZS_Collector::N_VEHICLES);
    myMkExistingItem(*ret, "occupancy degree [?]",
        MS_E2_ZS_Collector::OCCUPANCY_DEGREE);
    myMkExistingItem(*ret, "space mean speed [?]",
        MS_E2_ZS_Collector::SPACE_MEAN_SPEED);
    myMkExistingItem(*ret, "halting duration [?]",
        MS_E2_ZS_Collector::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE);
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUI_E2_ZS_Collector::MyWrapper::myMkExistingItem(GUIParameterTableWindow &ret,
                                                 const std::string &name,
                                                 MS_E2_ZS_Collector::DetType type)
{
    if(!myDetector.hasDetector(type)) {
        return;
    }
    DoubleValueSource *binding =
        new ValueRetriever(myDetector, type, 1);
    ret.mkItem(name.c_str(), true, binding);
}


GUIGlObjectType
GUI_E2_ZS_Collector::MyWrapper::getType() const
{
    return GLO_DETECTOR;
}



std::string
GUI_E2_ZS_Collector::MyWrapper::microsimID() const
{
    return myDetector.getId();
}



bool
GUI_E2_ZS_Collector::MyWrapper::active() const
{
    return true;
}


void
GUI_E2_ZS_Collector::MyWrapper::drawGL_SG(double scale) const
{
	double width=2; // !!!!
    glColor3f(0, .8, .8);
    if(width>1.0) {
        glPushMatrix();
        glTranslated(mySGPosition.x(), mySGPosition.y(), 0);
        glRotated( mySGRotation, 0, 0, 1 );
        glBegin( GL_QUADS );
        glVertex2f(-1, 0);
        glVertex2f(-1, -mySGLength);
        glVertex2f(1, -mySGLength);
        glVertex2f(1, 0);
        glEnd();
        glBegin( GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, mySGLength);
        glEnd();
        glPopMatrix();
    } else {
        glPushMatrix();
        glTranslated(mySGPosition.x(), mySGPosition.y(), 0);
        glRotated( mySGRotation, 0, 0, 1 );
        glBegin( GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, -mySGLength);
        glEnd();
        glPopMatrix();
    }
}


void
GUI_E2_ZS_Collector::MyWrapper::drawGL_FG(double scale) const
{
	double width=2; // !!!!
    glColor3f(0, .8, .8);
    if(width>1.0) {
        for(size_t i=0; i<myFullGeometry.size()-1; i++) {
			GLHelper::drawBoxLine(myFullGeometry.at(i), myShapeRotations[i], myShapeLengths[i], 1.0);
        }
    } else {
        for(size_t i=0; i<myFullGeometry.size()-1; i++) {
            GLHelper::drawLine(myFullGeometry.at(i), myShapeRotations[i], myShapeLengths[i]);
        }
    }
}


Position2D
GUI_E2_ZS_Collector::MyWrapper::getPosition() const
{
    return myBoundery.getCenter();
}


GUI_E2_ZS_Collector &
GUI_E2_ZS_Collector::MyWrapper::getLoop()
{
    return myDetector;
}


