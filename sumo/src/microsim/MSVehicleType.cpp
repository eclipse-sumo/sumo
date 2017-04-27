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
#include <microsim/cfmodels/MSCFModel_Rail.h>
#include "MSNet.h"
#include "cfmodels/MSCFModel_IDM.h"
#include "cfmodels/MSCFModel_Kerner.h"
#include "cfmodels/MSCFModel_Krauss.h"
#include "cfmodels/MSCFModel_KraussOrig1.h"
#include "cfmodels/MSCFModel_KraussPS.h"
#include "cfmodels/MSCFModel_KraussX.h"
#include "cfmodels/MSCFModel_SmartSK.h"
#include "cfmodels/MSCFModel_Daniel1.h"
#include "cfmodels/MSCFModel_PWag2009.h"
#include "cfmodels/MSCFModel_Wiedemann.h"
#include "MSVehicleType.h"


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


double
MSVehicleType::computeChosenSpeedDeviation(MTRand* rng, const double minDev) const {
    return MAX2(minDev, myParameter.speedFactor.sample(rng));
}


// ------------ Setter methods
void
MSVehicleType::setLength(const double& length) {
    if (myOriginalType != 0 && length < 0) {
        myParameter.length = myOriginalType->getLength();
    } else {
        myParameter.length = length;
    }
}


void
MSVehicleType::setHeight(const double& height) {
    if (myOriginalType != 0 && height < 0) {
        myParameter.height = myOriginalType->getHeight();
    } else {
        myParameter.height = height;
    }
}


void
MSVehicleType::setMinGap(const double& minGap) {
    if (myOriginalType != 0 && minGap < 0) {
        myParameter.minGap = myOriginalType->getMinGap();
    } else {
        myParameter.minGap = minGap;
    }
}


void
MSVehicleType::setMinGapLat(const double& minGapLat) {
    if (myOriginalType != 0 && minGapLat < 0) {
        myParameter.minGapLat = myOriginalType->getMinGapLat();
    } else {
        myParameter.minGapLat = minGapLat;
    }
}


void
MSVehicleType::setMaxSpeed(const double& maxSpeed) {
    if (myOriginalType != 0 && maxSpeed < 0) {
        myParameter.maxSpeed = myOriginalType->getMaxSpeed();
    } else {
        myParameter.maxSpeed = maxSpeed;
    }
}


void
MSVehicleType::setMaxSpeedLat(const double& maxSpeedLat) {
    if (myOriginalType != 0 && maxSpeedLat < 0) {
        myParameter.maxSpeedLat = myOriginalType->getMaxSpeedLat();
    } else {
        myParameter.maxSpeedLat = maxSpeedLat;
    }
}


void
MSVehicleType::setVClass(SUMOVehicleClass vclass) {
    myParameter.vehicleClass = vclass;
}

void
MSVehicleType::setPreferredLateralAlignment(LateralAlignment latAlignment) {
    myParameter.latAlignment = latAlignment;
}


void
MSVehicleType::setDefaultProbability(const double& prob) {
    if (myOriginalType != 0 && prob < 0) {
        myParameter.defaultProbability = myOriginalType->getDefaultProbability();
    } else {
        myParameter.defaultProbability = prob;
    }
}


void
MSVehicleType::setSpeedFactor(const double& factor) {
    if (myOriginalType != 0 && factor < 0) {
        myParameter.speedFactor.getParameter()[0] = myOriginalType->myParameter.speedFactor.getParameter()[0];
    } else {
        myParameter.speedFactor.getParameter()[0] = factor;
    }
}


