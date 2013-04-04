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

//#define MSCalibrator_DEBUG

// ===========================================================================
// static members
// ===========================================================================
std::vector<MSMoveReminder*> MSCalibrator::LeftoverReminders;
std::vector<SUMOVehicleParameter*> MSCalibrator::LeftoverVehicleParameters;

// ===========================================================================
// method definitions
// ===========================================================================
MSCalibrator::MSCalibrator(const std::string &id,
        MSEdge *edge, SUMOReal pos,
        const std::string &aXMLFilename,
        const std::string &outputFilename,
        const SUMOTime freq)
        : MSTrigger(id), MSRouteHandler(aXMLFilename, false),
        myEdge(edge),
        myEdgeMeanData(0, myEdge->getLength(), false),
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
            MSMeanData_Net::MSLaneMeanDataValues* laneData = new MSMeanData_Net::MSLaneMeanDataValues(*i, myEdge->getLength(), false);
            LeftoverReminders.push_back(laneData);
            myLaneMeanData.push_back(laneData);
            (*i)->addMoveReminder(laneData);
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
            LeftoverVehicleParameters.push_back(state.vehicleParameter);
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
        "\" speed=\""<< myEdgeMeanData.travelledDistance / myEdgeMeanData.getSamples() <<
        "\" aspiredSpeed=\""<< myCurrentStateInterval->v <<
        "\"/>\n";
    }
    myDidSpeedAdaption = false;
    myInserted = 0;
    myRemoved = 0;
    myClearedInJam = 0;
    myHaveWarnedAboutClearingJam = false;
    reset();
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
    
    myEdgeMeanData.reset();
    for (std::vector<MSMeanData_Net::MSLaneMeanDataValues*>::iterator it = myLaneMeanData.begin(); it != myLaneMeanData.end(); ++it) {
        (*it)->addTo(myEdgeMeanData);
    }

    // check whether an adaptation value exists
    if (isCurrentStateActive(currentTime)) {
        // all happens in isCurrentStateActive()
    } else {
        reset();
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
    if (!myDidSpeedAdaption && myCurrentStateInterval->v >= 0) {
        for (std::vector<MSLane*>::const_iterator i = myEdge->getLanes().begin(); i != myEdge->getLanes().end(); ++i) {
            (*i)->setMaxSpeed(myCurrentStateInterval->v);
        }
        mySpeedIsDefault = false;
        myDidSpeedAdaption = true;
    }

    const bool calibrateFlow = myCurrentStateInterval->q >= 0;
    const SUMOReal totalHourFraction = STEPS2TIME(myCurrentStateInterval->end - myCurrentStateInterval->begin) / (SUMOReal) 3600.;
    const int totalWishedNum = (int)std::floor(myCurrentStateInterval->q * totalHourFraction + 0.5); // round to closest int
    int adaptedNum = passed() + myClearedInJam; 
    bool hasInvalidJam = invalidJam();
#ifdef MSCalibrator_DEBUG
    std::cout << time2string(currentTime) << " " << myID 
        << " q=" << myCurrentStateInterval->q
        << " totalWished=" << totalWishedNum
        << " adapted=" << adaptedNum
        << " jam=" << hasInvalidJam
        << " entered=" << myEdgeMeanData.nVehEntered
        << " departed=" << myEdgeMeanData.nVehDeparted
        << " arrived=" << myEdgeMeanData.nVehArrived
        << " left=" << myEdgeMeanData.nVehLeft
        << " waitSecs=" << myEdgeMeanData.waitSeconds
        << "\n";
#endif
    if ((calibrateFlow && adaptedNum >= totalWishedNum) || hasInvalidJam) {
        // if enough vehicles have passed this calibrator we wish to vaporize all
        // subsequent vehicles in this calibration interval
        // Likewise, if the edge is experiencing an invalid jam we want to vaporize
        // all subsequent vehicles to avoid back-propagation of the jam
        myEdge->incVaporization(currentTime);
        if (hasInvalidJam) {
            if (!myHaveWarnedAboutClearingJam) {
                WRITE_WARNING("Clearing jam at calibrator '" + myID + "' at time " + time2string(currentTime));
                myHaveWarnedAboutClearingJam = true;
            }
        }
    } else {
        if (myEdge->isVaporizing()) {
            // disable previous vaporization request
            myEdge->decVaporization(currentTime);
        }
        // maybe we need to insert some vehicles
        if (calibrateFlow) {
            const SUMOReal hourFraction = STEPS2TIME(currentTime - myCurrentStateInterval->begin + DELTA_T) / (SUMOReal) 3600.;
            const int wishedNum = (int)std::floor(myCurrentStateInterval->q * hourFraction + 0.5); // round to closest int
            // only the difference between inflow and aspiredFlow should be added, thus
            // we should not count vehicles vaporized from a jam here
            // if we have enough time left we can add missing vehicles later
            const int relaxedInsertion = (int)std::floor(STEPS2TIME(myCurrentStateInterval->end - currentTime) / 3);
            const int insertionSlack = MAX2(0, adaptedNum + relaxedInsertion - totalWishedNum);
            // increase number of vehicles
#ifdef MSCalibrator_DEBUG
            std::cout  
                << "   wished:" << wishedNum 
                << " slack:" << insertionSlack 
                << " before:" << adaptedNum
                << " remainCap:" << remainingVehicleCapacity()
                << " maxIn:" << maximumInflow()
                << "\n";
#endif
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
                if (!route->contains(myEdge)) {
                    WRITE_WARNING("Route '" + route->getID() + "' in calibrator '" + myID + "' does not contain edge '" + myEdge->getID() + "'.");
                    break;
                }
                const unsigned int routeIndex = (unsigned int)std::distance(route->begin(), 
                        std::find(route->begin(), route->end(), myEdge));
                MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
                assert(route != 0 && vtype != 0);
                // build the vehicle
                SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                newPars->id = myID + "." + toString((int)STEPS2TIME(myCurrentStateInterval->begin)) + "." + toString(myInserted);
                newPars->depart = currentTime;
                newPars->routeid = route->getID();
                MSVehicle *vehicle = dynamic_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().buildVehicle(
                            newPars, route, vtype));
#ifdef MSCalibrator_DEBUG
                std::cout << " resetting route pos: " << routeIndex << "\n";
#endif
                vehicle->resetRoutePosition(routeIndex);
                if (myEdge->insertVehicle(*vehicle, currentTime)) {
                    vehicle->onDepart();
                    myInserted++;
                    adaptedNum++;
#ifdef MSCalibrator_DEBUG
                    std::cout << "I ";
#endif
                } else {
                    // could not insert vehicle
#ifdef MSCalibrator_DEBUG
                    std::cout << "F ";
#endif
                    MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
                    break;
                }
            }
        }
    }
    if (myCurrentStateInterval->end <= currentTime + myFrequency) {
        writeXMLOutput();
    }
    return myFrequency;
}

void 
MSCalibrator::reset() {
    myEdgeMeanData.reset();
    for (std::vector<MSMeanData_Net::MSLaneMeanDataValues*>::iterator it = myLaneMeanData.begin(); it != myLaneMeanData.end(); ++it) {
        (*it)->reset();
    }
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


void
MSCalibrator::cleanup() {
    for (std::vector<MSMoveReminder*>::iterator it = LeftoverReminders.begin(); it != LeftoverReminders.end(); ++it) {
        delete *it;
    }
    for (std::vector<SUMOVehicleParameter*>::iterator it = LeftoverVehicleParameters.begin(); 
            it != LeftoverVehicleParameters.end(); ++it) {
        delete *it;
    }
}

/****************************************************************************/

