/****************************************************************************/
/// @file    MSCalibrator.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, May 2005
/// @version $Id: MSCalibrator.cpp 2345 2013-01-17 14:27:26Z behr_mi $
///
// Calibrates the flow on an edge by removing an inserting vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <algorithm>
#include <cmath>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEventControl.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <utils/common/RandomDistributor.h>
#include <utils/common/SUMOVehicleParameter.h>
#include "MSCalibrator.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSCalibrator::MSCalibrator(const std::string &id,
        const MSEdge *edge, SUMOReal pos,
        const std::string &aXMLFilename,
        const std::string &outputFilename,
        const SUMOTime freq)
        : MSTrigger(id), MSRouteHandler(aXMLFilename, false),
        myEdge(edge),
        myMeanData(0, myEdge->getLength(), false),
        myOutput(0), myFrequency(freq), myRemoved(0),
        myInserted(0), myClearedInJam(0),
        mySpeedIsDefault(true), myDidSpeedAdaption(false), myDidInit(false),
        myDefaultSpeed(myEdge->getSpeedLimit()),
        myHaveWarnedAboutClearingJam(false)
{
    if (outputFilename != "") {
        myOutput = &OutputDevice::getDevice(outputFilename);
        myOutput->writeXMLHeader("calibratorstats");
    }
    if (aXMLFilename != "") {
        XMLSubSys::runParser(*this, aXMLFilename);
        if (!myDidInit) {
            init();
        }
    }
}


void
MSCalibrator::init() {
    if (myIntervals.size() > 0) {
        if (myIntervals.back().end == -1) {
            myIntervals.back().end = SUMOTime_MAX;
        }
        myCurrentStateInterval = myIntervals.begin();
        // calibration should happen after regular insertions have taken place
        MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this,
                MSNet::getInstance()->getCurrentTimeStep(),
                MSEventControl::ADAPT_AFTER_EXECUTION);
        for (std::vector<MSLane*>::const_iterator i = myEdge->getLanes().begin(); i != myEdge->getLanes().end(); ++i) {
            (*i)->addMoveReminder(&myMeanData);
        }
    } else {
        WRITE_WARNING("No flow intervals in calibrator '" + myID + "'.");
    }
    myDidInit = true;
}


MSCalibrator::~MSCalibrator() {
    if (myCurrentStateInterval!=myIntervals.end()) {
        writeXMLOutput();
    }
    //mySegment->removeDetector(&myMeanData);
}


void
MSCalibrator::myStartElement(int element,
                                      const SUMOSAXAttributes &attrs) {
    if (element==SUMO_TAG_FLOW) {
        AspiredState state;
        int lastEnd = -1;
        if (myIntervals.size() > 0) {
            lastEnd = myIntervals.back().end;
            if (lastEnd == -1) {
                lastEnd = myIntervals.back().begin;
            }
        }
        try {
            bool ok = true;
            state.q = attrs.getOpt<SUMOReal>(SUMO_ATTR_VEHSPERHOUR, 0, ok, -1.);
            state.v = attrs.getOpt<SUMOReal>(SUMO_ATTR_SPEED, 0, ok, -1.);
            state.begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, myID.c_str(), ok);
            if (state.begin < lastEnd) {
                WRITE_ERROR("Overlapping or unsorted intervals in calibrator '" + myID + "'.");
            }
            state.end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, myID.c_str(), ok, -1);
            state.vehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, true, true);
            if (MSNet::getInstance()->getVehicleControl().getVType(state.vehicleParameter->vtypeid)==0) {
                WRITE_ERROR("Unknown vehicle type '" + state.vehicleParameter->vtypeid + "' in calibrator '" + myID + "'.");
            }
        } catch (EmptyData) {
            WRITE_ERROR("Mandatory attribute missing in definition of calibrator '" + myID + "'.");
        } catch (NumberFormatException) {
            WRITE_ERROR("Non-numeric value for numeric attribute in definition of calibrator '" + myID + "'.");
        }
        if (state.q < 0 && state.v < 0) {
            WRITE_ERROR("Either 'vehsPerHour' or 'speed' has to be given in flow definition of calibrator '" + myID + "'.");
        }
        if (myIntervals.size() > 0 && myIntervals.back().end == -1) {
            myIntervals.back().end = state.begin;
        }
        myIntervals.push_back(state);
    } else {
        MSRouteHandler::myStartElement(element, attrs);
    }
}


void
MSCalibrator::myEndElement(int element) {
    if (element==SUMO_TAG_CALIBRATOR) {
        if (!myDidInit) {
            init();
        }
    } else if (element!=SUMO_TAG_FLOW) {
        MSRouteHandler::myEndElement(element);
    }
}


