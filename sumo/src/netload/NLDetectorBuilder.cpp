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
// Revision 1.21  2004/04/02 11:23:51  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles all simulation-wide output
//
// Revision 1.20  2004/02/18 05:32:51  dkrajzew
// missing pass of lane continuation to detector builder added
//
// Revision 1.19  2004/02/16 13:49:08  dkrajzew
// loading of e2-link-dependent detectors added
//
// Revision 1.18  2004/02/06 09:02:39  dkrajzew
// false detector positioning when negative values are used debugged
//
// Revision 1.17  2004/01/26 11:06:54  dkrajzew
// position setting reapplied
//
// Revision 1.16  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.15  2004/01/13 14:28:46  dkrajzew
// added alternative detector description; debugging
//
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
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition
//  parseable in one step
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
#include <microsim/actions/Command_SaveTLCoupledDet.h>
#include <microsim/actions/Command_SaveTLCoupledLaneDet.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>
#include "NLDetectorBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * NLDetectorBuilder::E3DetectorDefinition-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::E3DetectorDefinition::E3DetectorDefinition(
        const std::string &id,
        const std::string &filename,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds,
        const E3MeasuresVector &measures,
        int splInterval) :
    myID(id), myFileName(filename),
    myHaltingTimeThreshold(haltingTimeThreshold),
    myHaltingSpeedThreshold(haltingSpeedThreshold),
    myDeleteDataAfterSeconds(deleteDataAfterSeconds),
    myMeasures(measures), mySampleInterval(splInterval)
{
}


NLDetectorBuilder::E3DetectorDefinition::~E3DetectorDefinition()
{
}


/* -------------------------------------------------------------------------
 * NLDetectorBuilder-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::NLDetectorBuilder()
{
}


NLDetectorBuilder::~NLDetectorBuilder()
{
}


void
NLDetectorBuilder::buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &/*style*/, std::string filename)
{
     // get the output style
//   MSDetector::OutputStyle cstyle = convertStyle(id, style);
     // check whether the file must be converted into a relative path
    // get and check the lane
    MSLane *clane = getLaneChecking(lane);
    // compute position
    if(pos<0) {
        pos = clane->length() + pos;
    }
    if(pos>clane->length()) {
        throw InvalidArgument("The position lies beyond the lane's length.");
    }
    // build the loop
    MSInductLoop *loop = createInductLoop(id, clane, pos);
    // add the file output
    MSDetector2File* det2file =
        MSDetector2File::getInstance();
    det2file->addDetectorAndInterval(loop, filename, splInterval, splInterval);
}


