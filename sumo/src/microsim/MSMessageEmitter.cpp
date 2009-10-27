/****************************************************************************/
/// @file    MSMessageEmitter.cpp
/// @author  Clemens Honomichl
/// @date    Tue, 26 Feb 2008
///
// Builds detectors for microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#ifdef _MESSAGES

#include <utils/common/StringTokenizer.h>
#include <utils/iodevices/OutputDevice.h>
#include <iostream>
#include "MSMessageEmitter.h"
#include "MSLane.h"

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
MSMessageEmitter::MSMessageEmitter(std::string& file,
                                   const std::string& base,
                                   std::string& whatemit,
                                   bool reverse,
                                   bool tableOut,
                                   bool xy,
                                   SUMOReal step) : writeLCEvent(false), writeBEvent(false),
        writeHBEvent(false),
        myDev(OutputDevice::getDevice(file, base)) {
#ifdef _DEBUG
    cout << "constructing MSMessageEmitter with file '" + file + "'" << endl;
#endif
    MSMessageEmitter::xyCoords = xy;
    MSMessageEmitter::tableOutput = tableOut;
    MSMessageEmitter::reverseOrder = reverse;
    MSMessageEmitter::myStep = step;
    setWriteEvents(whatemit);
    if (!tableOutput)
        initXML();
}


MSMessageEmitter::~MSMessageEmitter() {
}


void
MSMessageEmitter::initXML() {
    myDev.writeXMLHeader("emitter");
}


std::string
MSMessageEmitter::trimmed(const std::string& str, const char* sepSet) {
    std::string::size_type const first = str.find_first_not_of(sepSet);
    return (first==std::string::npos) ?
           std::string() :
           str.substr(first, str.find_last_not_of(sepSet)-first+1);
}


void
MSMessageEmitter::writeLaneChangeEvent(const std::string& id, SUMOReal& timeStep,
                                       MSLane *oldlane, SUMOReal myPos,
                                       SUMOReal mySpeed, MSLane *newlane,
                                       SUMOReal x, SUMOReal y) {
    //myDev << "LaneChangeEvent\n";
    if (tableOutput) {
        if (!reverseOrder) {
            myDev << id + "\t\t"
            + oldlane->getID() + "\t" + newlane->getID()
            + "\t";
            if (xyCoords) {
                myDev << x << "\t" << y << "\t";
            }
            myDev << myPos;
            myDev << "\t" << mySpeed;
            myDev << "\t";
            myDev << timeStep;
            myDev << "\t\t1\n";
        } else {
            myDev << timeStep;
            myDev << "\t" + id + "\t\t"
            + oldlane->getID() + "\t" + newlane->getID()
            + "\t";
            if (xyCoords) {
                myDev << x << "\t" << y << "\t";
            }
            myDev << myPos;
            myDev << "\t" << mySpeed;
            myDev << "\t\t1\n";
        }
    } else {
        if (!reverseOrder) {
            myDev << "   <message vID=\"" + id + "\" oldlane=\""
            + oldlane->getID() + "\" newlane=\""
            + newlane->getID() + "\" pos=\"";
            myDev << myPos;
            myDev << "\" speed=\"" << mySpeed;
            if (xyCoords) {
                myDev << "\" X=\"" << x << "\" Y=\"" << y;
            }
            myDev << "\" edge=\"" << oldlane->getEdge().getID();
            myDev << "\" timestep=\"";
            myDev << timeStep;
            myDev << "\" event_type=\"lanechange";
            myDev << "\" />\n";
        } else {
            myDev << "   <message timestep=\"";
            myDev << timeStep;
            myDev << "\" vID=\"" + id + "\" oldlane=\""
            + oldlane->getID() + "\" newlane=\""
            + newlane->getID() + "\" pos=\"";
            myDev << myPos;
            myDev << "\" speed=\"" << mySpeed;
            myDev << "\" edge=\"" << oldlane->getEdge().getID();
            if (xyCoords) {
                myDev << "\" X=\"" << x << "\" Y=\"" << y;
            }
            myDev << "\" event_type=\"lanechange";
            myDev << "\" />\n";
        }
    }
}


bool
MSMessageEmitter::getWriteLCEvent() {
    return writeLCEvent;
}


bool
MSMessageEmitter::getWriteBEvent() {
    return writeBEvent;
}


bool
MSMessageEmitter::getWriteHBEvent() {
    return writeHBEvent;
}


bool
MSMessageEmitter::getEventsEnabled(const std::string& enabled) {
    bool retVal = false;
    if (enabled == "lanechange" && writeLCEvent) {
        retVal = true;
    } else if (enabled == "break" && writeBEvent) {
        retVal = true;
    } else if (enabled == "heartbeat" && writeHBEvent) {
        retVal = true;
    }
    return retVal;
}