void
MSCalibrator::writeXMLOutput() {
    if (myOutput != 0) {
        // vehicles drive to the end of an edge by default so they count as passed
        // but vaporized vehicles do not count
        const int p = passed();
        const SUMOReal durationSeconds = STEPS2TIME(myCurrentStateInterval->end-myCurrentStateInterval->begin);
        (*myOutput) << "   <interval begin=\"" << time2string(myCurrentStateInterval->begin) <<
        "\" end=\"" << time2string(myCurrentStateInterval->end) <<
        "\" id=\"" << myID <<
        "\" nVehContrib=\"" << p <<
        "\" removed=\""<< myRemoved << 
        "\" inserted=\"" << myInserted <<
        "\" cleared=\"" << myClearedInJam <<
        "\" flow=\"" << p * 3600.0 / durationSeconds <<
        "\" aspiredFlow=\"" << myCurrentStateInterval->q <<
        "\" speed=\""<< myMeanData.travelledDistance / myMeanData.getSamples() <<
        "\" aspiredSpeed=\""<< myCurrentStateInterval->v <<
        "\"/>\n";
    }
    myDidSpeedAdaption = false;
    myInserted = 0;
    myRemoved = 0;
    myClearedInJam = 0;
    myHaveWarnedAboutClearingJam = false;
    myMeanData.reset();
}


bool
MSCalibrator::isCurrentStateActive(SUMOTime time) {
    while (myCurrentStateInterval!=myIntervals.end() && myCurrentStateInterval->end<=time) {
        // XXX what about skipped intervals?
        myCurrentStateInterval++;
    }
    return myCurrentStateInterval!=myIntervals.end() &&
           myCurrentStateInterval->begin<=time && myCurrentStateInterval->end>time;
}


bool
MSCalibrator::tryEmit(MSLane *lane, MSVehicle *vehicle) {
    /// XXX Not yet implemented
    /*
    if (s->initialise(vehicle, vehicle->getParameter().depart)) {
        vehicle->onDepart();
        if (!MSNet::getInstance()->getVehicleControl().addVehicle(vehicle->getID(), vehicle)) {
            throw ProcessError("Emission of vehicle '" + vehicle->getID() + "' in calibrator '" + getID() + "'failed!");
        }
        return true;
    }
    */
    return false;
}


SUMOTime
MSCalibrator::execute(SUMOTime currentTime) {
    // get current simulation values (valid for the last simulation second)
    // XXX could we miss vehicle movements if this is called less often than every DELTA_T (default) ?
    //mySegment->prepareDetectorForWriting(myMeanData);

    // check whether an adaptation value exists
    if (isCurrentStateActive(currentTime)) {
        // all happens in isCurrentStateActive()
    } else {
        myMeanData.reset(); // discard collected values
        if (!mySpeedIsDefault) {
            // reset speed to default
            for (std::vector<MSLane*>::const_iterator i = myEdge->getLanes().begin(); i != myEdge->getLanes().end(); ++i) {
                (*i)->setMaxSpeed(myDefaultSpeed);
            }
            mySpeedIsDefault = true;
        }
        if (myCurrentStateInterval == myIntervals.end()) {
            return 0;
        }
        return myFrequency;
    }
    // we are active
    // XXX not yet implemented
    /*
    if (!myDidSpeedAdaption && myCurrentStateInterval->v >= 0) {
        MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(mySegment->getEdge());
        while (first != 0) {
            first->setSpeed(myCurrentStateInterval->v, currentTime, -1);
            first = first->getNextSegment();
        }
        mySpeedIsDefault = false;
        myDidSpeedAdaption = true;
    }
    // clear invalid jams
    bool hadInvalidJam = false;
    while (invalidJam()) {
        hadInvalidJam = true;
        if (!myHaveWarnedAboutClearingJam) {
            WRITE_WARNING("Clearing jam at calibrator '" + myID + "' at time " + time2string(currentTime));
        }
        // remove one vehicle currently on the segment
        if (mySegment->vaporizeAnyCar(currentTime)) {
            myClearedInJam++;
        } else {
            if (!myHaveWarnedAboutClearingJam) {
                // this frequenly happens for very short edges
                WRITE_WARNING("Could not clear jam at calibrator '" + myID + "' at time " + time2string(currentTime));
            }
            break;
        }
        myHaveWarnedAboutClearingJam = true;
    }
    if (myCurrentStateInterval->q >= 0) {
        // flow calibration starts here ...
        // compute the number of vehicles that should have passed the calibrator within the time
        // rom begin of the interval
        const SUMOReal totalHourFraction = STEPS2TIME(myCurrentStateInterval->end - myCurrentStateInterval->begin) / (SUMOReal) 3600.;
        const int totalWishedNum = (int)std::floor(myCurrentStateInterval->q * totalHourFraction + 0.5); // round to closest int
        int adaptedNum = passed() + myClearedInJam; 
        if (!hadInvalidJam) {
            // only add vehicles if we do not have an invalid upstream jam to prevent spill-back
            const SUMOReal hourFraction = STEPS2TIME(currentTime - myCurrentStateInterval->begin + DELTA_T) / (SUMOReal) 3600.;
            const int wishedNum = (int)std::floor(myCurrentStateInterval->q * hourFraction + 0.5); // round to closest int
            // only the difference between inflow and aspiredFlow should be added, thus
            // we should not count vehicles vaporized from a jam here
            // if we have enough time left we can add missing vehicles later
            const int relaxedInsertion = (int)std::floor(STEPS2TIME(myCurrentStateInterval->end - currentTime) / 3);
            const int insertionSlack = MAX2(0, adaptedNum + relaxedInsertion - totalWishedNum);
            // increase number of vehicles
            //std::cout << "time:" << STEPS2TIME(currentTime) << " w:" << wishedNum << " s:" << insertionSlack << " before:" << adaptedNum;
            while (wishedNum > adaptedNum + insertionSlack && remainingVehicleCapacity() > maximumInflow()) {
                SUMOVehicleParameter* pars = myCurrentStateInterval->vehicleParameter;
                const MSRoute *route = 0;
                StringTokenizer st(pars->routeid);
                while (route == 0 && st.hasNext()) {
                    route = MSRoute::dictionary(st.next());
                }
                if (route == 0) {
                    WRITE_WARNING("No valid routes in calibrator '" + myID + "'.");
                    break;
                }
                MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
                assert(route != 0 && vtype != 0);
                // build the vehicle
                const SUMOTime depart = mySegment->getNextInsertionTime(currentTime);
                SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                newPars->id = myID + "." + toString(depart) + "." + toString(myInserted);
                newPars->depart = depart;
                newPars->routeid = route->getID();
                MEVehicle *vehicle = static_cast<MEVehicle*>(MSNet::getInstance()->getVehicleControl().buildVehicle(
                            newPars, route, vtype));
                vehicle->setSegment(mySegment); // needed or vehicle will not be registered (XXX why?)
                vehicle->setEventTime(currentTime); // XXX superfluous?
                // move vehicle forward when the route does not begin at the calibrator's edge
                const MSEdge *myedge = &mySegment->getEdge();
                bool atDest = false;
                while (vehicle->getEdge()!=myedge) {
                    // let the vehicle move to the next edge
                    atDest = vehicle->moveRoutePointer();
                }
                // insert vehicle into the net
                if (atDest || !tryEmit(mySegment, vehicle)) {
                    //std::cout << "F ";
                    delete vehicle;
                    break;
                }
                //std::cout << "I ";
                myInserted++;
                adaptedNum++;
            }
        }
        //std::cout << " after:" << adaptedNum << "\n";
        // we only remove vehicles once we really have to
        while (totalWishedNum < adaptedNum) {
            if (!mySegment->vaporizeAnyCar(currentTime)) {
                // @bug: short edges may be jumped in a single step, giving us no chance to remove a vehicle
                break;
            }
            myRemoved++;
            adaptedNum--;
        }
    }
    if (myCurrentStateInterval->end <= currentTime + myFrequency) {
        writeXMLOutput();
    }
    assert(!invalidJam());
    */
    return myFrequency;
}


bool 
MSCalibrator::invalidJam() const {
    unsigned int vehiclesOnEdge = 0;
    SUMOReal meanSpeed = 0;
    for (std::vector<MSLane*>::const_iterator i = myEdge->getLanes().begin(); i != myEdge->getLanes().end(); ++i) {
        vehiclesOnEdge += (*i)->getVehicleNumber();
        meanSpeed += (*i)->getMeanSpeed();
    }
    meanSpeed /= myEdge->getLanes().size();
    if (vehiclesOnEdge == 0) {
        return false;
    }
    // maxSpeed reflects the calibration target
    const bool toSlow = meanSpeed < 0.8 * myEdge->getSpeedLimit();
    return toSlow && remainingVehicleCapacity() < maximumInflow();
}


int 
MSCalibrator::remainingVehicleCapacity() const {
    const SUMOVehicleParameter* pars = myCurrentStateInterval->vehicleParameter;
    const MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
    return myEdge->getLanes().size() * myEdge->getLength() / vtype->getLengthWithGap();
}


/****************************************************************************/