void
NLDetectorBuilder::buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, int splInterval,
        const std::string &style, std::string filename,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds )
{
    MSLane *clane = getLaneChecking(lane);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if(!cont) {
        convUncontE2PosLength(id, clane, pos, length);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
            clane, pos, length,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    } else {
        convContE2PosLength(id, clane, pos, length);
        det = buildMultiLaneE2Det(laneConts, id, DU_USER_DEFINED,
            clane, pos, length,
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


void
NLDetectorBuilder::buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, MSTrafficLightLogic *tll,
        const std::string &style, std::string filename,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds )
{
    MSLane *clane = getLaneChecking(lane);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if(!cont) {
        convUncontE2PosLength(id, clane, pos, length);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
            clane, pos, length,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    } else {
        convContE2PosLength(id, clane, pos, length);
        det = buildMultiLaneE2Det(laneConts, id, DU_USER_DEFINED,
            clane, pos, length,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    }
    // add the file output
    new Command_SaveTLCoupledDet(tll, det,
        MSNet::getInstance()->getCurrentTimeStep(), filename);
}


void
NLDetectorBuilder::buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, MSTrafficLightLogic *tll,
        const std::string &tolane,
        const std::string &style, std::string filename,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds )
{
    MSLane *clane = getLaneChecking(lane);
    MSLane *ctoLane = getLaneChecking(tolane);
    MSLink *link = MSLinkContHelper::getConnectingLink(*clane, *ctoLane);
    if(link==0) {
        throw InvalidArgument(
            string("The detector output can not be build as no connection between lanes '")
            + lane + string("' and '") + tolane + string("' exists."));
    }
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if(!cont) {
        convUncontE2PosLength(id, clane, pos, length);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
            clane, pos, length,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    } else {
        convContE2PosLength(id, clane, pos, length);
        det = buildMultiLaneE2Det(laneConts, id, DU_USER_DEFINED,
            clane, pos, length,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds,
            measures);
    }
    // add the file output
    new Command_SaveTLCoupledLaneDet(tll, det,
        MSNet::getInstance()->getCurrentTimeStep(), filename, link);
}


void
NLDetectorBuilder::convUncontE2PosLength(const std::string &id, MSLane *clane,
                                         float &pos, float &length)
{
    if(pos<0) {
        pos = clane->length() + pos;
    }
    // compute length
    if(length<0) {
        pos = pos + length;
        length *= -1;
    }
    // patch position
    if(pos<0.1) {
        pos = 0.1;
    }
    // patch length
    if(pos+length>clane->length()-0.1) {
        length = clane->length() - 0.1 - pos;
    }
    if(length<=0) {
        throw InvalidArgument(
            string("The length of detector '")
            + id + string("' is not positive."));
    }
}


void
NLDetectorBuilder::convContE2PosLength(const std::string &id, MSLane *clane,
                                       float &pos, float &length)
{
    if(pos<0) {
        pos *= -1.0;//clane->length() + pos;
    }
    if(length<=0) {
        throw InvalidArgument("The length of the continuated detector " + id + " is not positive.");
    }
}


void
NLDetectorBuilder::beginE3Detector(const std::string &id,
        std::string filename, int splInterval,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    E3MeasuresVector toAdd = parseE3Measures(measures);
    myE3Definition = new E3DetectorDefinition(id, filename,
        haltingSpeedThreshold, haltingSpeedThreshold, deleteDataAfterSeconds,
        toAdd, splInterval);
}


void
NLDetectorBuilder::addE3Entry(const std::string &lane, float pos)
{
    MSLane *clane = getLaneChecking(lane);
    if(myE3Definition==0) {
        throw InvalidArgument("Something is wrong with a detector description.");
    }
    if(pos<0) {
        pos = clane->length() + pos;
    }
    myE3Definition->myEntries.push_back(MSCrossSection(clane, pos));
}


void
NLDetectorBuilder::addE3Exit(const std::string &lane, float pos)
{
    MSLane *clane = getLaneChecking(lane);
    if(myE3Definition==0) {
        throw InvalidArgument("Something is wrong with a detector description.");
    }
    if(pos<0) {
        pos = clane->length() + pos;
    }
    myE3Definition->myExits.push_back(MSCrossSection(clane, pos));
}



void
NLDetectorBuilder::endE3Detector()
{
    if(myE3Definition==0) {
        throw InvalidArgument("Something is wrong with a detector description.");
    }
    MSE3Collector *det = createE3Detector(
        myE3Definition->myID,
        myE3Definition->myEntries,
        myE3Definition->myExits,
        myE3Definition->myHaltingTimeThreshold,
        myE3Definition->myHaltingSpeedThreshold,
        myE3Definition->myDeleteDataAfterSeconds);
    E3MeasuresVector &toAdd = myE3Definition->myMeasures;
    for(E3MeasuresVector::iterator i=toAdd.begin(); i!=toAdd.end(); i++) {
        det->addDetector(*i);
    }
    // add the file output
    MSDetector2File* det2file = MSDetector2File::getInstance();
    det2file->addDetectorAndInterval(det,
        myE3Definition->myFileName,
        myE3Definition->mySampleInterval,
        myE3Definition->mySampleInterval);
    // clean up
    delete myE3Definition;
    myE3Definition = 0;
}


MSE2Collector *
NLDetectorBuilder::buildSingleLaneE2Det(const std::string &id,
                                        DetectorUsage usage,
                                        MSLane *lane, float pos, float length,
                                        MSUnit::Seconds haltingTimeThreshold,
                                        MSUnit::MetersPerSecond haltingSpeedThreshold,
                                        MSUnit::Meters jamDistThreshold,
                                        MSUnit::Seconds deleteDataAfterSeconds,
                                        const std::string &measures)
{
    MSE2Collector *ret = createSingleLaneE2Detector(id, usage, lane, pos,
        length, haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);
    E2MeasuresVector toAdd = parseE2Measures(measures);
    for(E2MeasuresVector::iterator i=toAdd.begin(); i!=toAdd.end(); i++) {
        ret->addDetector(*i);
    }
    return ret;
}


MS_E2_ZS_CollectorOverLanes *
NLDetectorBuilder::buildMultiLaneE2Det(const SSVMap &laneConts,
                                       const std::string &id,
                                       DetectorUsage usage,
                                       MSLane *lane, float pos, float length,
                                       MSUnit::Seconds haltingTimeThreshold,
                                       MSUnit::MetersPerSecond haltingSpeedThreshold,
                                       MSUnit::Meters jamDistThreshold ,
                                       MSUnit::Seconds deleteDataAfterSeconds,
                                       const std::string &measures)
{
    MS_E2_ZS_CollectorOverLanes *ret = createMultiLaneE2Detector(id, usage,
        lane, pos, haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);
    ret->init(lane, length, laneConts);
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
    if(my.find("DENSITY")!=string::npos) {
        ret.push_back(E2::DENSITY);
    }
    if(my.find("MAX_JAM_LENGTH_IN_VEHICLES")!=string::npos) {
        ret.push_back(E2::MAX_JAM_LENGTH_IN_VEHICLES);
    }
    if(my.find("MAX_JAM_LENGTH_IN_METERS")!=string::npos) {
        ret.push_back(E2::MAX_JAM_LENGTH_IN_METERS);
    }
    if(my.find("JAM_LENGTH_SUM_IN_VEHICLES")!=string::npos) {
        ret.push_back(E2::JAM_LENGTH_SUM_IN_VEHICLES);
    }
    if(my.find("JAM_LENGTH_SUM_IN_METERS")!=string::npos) {
        ret.push_back(E2::JAM_LENGTH_SUM_IN_METERS);
    }
    if(my.find("QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES")!=string::npos) {
        ret.push_back(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES);
    }
    if(my.find("QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS")!=string::npos) {
        ret.push_back(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS);
    }
    if(my.find("N_VEHICLES")!=string::npos) {
        ret.push_back(E2::N_VEHICLES);
    }
    if(my.find("OCCUPANCY_DEGREE")!=string::npos) {
        ret.push_back(E2::OCCUPANCY_DEGREE);
    }
    if(my.find("SPACE_MEAN_SPEED")!=string::npos) {
        ret.push_back(E2::SPACE_MEAN_SPEED);
    }
    if(my.find("CURRENT_HALTING_DURATION_SUM_PER_VEHICLE")!=string::npos) {
        ret.push_back(E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE);
    }
    if(my.find("N_STARTED_HALTS")!=string::npos) {
        ret.push_back(E2::N_STARTED_HALTS);
    }
    if(my.find("HALTING_DURATION_SUM")!=string::npos) {
        ret.push_back(E2::HALTING_DURATION_SUM);
    }
    if(my.find("HALTING_DURATION_MEAN")!=string::npos) {
        ret.push_back(E2::HALTING_DURATION_MEAN);
    }
    if(my.find("APPROACHING_VEHICLES_STATES")!=string::npos) {
        ret.push_back(E2::APPROACHING_VEHICLES_STATES);
    }
    if(my.find("ALL")!=string::npos) {
        ret.push_back(E2::ALL);
    }
    return ret;
}



NLDetectorBuilder::E3MeasuresVector
NLDetectorBuilder::parseE3Measures(const std::string &measures)
{
    string my = measures;
    StringUtils::upper(my);
    E3MeasuresVector ret;
    if(my.find("MEAN_TRAVELTIME")!=string::npos) {
        ret.push_back(E3::MEAN_TRAVELTIME);
    }
    if(my.find("MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE")!=string::npos) {
        ret.push_back(E3::MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE);
    }
    if(my.find("NUMBER_OF_VEHICLES")!=string::npos) {
        ret.push_back(E3::NUMBER_OF_VEHICLES);
    }
    if(my.find("ALL")!=string::npos) {
        ret.push_back(E3::ALL);
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
        DetectorUsage usage, MSLane *lane, float pos, float length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    return new MSE2Collector(id, usage, lane, pos, length,
        haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);

}


MS_E2_ZS_CollectorOverLanes *
NLDetectorBuilder::createMultiLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, float pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    return new MS_E2_ZS_CollectorOverLanes( id, usage, lane, pos,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds);
}


MSE3Collector *
NLDetectorBuilder::createE3Detector(const std::string &id,
        const Detector::CrossSections &entries,
        const Detector::CrossSections &exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    return new MSE3Collector( id, entries, exits,
        haltingTimeThreshold, haltingSpeedThreshold, deleteDataAfterSeconds);
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


MSLane *
NLDetectorBuilder::getLaneChecking(const std::string &id)
{
    // get and check the lane
    MSLane *clane = MSLane::dictionary(id);
    if(clane==0) {
        throw InvalidArgument(
            string("On detector building:\n")
            + string("The lane with the id '") + id
            + string("' is not known."));
    }
    return clane;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


