//---------------------------------------------------------------------------//
//                        GUI_E2_ZS_CollectorOverLanes.cpp -
//  The gui-version of the MS_E2_ZS_Collector, together with the according
//   wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Okt 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/12/04 13:31:28  dkrajzew
// detector name changes applied
//
// Revision 1.2  2003/11/18 14:27:39  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.1  2003/11/17 07:15:27  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.4  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <microsim/MSInductLoop.h>
#include <gui/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUI_E2_ZS_CollectorOverLanes.h"
#include <gui/GUIGlObjectStorage.h>
#include <guisim/GUIEdge.h>
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeomHelper.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <qgl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUI_E2_ZS_CollectorOverLanes-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_CollectorOverLanes::GUI_E2_ZS_CollectorOverLanes( std::string id,
		MSLane* lane, MSUnit::Meters startPos, //MSUnit::Meters detLength,
        //const MS_E2_ZS_CollectorOverLanes::LaneContinuations &laneContinuations,
		MSUnit::Seconds haltingTimeThreshold,
		MSUnit::MetersPerSecond haltingSpeedThreshold,
		MSUnit::Meters jamDistThreshold,
		MSUnit::Seconds deleteDataAfterSeconds)
    : MS_E2_ZS_CollectorOverLanes(id, lane, startPos, //detLength, laneContinuations,
        haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold,
        deleteDataAfterSeconds)
{
}



GUI_E2_ZS_CollectorOverLanes::~GUI_E2_ZS_CollectorOverLanes()
{
}


GUIDetectorWrapper *
GUI_E2_ZS_CollectorOverLanes::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
											GUILaneWrapper &wrapper)
{
    throw 1;
}

GUIDetectorWrapper *
GUI_E2_ZS_CollectorOverLanes::buildDetectorWrapper(GUIGlObjectStorage &idStorage)
{
    return new MyWrapper(*this, idStorage, myAlreadyBuild);
}

/*
MS_E2_ZS_Collector *
GUI_E2_ZS_CollectorOverLanes::buildCollector(size_t c, size_t r, MSLane *l,
                                             double start, double end)
{
    return new GUI_E2_ZS_Collector(makeID(l->id(), c, r),
        l, end, end, haltingTimeThresholdM,
        haltingSpeedThresholdM, jamDistThresholdM, deleteDataAfterSecondsM);
}
*/

MSE2Collector *
GUI_E2_ZS_CollectorOverLanes::buildCollector(size_t c, size_t r, MSLane *l,
                                            double start, double end)
{
    string id = makeID(l->id(), c, r);
    if(start+end<l->length()) {
        start = l->length() - end - 0.1;
    }
    return new GUI_E2_ZS_Collector(id,
        l, start, end, false, haltingTimeThresholdM,
        haltingSpeedThresholdM, jamDistThresholdM, deleteDataAfterSecondsM);
}


/* -------------------------------------------------------------------------
 * GUI_E2_ZS_CollectorOverLanes::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_CollectorOverLanes::MyWrapper::MyWrapper(
        GUI_E2_ZS_CollectorOverLanes &detector,
        GUIGlObjectStorage &idStorage,
        const LaneDetMap &detectors)
    : GUIDetectorWrapper(idStorage, string("E2OverLanes detector:")+detector.getId()),
    myDetector(detector)
{
    size_t glID = idStorage.getUniqueID();
    for(LaneDetMap::const_iterator i=detectors.begin(); i!=detectors.end(); i++) {
        MSLane *l = (*i).first;
        GUIEdge *edge =
            static_cast<GUIEdge*>(MSEdge::dictionary(l->edge().id()));
        GUILaneWrapper &w = edge->getLaneGeometry(l);
        GUI_E2_ZS_Collector *c = static_cast<GUI_E2_ZS_Collector*>((*i).second);
        GUIDetectorWrapper *dw =
            c->buildDetectorWrapper(idStorage, w, detector, glID);
        mySubWrappers.push_back(dw);
        myBoundery.add(dw->getBoundery());
    }
}


GUI_E2_ZS_CollectorOverLanes::MyWrapper::~MyWrapper()
{
}


Boundery
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getBoundery() const
{
    return myBoundery;
}


GUIParameterTableWindow *
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getParameterWindow(GUIApplicationWindow &app,
                                                   GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    myMkExistingItem(*ret, "density [?]",
        E2::DENSITY);
    myMkExistingItem(*ret, "jam lengths [veh]",
        E2::MAX_JAM_LENGTH_IN_VEHICLES);
    myMkExistingItem(*ret, "jam length [m]",
        E2::MAX_JAM_LENGTH_IN_METERS);
    myMkExistingItem(*ret, "jam len sum [veh]",
        E2::JAM_LENGTH_SUM_IN_VEHICLES);
    myMkExistingItem(*ret, "jam len sum [m]",
        E2::JAM_LENGTH_SUM_IN_METERS);
    myMkExistingItem(*ret, "queue length [veh]",
        E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES);
    myMkExistingItem(*ret, "queue length [m]",
        E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS);
    myMkExistingItem(*ret, "vehicles [veh]",
        E2::N_VEHICLES);
    myMkExistingItem(*ret, "occupancy degree [?]",
        E2::OCCUPANCY_DEGREE);
    myMkExistingItem(*ret, "space mean speed [?]",
        E2::SPACE_MEAN_SPEED);
    myMkExistingItem(*ret, "halting duration [?]",
        E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE);
    //
    ret->mkItem("length [m]", false,
        myDetector.getLength());
    /*
    ret->mkItem("position [m]", false,
        myDetector.getStartPos());*/
//    ret->mkItem("", false, myDetector.getStartLaneID());
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUI_E2_ZS_CollectorOverLanes::MyWrapper::myMkExistingItem(GUIParameterTableWindow &ret,
                                                 const std::string &name,
                                                 E2::DetType type)
{
    if(!myDetector.hasDetector(type)) {
        return;
    }
    MyValueRetriever *binding =
        new MyValueRetriever(myDetector, type, 1);
    ret.mkItem(name.c_str(), true, binding);
}


std::string
GUI_E2_ZS_CollectorOverLanes::MyWrapper::microsimID() const
{
    return myDetector.getId();
}


bool
GUI_E2_ZS_CollectorOverLanes::MyWrapper::active() const
{
    return true;
}


void
GUI_E2_ZS_CollectorOverLanes::MyWrapper::drawGL_SG(double scale) const
{
    for(std::vector<GUIDetectorWrapper*>::const_iterator i=mySubWrappers.begin(); i!=mySubWrappers.end(); i++) {
        (*i)->drawGL_SG(scale);
    }
}


void
GUI_E2_ZS_CollectorOverLanes::MyWrapper::drawGL_FG(double scale) const
{
    for(std::vector<GUIDetectorWrapper*>::const_iterator i=mySubWrappers.begin(); i!=mySubWrappers.end(); i++) {
        (*i)->drawGL_FG(scale);
    }
}


Position2D
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getPosition() const
{
    return myBoundery.getCenter();
}


GUI_E2_ZS_CollectorOverLanes &
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getLoop()
{
    return myDetector;
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUI_E2_ZS_CollectorOverLanes.icc"
//#endif

// Local Variables:
// mode:C++
// End:

