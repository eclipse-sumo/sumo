/****************************************************************************/
/// @file    GUIE3Collector.cpp
/// @author  Daniel Krajzewicz
/// @date    Jan 2004
/// @version $Id$
///
// The gui-version of the MSE3Collector, together with the according
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIE3Collector.h"
#include "GUIEdge.h"
#include <utils/geom/Line2D.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIE3Collector::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIE3Collector::MyWrapper::MyWrapper(GUIE3Collector &detector,
                                     GUIGlObjectStorage &idStorage)
        : GUIDetectorWrapper(idStorage, "E3 detector:"+detector.getID()),
        myDetector(detector)
{
    const CrossSectionVector &entries = detector.getEntries();
    const CrossSectionVector &exits = detector.getExits();
    CrossSectionVectorConstIt i;
    for (i=entries.begin(); i!=entries.end(); ++i) {
        SingleCrossingDefinition def = buildDefinition(*i, false);
        myBoundary.add(def.myFGPosition);
        myEntryDefinitions.push_back(def);
    }
    for (i=exits.begin(); i!=exits.end(); ++i) {
        SingleCrossingDefinition def = buildDefinition(*i, true);
        myBoundary.add(def.myFGPosition);
        myExitDefinitions.push_back(def);
    }
}


GUIE3Collector::MyWrapper::~MyWrapper()
{}


GUIE3Collector::MyWrapper::SingleCrossingDefinition
GUIE3Collector::MyWrapper::buildDefinition(const MSCrossSection &section,
        bool /*exit!!!*/)
{
    const MSLane *lane = section.laneM;
    SUMOReal pos = section.posM;
    const Position2DVector &v =
        static_cast<const GUIEdge * const>(lane->getEdge())->getLaneGeometry(static_cast<const MSLane*>(lane)).getShape();
    Line2D l(v.getBegin(), v.getEnd());

    SingleCrossingDefinition def;
    def.myFGPosition = v.positionAtLengthPosition(pos);
    def.myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    return def;
}


Boundary
GUIE3Collector::MyWrapper::getBoundary() const
{
    return myBoundary;
}


GUIParameterTableWindow *
GUIE3Collector::MyWrapper::getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 3);
    // add items
    myMkExistingItem(*ret, "mean travel time [s]", MSE3Collector::MEAN_TRAVELTIME);
    myMkExistingItem(*ret, "mean halting number [n]", MSE3Collector::MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE);
    myMkExistingItem(*ret, "vehicle number [n]", MSE3Collector::NUMBER_OF_VEHICLES);
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIE3Collector::MyWrapper::myMkExistingItem(GUIParameterTableWindow &ret,
        const std::string &name,
        MSE3Collector::Value v)
{
    /*
    if (!myDetector.hasDetector(type)) {
        return;
    }
    */
    MyValueRetriever *binding = new MyValueRetriever(myDetector, v);
    ret.mkItem(name.c_str(), true, binding);
}


const std::string &
GUIE3Collector::MyWrapper::microsimID() const
{
    return myDetector.getID();
}


bool
GUIE3Collector::MyWrapper::active() const
{
    return true;
}


void
GUIE3Collector::MyWrapper::drawGL(SUMOReal /*scale*/, SUMOReal upscale)
{
    typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;
    CrossingDefinitions::const_iterator i;
    glColor3d(0, .8, 0);
    for (i=myEntryDefinitions.begin(); i!=myEntryDefinitions.end(); i++) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, upscale);
    }
    glColor3d(.8, 0, 0);
    for (i=myExitDefinitions.begin(); i!=myExitDefinitions.end(); i++) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, upscale);
    }
}


void
GUIE3Collector::MyWrapper::drawSingleCrossing(const Position2D &pos,
        SUMOReal rot, SUMOReal upscale) const
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPushMatrix();
    glScaled(upscale, upscale, upscale);
    glTranslated(pos.x(), pos.y(), 0);
    glRotated(rot, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2d(1.7, 0);
    glVertex2d(-1.7, 0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2d(-1.7, .5);
    glVertex2d(-1.7, -.5);
    glVertex2d(1.7, -.5);
    glVertex2d(1.7, .5);
    glEnd();
    // arrows
    glTranslated(1.5, 0, 0);
    GUITexturesHelper::drawDirectionArrow(TEXTURE_LINKDIR_STRAIGHT, 1.0);
    glTranslated(-3, 0, 0);
    GUITexturesHelper::drawDirectionArrow(TEXTURE_LINKDIR_STRAIGHT, 1.0);
    glPopMatrix();
}


Position2D
GUIE3Collector::MyWrapper::getPosition() const
{
    return myBoundary.getCenter();
}


GUIE3Collector &
GUIE3Collector::MyWrapper::getDetector()
{
    return myDetector;
}


/* -------------------------------------------------------------------------
 * GUIE3Collector-methods
 * ----------------------------------------------------------------------- */
GUIE3Collector::GUIE3Collector(const std::string &id,
                               const CrossSectionVector &entries,  const CrossSectionVector &exits,
                               MSUnit::MetersPerSecond haltingSpeedThreshold)
        : MSE3Collector(id, entries,  exits, haltingSpeedThreshold)
{}


GUIE3Collector::~GUIE3Collector()
{}


const CrossSectionVector &
GUIE3Collector::getEntries() const
{
    return entriesM;
}


const CrossSectionVector &
GUIE3Collector::getExits() const
{
    return exitsM;
}



GUIDetectorWrapper *
GUIE3Collector::buildDetectorWrapper(GUIGlObjectStorage &idStorage)
{
    return new MyWrapper(*this, idStorage);
}



/****************************************************************************/

