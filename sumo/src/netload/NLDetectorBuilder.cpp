/***************************************************************************
                          NLDetectorBuilder.cpp
                          A building helper for the detectors
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.14  2004/01/12 14:46:21  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.13  2004/01/12 14:37:32  dkrajzew
// reading of e2-detectors from files added
//
// Revision 1.12  2003/11/11 08:05:45  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.11  2003/09/22 12:29:36  dkrajzew
// construction using two sample intervals inserted (using one by now)
//
// Revision 1.10  2003/08/14 13:52:34  dkrajzew
// new building scheme applied
//
// Revision 1.9  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.8  2003/07/22 15:12:16  dkrajzew
// new usage of detectors applied
//
// Revision 1.7  2003/07/21 18:07:44  roessel
// Adaptions due to new MSInductLoop.
//
// Revision 1.6  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.5  2003/03/18 15:00:32  roessel
// Changed Loggedvalue to LoggedValue in #includes
//
// Revision 1.4  2003/03/17 14:24:30  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:06:33  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.8  2002/07/31 17:34:50  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.9  2002/07/26 10:49:41  dkrajzew
// Detector-output destination may now be specified using relative pathnames
//
// Revision 1.8  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.7  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.6  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.5  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting
// methods debugged
//
// Revision 1.4  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.3  2002/04/16 06:50:20  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/MSInductLoop.h>
#include <microsim/MSE2Collector.h>
#include <microsim/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/MSDetector2File.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include "NLDetectorBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLDetectorBuilder::NLDetectorBuilder()
{
}


NLDetectorBuilder::~NLDetectorBuilder()
{
}


void
NLDetectorBuilder::buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &/*style*/, std::string filename,
        const std::string &basePath)
{
     // get the output style
//   MSDetector::OutputStyle cstyle = convertStyle(id, style);
     // check whether the file must be converted into a relative path
     if(!FileHelpers::isAbsolute(filename)) {
         filename = FileHelpers::getConfigurationRelative(basePath, filename);
     }
    // get and check the lane
    MSLane *clane = MSLane::dictionary(lane);
    if(clane==0) {
        throw InvalidArgument(
            string("On detector building:\n")
            + string("The lane with the id '") + lane
            + string("' is not known."));
    }
    // compute position
    if(pos<0) {
        pos = clane->length() + pos;
    }
    // build the loop
    MSInductLoop *loop = createInductLoop(id, clane, pos);
    // add the file output
    MSDetector2File* det2file =
        MSDetector2File::getInstance();
    det2file->addDetectorAndInterval(loop, filename, splInterval, splInterval);
}


