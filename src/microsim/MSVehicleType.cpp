/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSVehicleType.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Thimor Bohn
/// @author  Michael Behrisch
/// @date    Tue, 06 Mar 2001
///
// The car-following model and parameter
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/common/StringUtils.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <microsim/cfmodels/MSCFModel_Rail.h>
#include "MSNet.h"
#include "cfmodels/MSCFModel_IDM.h"
#include "cfmodels/MSCFModel_Kerner.h"
#include "cfmodels/MSCFModel_Krauss.h"
#include "cfmodels/MSCFModel_KraussOrig1.h"
#include "cfmodels/MSCFModel_KraussPS.h"
#include "cfmodels/MSCFModel_KraussX.h"
#include "cfmodels/MSCFModel_EIDM.h"
#include "cfmodels/MSCFModel_SmartSK.h"
#include "cfmodels/MSCFModel_Daniel1.h"
#include "cfmodels/MSCFModel_PWag2009.h"
#include "cfmodels/MSCFModel_Wiedemann.h"
#include "cfmodels/MSCFModel_W99.h"
#include "cfmodels/MSCFModel_ACC.h"
#include "cfmodels/MSCFModel_CACC.h"
#include "MSInsertionControl.h"
#include "MSVehicleControl.h"
#include "cfmodels/MSCFModel_CC.h"
#include "MSVehicleType.h"


// ===========================================================================
// static members
// ===========================================================================
int MSVehicleType::myNextIndex = 0;


// ===========================================================================
// method definitions
// ===========================================================================
MSVehicleType::MSVehicleType(const SUMOVTypeParameter& parameter) :
    myParameter(parameter),
    myEnergyParams(&parameter),
    myWarnedActionStepLengthTauOnce(false),
    myWarnedActionStepLengthBallisticOnce(false),
    myWarnedStepLengthTauOnce(false),
    myIndex(myNextIndex++),
    myCarFollowModel(nullptr),
    myOriginalType(nullptr) {
    assert(getLength() > 0);
    assert(getMaxSpeed() > 0);

    // Check if actionStepLength was set by user, if not init to global default
    if (!myParameter.wasSet(VTYPEPARS_ACTIONSTEPLENGTH_SET)) {
        myParameter.actionStepLength = MSGlobals::gActionStepLength;
    }
    myCachedActionStepLengthSecs = STEPS2TIME(myParameter.actionStepLength);
}


MSVehicleType::~MSVehicleType() {
    delete myCarFollowModel;
}


double
MSVehicleType::computeChosenSpeedDeviation(SumoRNG* rng, const double minDev) const {
    return roundDecimal(MAX2(minDev, myParameter.speedFactor.sample(rng)), gPrecisionRandom);
}


// ------------ Setter methods
void
MSVehicleType::setLength(const double& length) {
    if (myOriginalType != nullptr && length < 0) {
        myParameter.length = myOriginalType->getLength();
    } else {
        myParameter.length = length;
    }
    myParameter.parametersSet |= VTYPEPARS_LENGTH_SET;
}


void
MSVehicleType::setHeight(const double& height) {
    if (myOriginalType != nullptr && height < 0) {
        myParameter.height = myOriginalType->getHeight();
    } else {
        myParameter.height = height;
    }
    myParameter.parametersSet |= VTYPEPARS_HEIGHT_SET;
}


void
MSVehicleType::setMinGap(const double& minGap) {
    if (myOriginalType != nullptr && minGap < 0) {
        myParameter.minGap = myOriginalType->getMinGap();
    } else {
        myParameter.minGap = minGap;
    }
    myParameter.parametersSet |= VTYPEPARS_MINGAP_SET;
}


void
MSVehicleType::setMinGapLat(const double& minGapLat) {
    if (myOriginalType != nullptr && minGapLat < 0) {
        myParameter.minGapLat = myOriginalType->getMinGapLat();
    } else {
        myParameter.minGapLat = minGapLat;
    }
    myParameter.parametersSet |= VTYPEPARS_MINGAP_LAT_SET;
}


void
MSVehicleType::setMaxSpeed(const double& maxSpeed) {
    if (myOriginalType != nullptr && maxSpeed < 0) {
        myParameter.maxSpeed = myOriginalType->getMaxSpeed();
    } else {
        myParameter.maxSpeed = maxSpeed;
    }
    myParameter.parametersSet |= VTYPEPARS_MAXSPEED_SET;
}


