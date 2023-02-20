/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    VehicleEngineHandler.cpp
/// @author  Michele Segata
/// @date    4 Feb 2015
///
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include "VehicleEngineHandler.h"


// ===========================================================================
// static variables
// ===========================================================================
StringBijection<int>::Entry VehicleEngineHandler::engineTags[] = {
    { "vehicles",     VehicleEngineHandler::ENGINE_TAG_VEHICLES },
    { "vehicle",      VehicleEngineHandler::ENGINE_TAG_VEHICLE },
    { "gears",        VehicleEngineHandler::ENGINE_TAG_GEARS                  },
    { "gear",         VehicleEngineHandler::ENGINE_TAG_GEAR                   },
    { "differential", VehicleEngineHandler::ENGINE_TAG_GEAR_DIFFERENTIAL      },
    { "mass",         VehicleEngineHandler::ENGINE_TAG_MASS                   },
    { "wheels",       VehicleEngineHandler::ENGINE_TAG_WHEELS                 },
    { "drag",         VehicleEngineHandler::ENGINE_TAG_DRAG                   },
    { "engine",       VehicleEngineHandler::ENGINE_TAG_ENGINE                 },
    { "power",        VehicleEngineHandler::ENGINE_TAG_ENGINE_POWER           },
    { "shifting",     VehicleEngineHandler::ENGINE_TAG_SHIFTING               },
    { "brakes",       VehicleEngineHandler::ENGINE_TAG_BRAKES                 },

    { "",             VehicleEngineHandler::ENGINE_TAG_NOTHING }
};


StringBijection<int>::Entry VehicleEngineHandler::engineAttrs[] = {
    { "id",           VehicleEngineHandler::ENGINE_ATTR_ID},
    { "n",            VehicleEngineHandler::ENGINE_ATTR_GEAR_N                 },
    { "ratio",        VehicleEngineHandler::ENGINE_ATTR_GEAR_RATIO             },
    { "mass",         VehicleEngineHandler::ENGINE_ATTR_MASS                   },
    { "massFactor",   VehicleEngineHandler::ENGINE_ATTR_MASS_FACTOR            },
    { "diameter",     VehicleEngineHandler::ENGINE_ATTR_WHEELS_DIAMETER        },
    { "friction",     VehicleEngineHandler::ENGINE_ATTR_WHEELS_FRICTION        },
    { "cr1",          VehicleEngineHandler::ENGINE_ATTR_WHEELS_CR1             },
    { "cr2",          VehicleEngineHandler::ENGINE_ATTR_WHEELS_CR2             },
    { "cAir",         VehicleEngineHandler::ENGINE_ATTR_DRAG_CAIR              },
    { "section",      VehicleEngineHandler::ENGINE_ATTR_DRAG_SECTION           },
    { "type",         VehicleEngineHandler::ENGINE_ATTR_ENGINE_TYPE            },
    { "efficiency",   VehicleEngineHandler::ENGINE_ATTR_ENGINE_EFFICIENCY      },
    { "cylinders",    VehicleEngineHandler::ENGINE_ATTR_ENGINE_CYLINDERS       },
    { "minRpm",       VehicleEngineHandler::ENGINE_ATTR_ENGINE_MINRPM          },
    { "maxRpm",       VehicleEngineHandler::ENGINE_ATTR_ENGINE_MAXRPM          },
    { "tauEx",        VehicleEngineHandler::ENGINE_ATTR_ENGINE_TAU_EX          },
    { "tauBurn",      VehicleEngineHandler::ENGINE_ATTR_ENGINE_TAU_BURN        },
    { "rpm",          VehicleEngineHandler::ENGINE_ATTR_ENGINE_POWER_RPM       },
    { "hp",           VehicleEngineHandler::ENGINE_ATTR_ENGINE_POWER_HP        },
    { "kw",           VehicleEngineHandler::ENGINE_ATTR_ENGINE_POWER_KW        },
    { "slope",        VehicleEngineHandler::ENGINE_ATTR_ENGINE_POWER_SLOPE     },
    { "intercept",    VehicleEngineHandler::ENGINE_ATTR_ENGINE_POWER_INTERCEPT },
    { "rpm",          VehicleEngineHandler::ENGINE_ATTR_SHIFTING_RPM           },
    { "deltaRpm",     VehicleEngineHandler::ENGINE_ATTR_SHIFTING_DELTARPM      },
    { "tau",          VehicleEngineHandler::ENGINE_ATTR_BRAKES_TAU             },

    { "",             VehicleEngineHandler::ENGINE_ATTR_NOTHING }
};


// ===========================================================================
// method definitions
// ===========================================================================
VehicleEngineHandler::VehicleEngineHandler(const std::string& toLoad)
    : GenericSAXHandler(engineTags, ENGINE_TAG_NOTHING, engineAttrs, ENGINE_ATTR_NOTHING, "vehicles"),
      skip(false), currentGear(1) {
    vehicleToLoad = toLoad;
}


VehicleEngineHandler::~VehicleEngineHandler() {}


