/****************************************************************************/
/// @file    MEInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MEInductLoop.cpp 96 2007-06-06 07:40:46Z behr_mi $
///
// An induction loop for mesoscopic simulation
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MEInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <limits>
#include <utils/helpers/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <mesosim/MESegment.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member variable definitions
// ===========================================================================
string MEInductLoop::xmlHeaderM(
    "<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
    "\n\n");

string MEInductLoop::xmlDetectorInfoEndM("</detector>\n");


// ===========================================================================
// method definitions
// ===========================================================================
MEInductLoop::MEInductLoop(const string& id,
                           MESegment* s,
                           SUMOReal positionInMeters,
                           SUMOTime deleteDataAfterSeconds)
        : posM(positionInMeters),
        deleteDataAfterStepsM(MSNet::getSteps((SUMOReal) deleteDataAfterSeconds)),
        myID(id), mySegment(s)
{
    assert(posM >= 0 && posM <= s->getLength());
    // start old-data removal through MSEventControl
    s->addDetectorData(this, deleteDataAfterStepsM,
                       MSNet::getInstance()->getCurrentTimeStep());
}


MEInductLoop::~MEInductLoop()
{}



void
MEInductLoop::writeXMLHeader(OutputDevice &dev) const
{
    dev.writeString(xmlHeaderM);
}


void
MEInductLoop::writeXMLDetectorInfoStart(OutputDevice &dev) const
{
    dev.writeString("<detector type=\"inductionloop\" id=\"" + myID +
                    "\" lane=\"" + mySegment->getID() + "\" pos=\"" +
                    toString(posM) + "\" >");
}


void
MEInductLoop::writeXMLDetectorInfoEnd(OutputDevice &dev) const
{
    dev.writeString(xmlDetectorInfoEndM);
}


void
MEInductLoop::writeXMLOutput(OutputDevice &dev,
                             SUMOTime startTime, SUMOTime stopTime)
{
    SUMOTime t(stopTime-startTime+1);
    MSLaneMeanDataValues &meanData = mySegment->getDetectorData(this, stopTime+1);
    SUMOReal traveltime = -42;
    SUMOReal meanSpeed = -43;
    SUMOReal meanDensity = -45;
    if (meanData.nSamples==0.) {
        traveltime = mySegment->getLength() / mySegment->getMaxSpeed();
        meanSpeed = mySegment->getMaxSpeed();
        meanDensity = 0;
    } else {
        meanSpeed = meanData.speedSum / (SUMOReal) meanData.nSamples;
        if (meanSpeed==0) {
            traveltime = std::numeric_limits<SUMOReal>::max() / (SUMOReal) 100.;
        } else {
            traveltime = mySegment->getLength() / meanSpeed;
        }
        meanDensity = (SUMOReal) meanData.vehLengthSum /
                      (SUMOReal)(stopTime-startTime+1) / mySegment->getLength();
    }
    dev.writeString("<interval begin=\"").writeString(
        toString(startTime)).writeString("\" end=\"").writeString(
            toString(stopTime)).writeString("\" ");
    if (dev.needsDetectorName()) {
        dev.writeString("id=\"").writeString(myID).writeString("\" ");
    }
    dev.writeString("nSamples=\"").writeString(
        toString(meanData.nSamples)).writeString("\" speed=\"").writeString(
            toString(meanSpeed)).writeString("\" traveltime=\"").writeString(
                toString(traveltime)).writeString("\" density=\"").writeString(
                    toString(meanDensity)).writeString("\"/>");
    mySegment->flushDetectorData(this, stopTime+1);
}


SUMOTime
MEInductLoop::getDataCleanUpSteps(void) const
{
    return deleteDataAfterStepsM;
}



/****************************************************************************/