void
MSVehicleType::setMaxSpeedLat(const double& maxSpeedLat) {
    if (myOriginalType != nullptr && maxSpeedLat < 0) {
        myParameter.maxSpeedLat = myOriginalType->getMaxSpeedLat();
    } else {
        myParameter.maxSpeedLat = maxSpeedLat;
    }
    myParameter.parametersSet |= VTYPEPARS_MAXSPEED_LAT_SET;
}


void
MSVehicleType::setVClass(SUMOVehicleClass vclass) {
    myParameter.vehicleClass = vclass;
    myParameter.parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
}


void
MSVehicleType::setPreferredLateralAlignment(const LatAlignmentDefinition& latAlignment, double latAlignmentOffset) {
    myParameter.latAlignmentProcedure = latAlignment;
    myParameter.latAlignmentOffset = latAlignmentOffset;
    myParameter.parametersSet |= VTYPEPARS_LATALIGNMENT_SET;
}

void
MSVehicleType::setScale(double value) {
    myParameter.scale = value;
    MSInsertionControl& insertControl = MSNet::getInstance()->getInsertionControl();
    insertControl.updateScale(getID());
}

void
MSVehicleType::setDefaultProbability(const double& prob) {
    if (myOriginalType != nullptr && prob < 0) {
        myParameter.defaultProbability = myOriginalType->getDefaultProbability();
    } else {
        myParameter.defaultProbability = prob;
    }
    myParameter.parametersSet |= VTYPEPARS_PROBABILITY_SET;
}


void
MSVehicleType::setSpeedFactor(const double& factor) {
    if (myOriginalType != nullptr && factor < 0) {
        myParameter.speedFactor.getParameter()[0] = myOriginalType->myParameter.speedFactor.getParameter()[0];
    } else {
        myParameter.speedFactor.getParameter()[0] = factor;
    }
    myParameter.parametersSet |= VTYPEPARS_SPEEDFACTOR_SET;
}


void
MSVehicleType::setSpeedDeviation(const double& dev) {
    if (myOriginalType != nullptr && dev < 0) {
        myParameter.speedFactor.getParameter()[1] = myOriginalType->myParameter.speedFactor.getParameter()[1];
    } else {
        myParameter.speedFactor.getParameter()[1] = dev;
    }
    myParameter.parametersSet |= VTYPEPARS_SPEEDFACTOR_SET;
}


void
MSVehicleType::setActionStepLength(const SUMOTime actionStepLength, bool resetActionOffset) {
    assert(actionStepLength >= 0);
    myParameter.parametersSet |= VTYPEPARS_ACTIONSTEPLENGTH_SET;

    if (myParameter.actionStepLength == actionStepLength) {
        return;
    }

    SUMOTime previousActionStepLength = myParameter.actionStepLength;
    myParameter.actionStepLength = actionStepLength;
    myCachedActionStepLengthSecs = STEPS2TIME(myParameter.actionStepLength);
    check();

    if (isVehicleSpecific()) {
        // don't perform vehicle lookup for singular vtype
        return;
    }

    // For non-singular vType reset all vehicle's actionOffsets
    // Iterate through vehicles
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (auto vehIt = vc.loadedVehBegin(); vehIt != vc.loadedVehEnd(); ++vehIt) {
        MSVehicle* veh = static_cast<MSVehicle*>(vehIt->second);
        if (&veh->getVehicleType() == this) {
            // Found vehicle of this type. Perform requested actionOffsetReset
            if (resetActionOffset) {
                veh->resetActionOffset();
            } else {
                veh->updateActionOffset(previousActionStepLength, actionStepLength);
            }
        }
    }
}


void
MSVehicleType::setEmissionClass(SUMOEmissionClass eclass) {
    myParameter.emissionClass = eclass;
    myParameter.parametersSet |= VTYPEPARS_EMISSIONCLASS_SET;
}


void
MSVehicleType::setMass(double mass) {
    myParameter.mass = mass;
    myParameter.parametersSet |= VTYPEPARS_MASS_SET;
}


