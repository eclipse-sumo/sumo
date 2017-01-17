/****************************************************************************/
/// @file    MSVehicleType.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Thimor Bohn
/// @author  Michael Behrisch
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// The car-following model and parameter
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

#include <cassert>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include "MSNet.h"
#include "cfmodels/MSCFModel_IDM.h"
#include "cfmodels/MSCFModel_Kerner.h"
#include "cfmodels/MSCFModel_Krauss.h"
#include "cfmodels/MSCFModel_KraussOrig1.h"
#include "cfmodels/MSCFModel_KraussPS.h"
#include "cfmodels/MSCFModel_SmartSK.h"
#include "cfmodels/MSCFModel_Daniel1.h"
#include "cfmodels/MSCFModel_PWag2009.h"
#include "cfmodels/MSCFModel_Wiedemann.h"
#include "MSVehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
int MSVehicleType::myNextIndex = 0;


// ===========================================================================
// method definitions
// ===========================================================================
MSVehicleType::MSVehicleType(const SUMOVTypeParameter& parameter)
    : myParameter(parameter), myIndex(myNextIndex++), myCarFollowModel(0), myOriginalType(0) {
    assert(getLength() > 0);
    assert(getMaxSpeed() > 0);
}


MSVehicleType::~MSVehicleType() {
    delete myCarFollowModel;
}


SUMOReal
MSVehicleType::computeChosenSpeedDeviation(MTRand* rng, const SUMOReal minDevFactor) const {
    if (myParameter.speedDev == 0) {
        return myParameter.speedFactor;
    }
    // for speedDev = 0.1, most 95% of the vehicles will drive between 80% and 120% of speedLimit * speedFactor
    const SUMOReal devA = MIN2(SUMOReal(2.), RandHelper::randNorm(0, 1., rng));
    // avoid voluntary speeds below 20% of the requested speedFactor
    return MAX2(minDevFactor, SUMOReal(devA * myParameter.speedDev + 1.)) * myParameter.speedFactor;
}


// ------------ Setter methods
void
MSVehicleType::setLength(const SUMOReal& length) {
    if (myOriginalType != 0 && length < 0) {
        myParameter.length = myOriginalType->getLength();
    } else {
        myParameter.length = length;
    }
}


void
MSVehicleType::setHeight(const SUMOReal& height) {
    if (myOriginalType != 0 && height < 0) {
        myParameter.height = myOriginalType->getHeight();
    } else {
        myParameter.height = height;
    }
}


void
MSVehicleType::setMinGap(const SUMOReal& minGap) {
    if (myOriginalType != 0 && minGap < 0) {
        myParameter.minGap = myOriginalType->getMinGap();
    } else {
        myParameter.minGap = minGap;
    }
}


void
MSVehicleType::setMaxSpeed(const SUMOReal& maxSpeed) {
    if (myOriginalType != 0 && maxSpeed < 0) {
        myParameter.maxSpeed = myOriginalType->getMaxSpeed();
    } else {
        myParameter.maxSpeed = maxSpeed;
    }
}


void
MSVehicleType::setVClass(SUMOVehicleClass vclass) {
    myParameter.vehicleClass = vclass;
}


void
MSVehicleType::setDefaultProbability(const SUMOReal& prob) {
    if (myOriginalType != 0 && prob < 0) {
        myParameter.defaultProbability = myOriginalType->getDefaultProbability();
    } else {
        myParameter.defaultProbability = prob;
    }
}


void
MSVehicleType::setSpeedFactor(const SUMOReal& factor) {
    if (myOriginalType != 0 && factor < 0) {
        myParameter.speedFactor = myOriginalType->getSpeedFactor();
    } else {
        myParameter.speedFactor = factor;
    }
}


void
MSVehicleType::setSpeedDeviation(const SUMOReal& dev) {
    if (myOriginalType != 0 && dev < 0) {
        myParameter.speedDev = myOriginalType->getSpeedDeviation();
    } else {
        myParameter.speedDev = dev;
    }
}


void
MSVehicleType::setEmissionClass(SUMOEmissionClass eclass) {
    myParameter.emissionClass = eclass;
}


void
MSVehicleType::setColor(const RGBColor& color) {
    myParameter.color = color;
}


void
MSVehicleType::setWidth(const SUMOReal& width) {
    if (myOriginalType != 0 && width < 0) {
        myParameter.width = myOriginalType->getWidth();
    } else {
        myParameter.width = width;
    }
}


void
MSVehicleType::setImpatience(const SUMOReal impatience) {
    if (myOriginalType != 0 && impatience < 0) {
        myParameter.impatience = myOriginalType->getImpatience();
    } else {
        myParameter.impatience = impatience;
    }
}


