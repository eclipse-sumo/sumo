/***************************************************************************
                          GUIDetectorBuilder.cpp
                          A building helper for the detectors
                             -------------------
    begin                : Tue, 22 Jul 2003
    copyright            : (C) 2003 by DLR http://ivf.dlr.de/
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
// Revision 1.10  2004/12/16 12:14:52  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.9  2004/11/23 10:12:27  dkrajzew
// new detectors usage applied
//
// Revision 1.8  2004/03/19 12:56:48  dkrajzew
// porting to FOX
//
// Revision 1.7  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics
//
// Revision 1.6  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.5  2003/11/11 08:16:50  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.4  2003/09/22 12:28:55  dkrajzew
// construction using two sample intervals inserted (using one by now)
//
// Revision 1.3  2003/08/14 13:45:46  dkrajzew
// new detector usage applied
//
// Revision 1.2  2003/08/04 11:35:51  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.1  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/output/MSInductLoop.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include <guisim/GUIE3Collector.h>
#include <microsim/output/MSDetector2File.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <microsim/logging/LoggedValue_Single.h>
#include <microsim/logging/LoggedValue_TimeFixed.h>
#include "GUIDetectorBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDetectorBuilder::GUIDetectorBuilder()
{
}


GUIDetectorBuilder::~GUIDetectorBuilder()
{
}


MSInductLoop *
GUIDetectorBuilder::createInductLoop(const std::string &id,
                                     MSLane *lane, double pos,
                                     int splInterval)
{
    return new GUIInductLoop(id, lane, pos, splInterval);
}


MSE2Collector *
GUIDetectorBuilder::createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, double pos, double length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    return new GUI_E2_ZS_Collector(id, usage, lane, pos, length,
        haltingTimeThreshold, haltingSpeedThreshold,
        jamDistThreshold, deleteDataAfterSeconds);

}


MS_E2_ZS_CollectorOverLanes *
GUIDetectorBuilder::createMultiLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, double pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    return new GUI_E2_ZS_CollectorOverLanes( id, usage, lane, pos,
            haltingTimeThreshold, haltingSpeedThreshold,
            jamDistThreshold, deleteDataAfterSeconds);
}


MSE3Collector *
GUIDetectorBuilder::createE3Detector(const std::string &id,
        const Detector::CrossSections &entries,
        const Detector::CrossSections &exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
{
    return new GUIE3Collector( id, entries, exits,
        haltingTimeThreshold, haltingSpeedThreshold, deleteDataAfterSeconds);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