void
MSMessageEmitter::writeHeartBeatEvent(const std::string &id, SUMOReal& timeStep, MSLane* lane, SUMOReal myPos,
                                      SUMOReal speed, SUMOReal x, SUMOReal y) {
    if (fmod(timeStep, myStep) == 0) {
        if (tableOutput) {
            if (!reverseOrder) {
                myDev << id + "\t\t";
                myDev << lane->getID();
                myDev << "\t";
                if (xyCoords) {
                    myDev << x << "\t" << y << "\t";
                }
                myDev << myPos;
                myDev << "\t" << speed;
                myDev << "\t";
                myDev << timeStep;
                myDev << "\t2\n";
            } else {
                myDev << timeStep;
                myDev << "\t" + id + "\t\t";
                myDev << lane->getID();
                myDev << "\t";
                if (xyCoords) {
                    myDev << x << "\t" << y << "\t";
                }
                myDev << myPos;
                myDev << "\t" << speed;
                myDev << "\t\t2\n";
            }
        } else {
            if (!reverseOrder) {
                myDev << "   <message vID=\"" + id + "\" lane=\"";
                myDev << lane->getID();
                myDev << "\" timestep=\"";
                myDev << timeStep;
                if (xyCoords) {
                    myDev << "\" X=\"" << x << "\" Y=\"" << y;
                }
                myDev << "\" edge=\"" << lane->getEdge().getID();
                myDev << "\" pos=\"" << myPos;
                myDev << "\" speed=\"" << speed;
                myDev << "\" event_type=\"heartbeat";
                myDev << "\" />\n";
            } else {
                myDev << "   <message timestep=\"";
                myDev << timeStep;
                myDev << "\" vID=\"" + id + "\" lane=\"";
                myDev << lane->getID();
                if (xyCoords) {
                    myDev << "\" X=\"" << x << "\" Y=\"" << y;
                }
                myDev << "\" edge=\"" << lane->getEdge().getID();
                myDev << "\" pos=\"" << myPos;
                myDev << "\" speed=\"" << speed;
                myDev << "\" event_type=\"heartbeat";
                myDev << "\" />\n";
            }
        }
    }
}


void
MSMessageEmitter::writeBreakEvent(const std::string& id, SUMOReal& timeStep, MSLane* lane, SUMOReal myPos,
                                  SUMOReal speed, SUMOReal x, SUMOReal y) {
    if (tableOutput) {
        if (!reverseOrder) {
            myDev << id + "\t\t";
            myDev << lane->getID();
            myDev << "\t";
            if (xyCoords) {
                myDev << x << "\t" << y << "\t";
            }
            myDev << myPos;
            myDev << "\t" << speed;
            myDev << "\t";
            myDev << timeStep;
            myDev << "\t2\n";
        } else {
            myDev << timeStep;
            myDev << "\t" + id + "\t\t";
            myDev << lane->getID();
            myDev << "\t";
            if (xyCoords) {
                myDev << x << "\t" << y << "\t";
            }
            myDev << myPos;
            myDev << "\t" << speed;
            myDev << "\t\t2\n";
        }
    } else {
        if (!reverseOrder) {
            myDev << "   <message vID=\"" + id + "\" lane=\"";
            myDev << lane->getID();
            myDev << "\" timestep=\"";
            myDev << timeStep;
            if (xyCoords) {
                myDev << "\" X=\"" << x << "\" Y=\"" << y;
            }
            myDev << "\" edge=\"" << lane->getEdge()->getID();
            myDev << "\" pos=\"" << myPos;
            myDev << "\" speed=\"" << speed;
            myDev << "\" event_type=\"break";
            myDev << "\" />\n";
        } else {
            myDev << "   <message timestep=\"";
            myDev << timeStep;
            myDev << "\" vID=\"" + id + "\" lane=\"";
            myDev << lane->getID();
            if (xyCoords) {
                myDev << "\" X=\"" << x << "\" Y=\"" << y;
            }
            myDev << "\" edge=\"" << lane->getEdge()->getID();
            myDev << "\" pos=\"" << myPos;
            myDev << "\" speed=\"" << speed;
            myDev << "\" event_type=\"break";
            myDev << "\" />\n";
        }
    }
}


void
MSMessageEmitter::setWriteEvents(std::string &events) {
    std::string tmp;
    StringTokenizer st(events, ";");
    while (st.hasNext()) {
        tmp = trimmed(st.next());
        if (tmp == "lanechange") {
#ifdef _DEBUG
            cout << "set event '" + tmp + "' to true" << endl;
#endif
            MSMessageEmitter::writeLCEvent = true;
        } else if (tmp == "break") {
#ifdef _DEBUG
            cout << "set event '" + tmp + "' to true" << endl;
#endif
            MSMessageEmitter::writeBEvent = true;
        } else if (tmp == "heartbeat") {
#ifdef _DEBUG
            cout << "set event '" + tmp + "' to true" << endl;
#endif
            MSMessageEmitter::writeHBEvent = true;
        } else {
            cout << "unknown event '" + tmp + "', skipping" << endl;
        }
    }
}


void
MSMessageEmitter::setFile(const std::string& file) {
#ifdef _DEBUG
    cout << "Filename: '" + file + "'." << endl;
#endif
    myDev = OutputDevice::getDevice(file);
#ifdef _DEBUG
    cout << "done..." << endl;
#endif
}
#endif