void
MSVehicleType::setColor(const RGBColor& color) {
    myParameter.color = color;
    myParameter.parametersSet |= VTYPEPARS_COLOR_SET;
}


void
MSVehicleType::setParkingBadges(const std::vector<std::string>& badges) {
    myParameter.parkingBadges.assign(badges.begin(), badges.end());
    myParameter.parametersSet |= VTYPEPARS_PARKING_BADGES_SET;
}


void
MSVehicleType::setWidth(const double& width) {
    if (myOriginalType != nullptr && width < 0) {
        myParameter.width = myOriginalType->getWidth();
    } else {
        myParameter.width = width;
    }
    myParameter.parametersSet |= VTYPEPARS_WIDTH_SET;
}

void
MSVehicleType::setImpatience(const double impatience) {
    myParameter.impatience = impatience;
    myParameter.parametersSet |= VTYPEPARS_IMPATIENCE_SET;
}


void
MSVehicleType::setBoardingDuration(SUMOTime duration, bool isPerson) {
    if (myOriginalType != nullptr && duration < 0) {
        myParameter.boardingDuration = myOriginalType->getBoardingDuration(isPerson);
    } else {
        if (isPerson) {
            myParameter.boardingDuration = duration;
        } else {
            myParameter.loadingDuration = duration;
        }
    }
    myParameter.parametersSet |= VTYPEPARS_BOARDING_DURATION;
}

void
MSVehicleType::setShape(SUMOVehicleShape shape) {
    myParameter.shape = shape;
    myParameter.parametersSet |= VTYPEPARS_SHAPE_SET;
}



// ------------ Static methods for building vehicle types
MSVehicleType*
MSVehicleType::build(SUMOVTypeParameter& from) {
    if (from.hasParameter("vehicleMass")) {
        if (from.wasSet(VTYPEPARS_MASS_SET)) {
            WRITE_WARNINGF(TL("The vType '%' has a 'mass' attribute and a 'vehicleMass' parameter. The 'mass' attribute will take precedence."), from.id);
        } else {
            WRITE_WARNINGF(TL("The vType '%' has a 'vehicleMass' parameter, which is deprecated. Please use the 'mass' attribute (for the empty mass) and the 'loading' parameter, if needed."), from.id);
            from.mass = from.getDouble("vehicleMass", from.mass);
            from.parametersSet |= VTYPEPARS_MASS_SET;
        }
    }
    MSVehicleType* vtype = new MSVehicleType(from);
    const double decel = from.getCFParam(SUMO_ATTR_DECEL, SUMOVTypeParameter::getDefaultDecel(from.vehicleClass));
    const double emergencyDecel = from.getCFParam(SUMO_ATTR_EMERGENCYDECEL, SUMOVTypeParameter::getDefaultEmergencyDecel(from.vehicleClass, decel, MSGlobals::gDefaultEmergencyDecel));
    // by default decel and apparentDecel are identical
    const double apparentDecel = from.getCFParam(SUMO_ATTR_APPARENTDECEL, decel);

    if (emergencyDecel < decel) {
        WRITE_WARNINGF(TL("Value of 'emergencyDecel' (%) should be higher than 'decel' (%) for vType '%'."), toString(emergencyDecel), toString(decel), from.id);
    }
    if (emergencyDecel < apparentDecel) {
        WRITE_WARNINGF(TL("Value of 'emergencyDecel' (%) is lower than 'apparentDecel' (%) for vType '%' may cause collisions."), toString(emergencyDecel), toString(apparentDecel), from.id);
    }

    switch (from.cfModel) {
        case SUMO_TAG_CF_IDM:
            vtype->myCarFollowModel = new MSCFModel_IDM(vtype, false);
            break;
        case SUMO_TAG_CF_IDMM:
            vtype->myCarFollowModel = new MSCFModel_IDM(vtype, true);
            break;
        case SUMO_TAG_CF_BKERNER:
            vtype->myCarFollowModel = new MSCFModel_Kerner(vtype);
            break;
        case SUMO_TAG_CF_KRAUSS_ORIG1:
            vtype->myCarFollowModel = new MSCFModel_KraussOrig1(vtype);
            break;
        case SUMO_TAG_CF_KRAUSS_PLUS_SLOPE:
            vtype->myCarFollowModel = new MSCFModel_KraussPS(vtype);
            break;
        case SUMO_TAG_CF_KRAUSSX:
            vtype->myCarFollowModel = new MSCFModel_KraussX(vtype);
            break;
        case SUMO_TAG_CF_EIDM:
            vtype->myCarFollowModel = new MSCFModel_EIDM(vtype);
            break;
        case SUMO_TAG_CF_SMART_SK:
            vtype->myCarFollowModel = new MSCFModel_SmartSK(vtype);
            break;
        case SUMO_TAG_CF_DANIEL1:
            vtype->myCarFollowModel = new MSCFModel_Daniel1(vtype);
            break;
        case SUMO_TAG_CF_PWAGNER2009:
            vtype->myCarFollowModel = new MSCFModel_PWag2009(vtype);
            break;
        case SUMO_TAG_CF_WIEDEMANN:
            vtype->myCarFollowModel = new MSCFModel_Wiedemann(vtype);
            break;
        case SUMO_TAG_CF_W99:
            vtype->myCarFollowModel = new MSCFModel_W99(vtype);
            break;
        case SUMO_TAG_CF_RAIL:
            vtype->myCarFollowModel = new MSCFModel_Rail(vtype);
            break;
        case SUMO_TAG_CF_ACC:
            vtype->myCarFollowModel = new MSCFModel_ACC(vtype);
            break;
        case SUMO_TAG_CF_CACC:
            vtype->myCarFollowModel = new MSCFModel_CACC(vtype);
            break;
        case SUMO_TAG_CF_CC:
            vtype->myCarFollowModel = new MSCFModel_CC(vtype);
            break;
        case SUMO_TAG_CF_KRAUSS:
        default:
            vtype->myCarFollowModel = new MSCFModel_Krauss(vtype);
            break;
    }
    // init Rail visualization parameters
    vtype->myParameter.initRailVisualizationParameters();
    return vtype;
}