void
MSVehicleType::setSpeedDeviation(const double& dev) {
    if (myOriginalType != 0 && dev < 0) {
        myParameter.speedFactor.getParameter()[1] = myOriginalType->myParameter.speedFactor.getParameter()[1];
    } else {
        myParameter.speedFactor.getParameter()[1] = dev;
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
MSVehicleType::setWidth(const double& width) {
    if (myOriginalType != 0 && width < 0) {
        myParameter.width = myOriginalType->getWidth();
    } else {
        myParameter.width = width;
    }
}


void
MSVehicleType::setImpatience(const double impatience) {
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
    const double accel = from.getCFParam(SUMO_ATTR_ACCEL, SUMOVTypeParameter::getDefaultAccel(from.vehicleClass));
    const double decel = from.getCFParam(SUMO_ATTR_DECEL, SUMOVTypeParameter::getDefaultDecel(from.vehicleClass));
//    const double emergencyDecel = from.getCFParam(SUMO_ATTR_EMERGENCYDECEL, SUMOVTypeParameter::getDefaultEmergencyDecel(from.vehicleClass));
    const double emergencyDecel = from.getCFParam(SUMO_ATTR_EMERGENCYDECEL, decel);
    // by default decel and apparentDecel are identical
    const double apparentDecel = from.getCFParam(SUMO_ATTR_APPARENTDECEL, decel);

    const double sigma = from.getCFParam(SUMO_ATTR_SIGMA, SUMOVTypeParameter::getDefaultImperfection(from.vehicleClass));
    const double tau = from.getCFParam(SUMO_ATTR_TAU, 1.);
    switch (from.cfModel) {
        case SUMO_TAG_CF_IDM:
            vtype->myCarFollowModel = new MSCFModel_IDM(vtype, accel, decel, emergencyDecel, tau,
                    from.getCFParam(SUMO_ATTR_CF_IDM_DELTA, 4.),
                    from.getCFParam(SUMO_ATTR_CF_IDM_STEPPING, .25));
            break;
        case SUMO_TAG_CF_IDMM:
            vtype->myCarFollowModel = new MSCFModel_IDM(vtype, accel, decel, emergencyDecel, tau,
                    from.getCFParam(SUMO_ATTR_CF_IDMM_ADAPT_FACTOR, 1.8),
                    from.getCFParam(SUMO_ATTR_CF_IDMM_ADAPT_TIME, 600.),
                    from.getCFParam(SUMO_ATTR_CF_IDM_STEPPING, .25));
            break;
        case SUMO_TAG_CF_BKERNER:
            vtype->myCarFollowModel = new MSCFModel_Kerner(vtype, accel, decel, emergencyDecel, tau,
                    from.getCFParam(SUMO_ATTR_K, .5),
                    from.getCFParam(SUMO_ATTR_CF_KERNER_PHI, 5.));
            break;
        case SUMO_TAG_CF_KRAUSS_ORIG1:
            vtype->myCarFollowModel = new MSCFModel_KraussOrig1(vtype, accel, decel, emergencyDecel, decel, sigma, tau);
            break;
        case SUMO_TAG_CF_KRAUSS_PLUS_SLOPE:
            vtype->myCarFollowModel = new MSCFModel_KraussPS(vtype, accel, decel, emergencyDecel, sigma, tau);
            break;
        case SUMO_TAG_CF_KRAUSSX:
            vtype->myCarFollowModel = new MSCFModel_KraussX(vtype, accel, decel, emergencyDecel, apparentDecel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_TMP1, 0.),
                    from.getCFParam(SUMO_ATTR_TMP2, 0.)
                                                           );
            break;
        case SUMO_TAG_CF_SMART_SK:
            vtype->myCarFollowModel = new MSCFModel_SmartSK(vtype, accel, decel, emergencyDecel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_TMP1, 1.),
                    from.getCFParam(SUMO_ATTR_TMP2, 1.),
                    from.getCFParam(SUMO_ATTR_TMP3, 1.),
                    from.getCFParam(SUMO_ATTR_TMP4, 1.),
                    from.getCFParam(SUMO_ATTR_TMP5, 1.));
            break;
        case SUMO_TAG_CF_DANIEL1:
            vtype->myCarFollowModel = new MSCFModel_Daniel1(vtype, accel, decel, emergencyDecel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_TMP1, 1.),
                    from.getCFParam(SUMO_ATTR_TMP2, 1.),
                    from.getCFParam(SUMO_ATTR_TMP3, 1.),
                    from.getCFParam(SUMO_ATTR_TMP4, 1.),
                    from.getCFParam(SUMO_ATTR_TMP5, 1.));
            break;
        case SUMO_TAG_CF_PWAGNER2009:
            vtype->myCarFollowModel = new MSCFModel_PWag2009(vtype, accel, decel, emergencyDecel, sigma, tau,
                    from.getCFParam(SUMO_ATTR_CF_PWAGNER2009_TAULAST, 0.3),
                    from.getCFParam(SUMO_ATTR_CF_PWAGNER2009_APPROB, 0.5));
            break;
        case SUMO_TAG_CF_WIEDEMANN:
            vtype->myCarFollowModel = new MSCFModel_Wiedemann(vtype, accel, decel, emergencyDecel,
                    from.getCFParam(SUMO_ATTR_CF_WIEDEMANN_SECURITY, 0.5),
                    from.getCFParam(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION, 0.5));
            break;
        case SUMO_TAG_CF_RAIL:
            vtype->myCarFollowModel = new MSCFModel_Rail(vtype, from.getCFParamString(SUMO_ATTR_TRAIN_TYPE, "NGT400"));
            break;
        case SUMO_TAG_CF_KRAUSS:
        default:
            vtype->myCarFollowModel = new MSCFModel_Krauss(vtype, accel, decel, emergencyDecel, apparentDecel, sigma, tau);
            break;
    }
    return vtype;
}


MSVehicleType*
MSVehicleType::buildSingularType(const std::string& id, const MSVehicleType* from) {
    MSVehicleType* vtype = new MSVehicleType(from->myParameter);
    vtype->myParameter.id = id;
    vtype->myCarFollowModel = from->myCarFollowModel->duplicate(vtype);
    if (from->myOriginalType != 0) {
        vtype->myOriginalType = from->myOriginalType;
        MSNet::getInstance()->getVehicleControl().removeVType(from);
    } else {
        vtype->myOriginalType = from;
    }
    if (!MSNet::getInstance()->getVehicleControl().addVType(vtype)) {
        throw ProcessError("could not add singular type " + vtype->getID());
    }
    return vtype;
}


/****************************************************************************/