void
VehicleEngineHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    switch (element) {
        case ENGINE_TAG_VEHICLE:
            skip = attrs.getString(ENGINE_ATTR_ID) != vehicleToLoad;
            if (!skip) {
                engineParameters.id = vehicleToLoad;
            }
            break;
        case ENGINE_TAG_GEAR:
            if (!skip) {
                int number = attrs.getInt(ENGINE_ATTR_GEAR_N);
                if (number != currentGear) {
                    //fatal
                    std::stringstream ss;
                    ss << "Invalid gear number " << number << ". Please check that gears are inserted in order";
                    throw ProcessError(ss.str());
                }
                gearRatios.push_back(attrs.getFloat(ENGINE_ATTR_GEAR_RATIO));
                currentGear++;
            }
            break;
        case ENGINE_TAG_GEAR_DIFFERENTIAL:
            if (!skip) {
                // Load final drive ratio
                engineParameters.differentialRatio = attrs.getFloat(ENGINE_ATTR_GEAR_RATIO);
            }
            break;
        case ENGINE_TAG_MASS:
            if (!skip) {
                // Loads mass information, i.e., mass in kg and mass factor which takes into account rotational parts of the engine
                engineParameters.mass_kg = attrs.getFloat(ENGINE_ATTR_MASS);
                engineParameters.massFactor = attrs.getFloat(ENGINE_ATTR_MASS_FACTOR);
            }
            break;
        case ENGINE_TAG_DRAG:
            if (!skip) {
                // Load air drag related data such as drag coefficient and maximum vehicle section
                engineParameters.cAir = attrs.getFloat(ENGINE_ATTR_DRAG_CAIR);
                engineParameters.a_m2 = attrs.getFloat(ENGINE_ATTR_DRAG_SECTION);
            }
            break;
        case ENGINE_TAG_WHEELS:
            if (!skip) {
                // Load data about vehicle's wheels, such as diameter and friction coefficient
                engineParameters.wheelDiameter_m = attrs.getFloat(ENGINE_ATTR_WHEELS_DIAMETER);
                engineParameters.tiresFrictionCoefficient = attrs.getFloat(ENGINE_ATTR_WHEELS_FRICTION);
                engineParameters.cr1 = attrs.getFloat(ENGINE_ATTR_WHEELS_CR1);
                engineParameters.cr2 = attrs.getFloat(ENGINE_ATTR_WHEELS_CR2);
            }
            break;
        case ENGINE_TAG_ENGINE:
            if (!skip) {
                // Load data about the engine, such as efficiency factor and cylinders
                engineParameters.engineEfficiency = attrs.getFloat(ENGINE_ATTR_ENGINE_EFFICIENCY);
                engineParameters.cylinders = attrs.getInt(ENGINE_ATTR_ENGINE_CYLINDERS);
                engineParameters.minRpm = attrs.getInt(ENGINE_ATTR_ENGINE_MINRPM);
                engineParameters.maxRpm = attrs.getInt(ENGINE_ATTR_ENGINE_MAXRPM);
                engineParameters.tauEx_s = attrs.getFloat(ENGINE_ATTR_ENGINE_TAU_EX);
                engineParameters.fixedTauBurn = attrs.hasAttribute(ENGINE_ATTR_ENGINE_TAU_BURN);
                if (engineParameters.fixedTauBurn) {
                    engineParameters.tauBurn_s = attrs.getFloat(ENGINE_ATTR_ENGINE_TAU_BURN);
                }
                engineParameters.maxRpm = attrs.getInt(ENGINE_ATTR_ENGINE_MAXRPM);
                std::string mapType = attrs.getString(ENGINE_ATTR_ENGINE_TYPE);
                if (mapType != "poly") {
                    throw ProcessError(TL("Invalid engine map type. Only \"poly\" is supported for now"));
                }
            }
            break;
        case ENGINE_TAG_ENGINE_POWER:
            if (!skip) {
                //check that the degree is within the maximum supported
                if (attrs.hasAttribute("x" + toString(MAX_POLY_DEGREE))) {
                    std::stringstream ss;
                    ss << "Maximum degree for the engine polynomial is " << MAX_POLY_DEGREE << ". Please check your model's data";
                    throw ProcessError(ss.str());
                }
                //parse all polynomial coefficients
                for (int i = 0; i < MAX_POLY_DEGREE; i++) {
                    const std::string param = "x" + toString(i);
                    if (attrs.hasAttribute(param)) {
                        engineParameters.engineMapping.x[i] = attrs.getFloat(param);
                    } else {
                        //save the actual degree
                        engineParameters.engineMapping.degree = i;
                        break;
                    }
                }
            }
            break;
        case ENGINE_TAG_SHIFTING:
            if (!skip) {
                engineParameters.shiftingRule.rpm = attrs.getFloat(ENGINE_ATTR_SHIFTING_RPM);
                engineParameters.shiftingRule.deltaRpm = attrs.getFloat(ENGINE_ATTR_SHIFTING_DELTARPM);
            }
            break;
        case ENGINE_TAG_BRAKES:
            if (!skip) {
                engineParameters.brakesTau_s = attrs.getFloat(ENGINE_ATTR_BRAKES_TAU);
            }
            break;
        case ENGINE_TAG_VEHICLES:
        case ENGINE_TAG_GEARS:
            break;
        default:
            WRITE_WARNINGF(TL("Unknown tag '%' while parsing."), toString(element));
    }
}


void
VehicleEngineHandler::myEndElement(int element) {
    switch (element) {
        case ENGINE_TAG_GEARS:
            delete[] engineParameters.gearRatios;
            engineParameters.gearRatios = new double[gearRatios.size()];
            for (int i = 0; i < (int)gearRatios.size(); i++) {
                engineParameters.gearRatios[i] = gearRatios[i];
            }
            engineParameters.nGears = (int)gearRatios.size();
            break;
        case ENGINE_TAG_VEHICLES:
            engineParameters.computeCoefficients();
            break;
        default:
            break;
    }
}


/****************************************************************************/