SUMOTime
MSVehicleType::getEntryManoeuvreTime(const int angle) const {
    return (getParameter().getEntryManoeuvreTime(angle));
}

SUMOTime
MSVehicleType::getExitManoeuvreTime(const int angle) const {
    return (getParameter().getExitManoeuvreTime(angle));
}

MSVehicleType*
MSVehicleType::buildSingularType(const std::string& id) const {
    return duplicateType(id, false);
}


MSVehicleType*
MSVehicleType::duplicateType(const std::string& id, bool persistent) const {
    MSVehicleType* vtype = new MSVehicleType(myParameter);
    vtype->myParameter.id = id;
    vtype->myCarFollowModel = myCarFollowModel->duplicate(vtype);
    if (!persistent) {
        vtype->myOriginalType = this;
    }
    if (!MSNet::getInstance()->getVehicleControl().addVType(vtype)) {
        std::string singular = persistent ? "" : "singular ";
        throw ProcessError("could not add " + singular + "type " + vtype->getID());
    }
    return vtype;
}

void
MSVehicleType::check() {
    if (!myWarnedActionStepLengthTauOnce
            && myParameter.actionStepLength != DELTA_T
            && STEPS2TIME(myParameter.actionStepLength) > getCarFollowModel().getHeadwayTime()) {
        myWarnedActionStepLengthTauOnce = true;
        std::stringstream s;
        s << "Given action step length " << STEPS2TIME(myParameter.actionStepLength) << " for vehicle type '" << getID()
          << "' is larger than its parameter tau (=" << getCarFollowModel().getHeadwayTime() << ")!"
          << " This may lead to collisions. (This warning is only issued once per vehicle type).";
        WRITE_WARNING(s.str());
    }
    if (!myWarnedActionStepLengthBallisticOnce
            && myParameter.actionStepLength != DELTA_T
            && MSGlobals::gSemiImplicitEulerUpdate) {
        myWarnedActionStepLengthBallisticOnce = true;
        std::string warning2;
        if (OptionsCont::getOptions().isDefault("step-method.ballistic")) {
            warning2 = " Setting it now to avoid collisions.";
            MSGlobals::gSemiImplicitEulerUpdate = false;
        } else {
            warning2 = " This may cause collisions.";
        }
        WRITE_WARNINGF("Action step length '%' is used for vehicle type '%' but step-method.ballistic was not set." + warning2
                       , STEPS2TIME(myParameter.actionStepLength), getID())
    }
    if (!myWarnedStepLengthTauOnce && TS > getCarFollowModel().getHeadwayTime()
            && !MSGlobals::gUseMesoSim) {
        myWarnedStepLengthTauOnce = true;
        WRITE_WARNINGF(TL("Value of tau=% in vehicle type '%' lower than simulation step size may cause collisions."),
                       getCarFollowModel().getHeadwayTime(), getID());
    }
    if (MSGlobals::gUseMesoSim && getVehicleClass() != SVC_PEDESTRIAN && !OptionsCont::getOptions().getBool("meso-lane-queue")) {
        SVCPermissions ignoreVClasses = parseVehicleClasses(OptionsCont::getOptions().getStringVector("meso-ignore-lanes-by-vclass"));
        if ((ignoreVClasses & getVehicleClass()) != 0) {
            WRITE_WARNINGF(TL("Vehicle class '%' of vType '%' is set as ignored by option --meso-ignore-lanes-by-vclass to ensure default vehicle capacity. Set option --meso-lane-queue for multi-modal meso simulation"),
                           toString(getVehicleClass()), getID());
        }
    }
    if (!myParameter.wasSet(VTYPEPARS_EMISSIONCLASS_SET) && !OptionsCont::getOptions().getBool("device.battery.track-fuel")
            && (OptionsCont::getOptions().getFloat("device.battery.probability") == 1.
                || myParameter.getDouble("device.battery.probability", -1.) == 1.
                || StringUtils::toBool(myParameter.getParameter("has.battery.device", "false")))) {
        myParameter.emissionClass = PollutantsInterface::getClassByName("Energy");
        myParameter.parametersSet |= VTYPEPARS_EMISSIONCLASS_SET;
        WRITE_MESSAGEF(TL("The battery device is active for vType '%' but no emission class is set. The emission class Energy/unknown will be used, please consider setting an explicit emission class!"),
                       getID());
    }
}