void
NLDetectorBuilder::buildE2Detector(const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, int splInterval,
        const std::string &/*style*/, std::string filename,
        const std::string &basePath, const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds )
{
     // get the output style
//   MSDetector::OutputStyle cstyle = convertStyle(id, style);
     // check whether the file must be converted into a relative path
     if(!FileHelpers::isAbsolute(filename)) {
         filename = FileHelpers::getConfigurationRelative(basePath, filename);
     }
    // get and check the lane
    MSLane *clane = MSLane::dictionary(lane);
    if(clane==0) {
        throw InvalidArgument(
            string("On detector building:\n")
            + string("The lane with the id '") + lane
            + string("' is not known."));
    }
    // compute position
    if(pos<0) {
        pos = clane->length() + pos;
    }
    // patch position
    if(pos<0.1) {
        pos = 0.1;
    }
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if(!cont) {
        // patch length
        if(pos+length>clane->length()-0.1) {
            length = clane->length() - 0.1 - pos;
        }
        det = buildSingleLaneE2Det(id, clane, pos, length, splInterval,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    } else {
        det = buildMultiLaneE2Det(id, clane, pos, length, splInterval,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    }
    // add the file output
    MSDetector2File* det2file =
        MSDetector2File::getInstance();
    det2file->addDetectorAndInterval(det, filename,
        splInterval, splInterval);
}


MSDetectorFileOutput *
NLDetectorBuilder::buildSingleLaneE2Det(const std::string &id,
                                        MSLane *lane, float pos, float length,
                                        int splInterval,
                                        MSUnit::Seconds haltingTimeThreshold,
                                        MSUnit::MetersPerSecond haltingSpeedThreshold,
                                        MSUnit::Meters jamDistThreshold,
                                        MSUnit::Seconds deleteDataAfterSeconds,
                                        const std::string &measures)
{
    MSE2Collector *ret = createSingleLaneE2Detector(id, lane, pos, length,
        haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);
    E2MeasuresVector toAdd = parseE2Measures(measures);
    for(E2MeasuresVector::iterator i=toAdd.begin(); i!=toAdd.end(); i++) {
        ret->addDetector(*i);
    }
    return ret;
}


MSDetectorFileOutput *
NLDetectorBuilder::buildMultiLaneE2Det(const std::string &id,
                                       MSLane *lane, float pos, float length,
                                       int splInterval,
                                       MSUnit::Seconds haltingTimeThreshold,
                                       MSUnit::MetersPerSecond haltingSpeedThreshold,
                                       MSUnit::Meters jamDistThreshold ,
                                       MSUnit::Seconds deleteDataAfterSeconds,
                                       const std::string &measures)
{
    MS_E2_ZS_CollectorOverLanes *ret = createMultiLaneE2Detector(id, lane, pos,
        haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);
    ret->init(lane, length, MS_E2_ZS_CollectorOverLanes::LaneContinuations());
    E2MeasuresVector toAdd = parseE2Measures(measures);
    for(E2MeasuresVector::iterator i=toAdd.begin(); i!=toAdd.end(); i++) {
        ret->addDetector(*i);
    }
    return ret;
}




NLDetectorBuilder::E2MeasuresVector
NLDetectorBuilder::parseE2Measures(const std::string &measures)
{
    string my = measures;
    StringUtils::upper(my);
    E2MeasuresVector ret;
    if(measures.find("DENSITY")!=string::npos) {
        ret.push_back(E2::DENSITY);
    }
    if(measures.find("MAX_JAM_LENGTH_IN_VEHICLES")!=string::npos) {
        ret.push_back(E2::MAX_JAM_LENGTH_IN_VEHICLES);
    }
    if(measures.find("MAX_JAM_LENGTH_IN_METERS")!=string::npos) {
        ret.push_back(E2::MAX_JAM_LENGTH_IN_METERS);
    }
    if(measures.find("JAM_LENGTH_SUM_IN_VEHICLES")!=string::npos) {
        ret.push_back(E2::JAM_LENGTH_SUM_IN_VEHICLES);
    }
    if(measures.find("JAM_LENGTH_SUM_IN_METERS")!=string::npos) {
        ret.push_back(E2::JAM_LENGTH_SUM_IN_METERS);
    }
    if(measures.find("QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES")!=string::npos) {
        ret.push_back(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES);
    }
    if(measures.find("QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS")!=string::npos) {
        ret.push_back(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS);
    }
    if(measures.find("N_VEHICLES")!=string::npos) {
        ret.push_back(E2::N_VEHICLES);
    }
    if(measures.find("OCCUPANCY_DEGREE")!=string::npos) {
        ret.push_back(E2::OCCUPANCY_DEGREE);
    }
    if(measures.find("SPACE_MEAN_SPEED")!=string::npos) {
        ret.push_back(E2::SPACE_MEAN_SPEED);
    }
    if(measures.find("CURRENT_HALTING_DURATION_SUM_PER_VEHICLE")!=string::npos) {
        ret.push_back(E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE);
    }
    if(measures.find("N_STARTED_HALTS")!=string::npos) {
        ret.push_back(E2::N_STARTED_HALTS);
    }
    if(measures.find("HALTING_DURATION_SUM")!=string::npos) {
        ret.push_back(E2::HALTING_DURATION_SUM);
    }
    if(measures.find("HALTING_DURATION_MEAN")!=string::npos) {
        ret.push_back(E2::HALTING_DURATION_MEAN);
    }
    if(measures.find("APPROACHING_VEHICLES_STATES")!=string::npos) {
        ret.push_back(E2::APPROACHING_VEHICLES_STATES);
    }
    if(measures.find("ALL")!=string::npos) {
        ret.push_back(E2::ALL);
    }
    return ret;
}


MSInductLoop *
NLDetectorBuilder::createInductLoop(const std::string &id,
                                    MSLane *lane, double pos)
{
    return new MSInductLoop(id, lane, pos);
}


MSE2Collector *
NLDetectorBuilder::createSingleLaneE2Detector(const std::string &id,
                                              MSLane *lane, float pos,
                                              float length,
                                              MSUnit::Seconds haltingTimeThreshold,
                                              MSUnit::MetersPerSecond haltingSpeedThreshold,
                                              MSUnit::Meters jamDistThreshold,
                                              MSUnit::Seconds deleteDataAfterSeconds)
{
    return new MSE2Collector(id, lane, pos, length,
        haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);

}


MS_E2_ZS_CollectorOverLanes *
NLDetectorBuilder::createMultiLaneE2Detector(const std::string &id,
                                              MSLane *lane, float pos,
                                              MSUnit::Seconds haltingTimeThreshold,
                                              MSUnit::MetersPerSecond haltingSpeedThreshold,
                                              MSUnit::Meters jamDistThreshold,
                                              MSUnit::Seconds deleteDataAfterSeconds)
{
    return new MS_E2_ZS_CollectorOverLanes( id, lane, pos,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds);
}


            /*
MSDetector::OutputStyle NLDetectorBuilder::convertStyle(const std::string &id,
        const std::string &style)
{
     if(style=="GNUPLOT" || style=="GPLOT")
         return MSDetector::GNUPLOT;
     if(style=="CSV")
         return MSDetector::CSV;
     throw InvalidArgument("Unknown output style '" + style + "' while parsing the detector '" + id + "' occured.");
}
*/

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