void
MSVehicleType::setShape(SUMOVehicleShape shape) {
    myParameter.shape = shape;
}



// ------------ Static methods for building vehicle types
MSVehicleType*
MSVehicleType::build(SUMOVTypeParameter& from) {
    MSVehicleType* vtype = new MSVehicleType(from);
    const SUMOReal accel = from.getCFParam(SUMO_ATTR_ACCEL, SUMOVTypeParameter::getDefaultAccel(from.vehicleClass));
    const SUMOReal decel = from.getCFParam(SUMO_ATTR_DECEL, SUMOVTypeParameter::getDefaultDecel(from.vehicleClass));
    const SUMOReal sigma = from.getCFParam(SUMO_ATTR_SIGMA, SUMOVTypeParameter::getDefaultImperfection(from.vehicleClass));
    const SUMOReal tau = from.getCFParam(SUMO_ATTR_TAU, 1.);
    switch (from.cfModel) {
        case SUMO_TAG_CF_IDM:
            vtype->myCarFollowModel = new MSCFModel_IDM(vtype, accel, decel, tau,
                    from.getCFParam(SUMO_ATTR_CF_IDM_DELTA, 4.),
                    from.getCFParam(SUMO_ATTR_CF_IDM_STEPPING, .25));
            break;
        case SUMO_TAG_CF_IDMM:
            vtype->myCarFollowModel = new MSCFModel_IDM(vtype, accel, decel, tau,
                    from.getCFParam(SUMO_ATTR_CF_IDMM_ADAPT_FACTOR, 1.8),
                    from.getCFParam(SUMO_ATTR_CF_IDMM_ADAPT_TIME, 600.),
                    from.getCFParam(SUMO_ATTR_CF_IDM_STEPPING, .25));
            break;
        case SUMO_TAG_CF_BKERNER:
            vtype->myCarFollowModel = new MSCFModel_Kerner(vtype, accel, decel, tau,
                    from.getCFParam(SUMO_ATTR_K, .5),
                    from.getCFParam(SUMO_ATTR_CF_KERNER_PHI, 5.));
            break;
        case SUMO_TAG_CF_KRAUSS_ORIG1:
            vtype->myCarFollowModel = new MSCFModel_KraussOrig1(vtype, accel, decel, sigma, tau);
            break;
        case SUMO_TAG_CF_KRAUSS_PLUS_SLOPE:
            vtype->myCarFollowModel = new MSCFModel_KraussPS(vtype, accel, decel, sigma, tau);
            break;
        case SUMO_TAG_CF_SMART_SK:
            vtype->myCarFollowModel = new MSCFModel_SmartSK(vtype, accel, decel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_TMP1, 1.),
                    from.getCFParam(SUMO_ATTR_TMP2, 1.),
                    from.getCFParam(SUMO_ATTR_TMP3, 1.),
                    from.getCFParam(SUMO_ATTR_TMP4, 1.),
                    from.getCFParam(SUMO_ATTR_TMP5, 1.));
            break;
        case SUMO_TAG_CF_DANIEL1:
            vtype->myCarFollowModel = new MSCFModel_Daniel1(vtype, accel, decel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_TMP1, 1.),
                    from.getCFParam(SUMO_ATTR_TMP2, 1.),
                    from.getCFParam(SUMO_ATTR_TMP3, 1.),
                    from.getCFParam(SUMO_ATTR_TMP4, 1.),
                    from.getCFParam(SUMO_ATTR_TMP5, 1.));
            break;
        case SUMO_TAG_CF_PWAGNER2009:
            vtype->myCarFollowModel = new MSCFModel_PWag2009(vtype, accel, decel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_CF_PWAGNER2009_TAULAST, 0.3),
                    from.getCFParam(SUMO_ATTR_CF_PWAGNER2009_APPROB, 0.5));
            break;
        case SUMO_TAG_CF_WIEDEMANN:
            vtype->myCarFollowModel = new MSCFModel_Wiedemann(vtype, accel, decel,
                    from.getCFParam(SUMO_ATTR_CF_WIEDEMANN_SECURITY, 0.5),
                    from.getCFParam(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION, 0.5));
            break;
        case SUMO_TAG_CF_KRAUSS:
        default:
            vtype->myCarFollowModel = new MSCFModel_Krauss(vtype, accel, decel, sigma, tau);
            break;
    }
    return vtype;
}


MSVehicleType*
MSVehicleType::build(const std::string& id, const MSVehicleType* from) {
    MSVehicleType* vtype = new MSVehicleType(from->myParameter);
    vtype->myParameter.id = id;
    vtype->myCarFollowModel = from->myCarFollowModel->duplicate(vtype);
    vtype->myOriginalType = from->myOriginalType != 0 ? from->myOriginalType : from;
    return vtype;
}


/****************************************************************************/

