/****************************************************************************/
/// @file    METriggeredCalibrator.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// Calibrates the flow on a segment to a specified one
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSRouteProbe.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <utils/common/RandomDistributor.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MELoop.h"
#include "MESegment.h"
#include "MEVehicle.h"
#include "METriggeredCalibrator.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
METriggeredCalibrator::METriggeredCalibrator(const std::string& id,
        const MSEdge* const edge, const SUMOReal pos,
        const std::string& aXMLFilename,
        const std::string& outputFilename,
        const SUMOTime freq, const SUMOReal length,
        const MSRouteProbe* probe)
    : MSCalibrator(id, edge, pos, aXMLFilename, outputFilename, freq, length, probe, false),
      mySegment(MSGlobals::gMesoNet->getSegmentForEdge(*edge, pos)) {
    myEdgeMeanData.setDescription("meandata_calibrator_" + getID());
    mySegment->addDetector(&myEdgeMeanData);
}


METriggeredCalibrator::~METriggeredCalibrator() {
    if (myCurrentStateInterval != myIntervals.end()) {
        writeXMLOutput();
        myCurrentStateInterval = myIntervals.end();
    }
    mySegment->removeDetector(&myEdgeMeanData);
}


bool
METriggeredCalibrator::tryEmit(MESegment* s, MEVehicle* vehicle) {
    if (s->initialise(vehicle, vehicle->getParameter().depart)) {
        if (!MSNet::getInstance()->getVehicleControl().addVehicle(vehicle->getID(), vehicle)) {
            throw ProcessError("Emission of vehicle '" + vehicle->getID() + "' in calibrator '" + getID() + "'failed!");
        }
        return true;
    }
    return false;
}


SUMOTime
METriggeredCalibrator::execute(SUMOTime currentTime) {
    // get current simulation values (valid for the last simulation second)
    // XXX could we miss vehicle movements if this is called less often than every DELTA_T (default) ?
    mySegment->prepareDetectorForWriting(myEdgeMeanData);

    // check whether an adaptation value exists
    if (isCurrentStateActive(currentTime)) {
        // all happens in isCurrentStateActive()
    } else {
        myEdgeMeanData.reset(); // discard collected values
        if (!mySpeedIsDefault) {
            // if not, reset adaptation values
            mySegment->getEdge().setMaxSpeed(myDefaultSpeed);
            MESegment* first = MSGlobals::gMesoNet->getSegmentForEdge(mySegment->getEdge());
            const SUMOReal jamThresh = OptionsCont::getOptions().getFloat("meso-jam-threshold");
            while (first != 0) {
                first->setSpeed(myDefaultSpeed, currentTime, jamThresh);
                first = first->getNextSegment();
            }
            mySpeedIsDefault = true;
        }
        if (myCurrentStateInterval == myIntervals.end()) {
            // keep calibrator alive but do not call again
            return TIME2STEPS(86400);
        }
        return myFrequency;
    }
    // we are active
    if (!myDidSpeedAdaption && myCurrentStateInterval->v >= 0 && myCurrentStateInterval->v != mySegment->getEdge().getSpeedLimit()) {
        mySegment->getEdge().setMaxSpeed(myCurrentStateInterval->v);
        MESegment* first = MSGlobals::gMesoNet->getSegmentForEdge(mySegment->getEdge());
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
                const MSRoute* route = myProbe != 0 ? myProbe->getRoute() : 0;
                if (route == 0) {
                    route = MSRoute::dictionary(pars->routeid);
                }
                if (route == 0) {
                    WRITE_WARNING("No valid routes in calibrator '" + myID + "'.");
                    break;
                }
                if (!route->contains(myEdge)) {
                    WRITE_WARNING("Route '" + route->getID() + "' in calibrator '" + myID + "' does not contain edge '" + myEdge->getID() + "'.");
                    break;
                }
                MSVehicleType* vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
                assert(route != 0 && vtype != 0);
                // build the vehicle
                const SUMOTime depart = mySegment->getNextInsertionTime(currentTime);
                SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                newPars->id = myID + "." + toString(depart) + "." + toString(myInserted);
                newPars->depart = depart;
                newPars->routeid = route->getID();
                MEVehicle* vehicle = static_cast<MEVehicle*>(MSNet::getInstance()->getVehicleControl().buildVehicle(
                                         newPars, route, vtype, false, false));
                vehicle->setSegment(mySegment); // needed or vehicle will not be registered (XXX why?)
                vehicle->setEventTime(currentTime); // XXX superfluous?
                // move vehicle forward when the route does not begin at the calibrator's edge
                const MSEdge* myedge = &mySegment->getEdge();
                bool atDest = false;
                while (vehicle->getEdge() != myedge) {
                    // let the vehicle move to the next edge
                    atDest = vehicle->moveRoutePointer();
                }
                // insert vehicle into the net
                if (atDest || !tryEmit(mySegment, vehicle)) {
                    //std::cout << "F ";
                    MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
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
    return myFrequency;
}


bool
METriggeredCalibrator::invalidJam() const {
    if (mySegment->getBruttoOccupancy() == 0.) {
        return false;
    }
    // maxSpeed reflects the calibration target
    const bool toSlow = mySegment->getMeanSpeed() < 0.8 * mySegment->getEdge().getSpeedLimit();
    return toSlow && remainingVehicleCapacity() < maximumInflow();
}


int
METriggeredCalibrator::remainingVehicleCapacity() const {
    const SUMOVehicleParameter* pars = myCurrentStateInterval->vehicleParameter;
    const MSVehicleType* vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
    return mySegment->remainingVehicleCapacity(vtype->getLengthWithGap());
}


void
METriggeredCalibrator::reset() {
    myEdgeMeanData.reset();
}


/****************************************************************************/

