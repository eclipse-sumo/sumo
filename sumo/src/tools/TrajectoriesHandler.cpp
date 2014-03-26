/****************************************************************************/
/// @file    TrajectoriesHandler.cpp
/// @author  Michael Behrisch
/// @date    14.03.2014
/// @version $Id$
///
// An XML-Handler for amitran and netstate trajectories
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utility>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "TrajectoriesHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
TrajectoriesHandler::TrajectoriesHandler(const bool computeA, const SUMOEmissionClass defaultClass,
                                         const SUMOReal defaultSlope)
    : SUMOSAXHandler(""), myComputeA(computeA), myDefaultClass(defaultClass),
      myDefaultSlope(defaultSlope) {}


TrajectoriesHandler::~TrajectoriesHandler() {}


void
TrajectoriesHandler::myStartElement(int element,
                                    const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_TIMESTEP:
            myCurrentTime = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, 0, ok);
            break;
        case SUMO_TAG_VEHICLE:
            if (attrs.hasAttribute(SUMO_ATTR_SPEED)) {
                writeEmissions(std::cout, attrs.getString(SUMO_ATTR_ID), myDefaultClass, myCurrentTime, attrs.getFloat(SUMO_ATTR_SPEED));
            } else {
                myEmissionClassByVehicle[attrs.getString(SUMO_ATTR_ID)] = myEmissionClassByType[attrs.getString(SUMO_ATTR_ACTORCONFIG)];
            }
            break;
        case SUMO_TAG_ACTORCONFIG: {
            const std::string id = attrs.getString(SUMO_ATTR_ID);
            std::string vClass = attrs.getString(SUMO_ATTR_VEHICLECLASS);
            std::string fuel = attrs.getString(SUMO_ATTR_FUEL);
            std::string eClass = attrs.getString(SUMO_ATTR_EMISSIONCLASS);
            SUMOReal weight = attrs.getOpt(SUMO_ATTR_WEIGHT, id.c_str(), ok, 0.) * 10.;
            myEmissionClassByType[id] = PollutantsInterface::getClass(myDefaultClass, vClass, fuel, eClass, weight);
            break;
        }
        case SUMO_TAG_MOTIONSTATE: {
            const std::string id = attrs.getString(SUMO_ATTR_ID);
            const SUMOEmissionClass c = myEmissionClassByVehicle[id];
            const SUMOReal v = attrs.getFloat(SUMO_ATTR_SPEED);
            const SUMOReal a = attrs.getOpt(SUMO_ATTR_ACCELERATION, id.c_str(), ok, INVALID_VALUE);
            const SUMOReal s = attrs.getOpt(SUMO_ATTR_SLOPE, id.c_str(), ok, INVALID_VALUE);
            writeEmissions(std::cout, id, c, myCurrentTime, v);
            break;
        }
        default:
            break;
    }
}


void
TrajectoriesHandler::writeEmissions(std::ostream& o, const std::string id,
                                    const SUMOEmissionClass c,
                                    const SUMOReal t, const SUMOReal v,
                                    SUMOReal a, SUMOReal s) {
    if (myComputeA) {
        a = v - myLastV[id];
        myLastV[id] = v;
    }
    if (a == INVALID_VALUE) {
        throw ProcessError("Acceleration information is missing; try running with --compute-a.");
    }
    if (s == INVALID_VALUE) {
        s = myDefaultSlope;
    }
    const SUMOReal aCO = PollutantsInterface::computeCO(c, v, a, s);
    const SUMOReal aCO2 = PollutantsInterface::computeCO2(c, v, a, s);
    const SUMOReal aHC = PollutantsInterface::computeHC(c, v, a, s);
    const SUMOReal aNOx = PollutantsInterface::computeNOx(c, v, a, s);
    const SUMOReal aPMx = PollutantsInterface::computePMx(c, v, a, s);
    const SUMOReal aFuel = PollutantsInterface::computeFuel(c, v, a, s);
    mySumCO[id] += aCO;
    mySumCO2[id] += aCO2;
    mySumHC[id] += aHC;
    mySumNOx[id] += aNOx;
    mySumPMx[id] += aPMx;
    mySumFuel[id] += aFuel;
    o << t << ";" << v << ";" << a << ";" << s
    << ";" << aCO << ";" << aCO2 << ";" << aHC << ";" << aPMx << ";" << aNOx << ";" << aFuel << std::endl;
}


void
TrajectoriesHandler::writeSums(std::ostream& o, const std::string id) {
    o << "CO:" << mySumCO[id] << std::endl
    << "CO2:" << mySumCO2[id] << std::endl
    << "HC:" << mySumHC[id] << std::endl
    << "NOx:" << mySumNOx[id] << std::endl
    << "PMx:" << mySumPMx[id] << std::endl
    << "fuel:" << mySumFuel[id] << std::endl;
}


/****************************************************************************/