void
MSVehicleType::setAccel(double accel) {
    if (myOriginalType != nullptr && accel < 0) {
        accel = myOriginalType->getCarFollowModel().getMaxAccel();
    }
    myCarFollowModel->setMaxAccel(accel);
    myParameter.cfParameter[SUMO_ATTR_ACCEL] = toString(accel);
}

void
MSVehicleType::setDecel(double decel) {
    if (myOriginalType != nullptr && decel < 0) {
        decel = myOriginalType->getCarFollowModel().getMaxDecel();
    }
    myCarFollowModel->setMaxDecel(decel);
    myParameter.cfParameter[SUMO_ATTR_DECEL] = toString(decel);
}

void
MSVehicleType::setEmergencyDecel(double emergencyDecel) {
    if (myOriginalType != nullptr && emergencyDecel < 0) {
        emergencyDecel = myOriginalType->getCarFollowModel().getEmergencyDecel();
    }
    myCarFollowModel->setEmergencyDecel(emergencyDecel);
    myParameter.cfParameter[SUMO_ATTR_EMERGENCYDECEL] = toString(emergencyDecel);
}

void
MSVehicleType::setApparentDecel(double apparentDecel) {
    if (myOriginalType != nullptr && apparentDecel < 0) {
        apparentDecel = myOriginalType->getCarFollowModel().getApparentDecel();
    }
    myCarFollowModel->setApparentDecel(apparentDecel);
    myParameter.cfParameter[SUMO_ATTR_APPARENTDECEL] = toString(apparentDecel);
}

void
MSVehicleType::setImperfection(double imperfection) {
    if (myOriginalType != nullptr && imperfection < 0) {
        imperfection = myOriginalType->getCarFollowModel().getImperfection();
    }
    myCarFollowModel->setImperfection(imperfection);
    myParameter.cfParameter[SUMO_ATTR_SIGMA] = toString(imperfection);
}

void
MSVehicleType::setTau(double tau) {
    if (myOriginalType != nullptr && tau < 0) {
        tau = myOriginalType->getCarFollowModel().getHeadwayTime();
    }
    myCarFollowModel->setHeadwayTime(tau);
    myParameter.cfParameter[SUMO_ATTR_TAU] = toString(tau);
}

/****************************************************************************/
