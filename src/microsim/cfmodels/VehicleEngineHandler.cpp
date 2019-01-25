/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    VehicleEngineHandler.cpp
/// @author  Michele Segata
/// @date    4 Feb 2015
/// @version $Id: $
///
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include "VehicleEngineHandler.h"
#include <stdio.h>

// ===========================================================================
// class definitions
// ===========================================================================
VehicleEngineHandler::VehicleEngineHandler(const std::string& toLoad)
    : currentTag(TAG_VEHICLES), skip(false), currentGear(1), engineMapType(POLYNOMIAL) {
    vehicleToLoad = toLoad;
}


VehicleEngineHandler::~VehicleEngineHandler() {}

std::string transcode(const XMLCh* const qname) {
    return std::string(XERCES_CPP_NAMESPACE::XMLString::transcode(qname));
}

const XMLCh* transcode(const char* name) {
    return XERCES_CPP_NAMESPACE::XMLString::transcode(name);
}

std::string getAttributeValue(const char* attributeName, const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    return transcode(attrs.getValue(transcode(attributeName)));
}

void
VehicleEngineHandler::startElement(const XMLCh* const /*uri*/,
                                   const XMLCh* const /*localname*/,
                                   const XMLCh* const qname,
                                   const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    std::string tag = XERCES_CPP_NAMESPACE::XMLString::transcode(qname);

    switch (currentTag) {

        case TAG_VEHICLES:

            //we are already inside the root. just ignore this
            if (tag == ENGINE_TAG_VEHICLES) {
                break;
            }

            //this is a new vehicle definition. is this the one we should load?
            if (tag == ENGINE_TAG_VEHICLE) {
                if (getAttributeValue(ENGINE_TAG_VEHICLE_ID, attrs) != vehicleToLoad) {
                    skip = true;
                } else {
                    engineParameters.id = vehicleToLoad;
                }
                currentTag = TAG_VEHICLE;
            }

            break;

        case TAG_VEHICLE:

            //we are not interested in this vehicle
            if (skip) {
                break;
            }

            //definition of gear ratios
            if (tag == ENGINE_TAG_GEARS) {
                currentTag = TAG_GEARS;
            }
            //definition of masses
            else if (tag == ENGINE_TAG_MASS) {
                loadMassData(attrs);
            }
            //definition of air drag
            else if (tag == ENGINE_TAG_DRAG) {
                loadDragData(attrs);
            }
            //definition of wheels
            else if (tag == ENGINE_TAG_WHEELS) {
                loadWheelsData(attrs);
            }
            //definition of engine
            else if (tag == ENGINE_TAG_ENGINE) {
                loadEngineData(attrs);
                currentTag = TAG_ENGINE;
            }
            //definition of the shifting rule
            else if (tag == ENGINE_TAG_SHIFTING) {
                loadShiftingData(attrs);
            }
            //definition of brakes
            else if (tag == ENGINE_TAG_BRAKES) {
                loadBrakesData(attrs);
            } else {
                raiseUnknownTagError(tag);
            }

            break;

        case TAG_GEARS:

            if (skip) {
                break;
            }

            if (tag == ENGINE_TAG_GEAR) {
                //definition of the ratio for a single gear
                loadGearData(attrs);
            } else if (tag == ENGINE_TAG_GEAR_DIFFERENTIAL) {
                //definition of the ratio for the final drive
                loadDifferentialData(attrs);
            } else {
                raiseUnknownTagError(tag);
            }

            break;

        case TAG_ENGINE:

            if (skip) {
                break;
            }

            switch (engineMapType) {
                case POLYNOMIAL:
                default:
                    if (tag == ENGINE_TAG_ENGINE_POWER) {
                        loadEngineModelData(attrs);
                    } else {
                        raiseUnknownTagError(tag);
                    }
                    break;
            }

            break;

        default:

            break;

    }

}


void
VehicleEngineHandler::endElement(const XMLCh* const /*uri*/,
                                 const XMLCh* const /*localname*/,
                                 const XMLCh* const qname) {
    std::string tag = XERCES_CPP_NAMESPACE::XMLString::transcode(qname);

    switch (currentTag) {

        case TAG_VEHICLES:
            break;

        case TAG_VEHICLE:
            if (tag == ENGINE_TAG_VEHICLE) {
                skip = false;
                currentTag = TAG_VEHICLES;
            }
            break;

        case TAG_GEARS:
            if (tag == ENGINE_TAG_GEARS) {
                currentTag = TAG_VEHICLE;
                currentGear = 0;

                delete [] engineParameters.gearRatios;
                engineParameters.gearRatios = new double[gearRatios.size()];
                for (unsigned int i = 0; i < gearRatios.size(); i++) {
                    engineParameters.gearRatios[i] = gearRatios[i];
                }
                engineParameters.nGears = gearRatios.size();
            }

            break;

        case TAG_ENGINE:
            if (tag == ENGINE_TAG_ENGINE) {
                currentTag = TAG_VEHICLE;
            }
            break;

        default:

            break;

    }

}

void VehicleEngineHandler::endDocument() {
    engineParameters.computeCoefficients();
}

const EngineParameters& VehicleEngineHandler::getEngineParameters() {
    return engineParameters;
}

void VehicleEngineHandler::loadMassData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.mass_kg = parseDoubleAttribute(ENGINE_TAG_MASS, ENGINE_TAG_MASS_MASS, attrs);
    engineParameters.massFactor = parseDoubleAttribute(ENGINE_TAG_MASS, ENGINE_TAG_MASS_FACTOR, attrs);
}
void VehicleEngineHandler::loadDragData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.cAir = parseDoubleAttribute(ENGINE_TAG_DRAG, ENGINE_TAG_DRAG_CAIR, attrs);
    engineParameters.a_m2 = parseDoubleAttribute(ENGINE_TAG_DRAG, ENGINE_TAG_DRAG_SECTION, attrs);
}
void VehicleEngineHandler::loadWheelsData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.wheelDiameter_m = parseDoubleAttribute(ENGINE_TAG_WHEELS, ENGINE_TAG_WHEELS_DIAMETER, attrs);
    engineParameters.tiresFrictionCoefficient = parseDoubleAttribute(ENGINE_TAG_WHEELS, ENGINE_TAG_WHEELS_FRICTION, attrs);
    engineParameters.cr1 = parseDoubleAttribute(ENGINE_TAG_WHEELS, ENGINE_TAG_WHEELS_CR1, attrs);
    engineParameters.cr2 = parseDoubleAttribute(ENGINE_TAG_WHEELS, ENGINE_TAG_WHEELS_CR2, attrs);
}
void VehicleEngineHandler::loadEngineData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.engineEfficiency = parseDoubleAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_EFFICIENCY, attrs);
    engineParameters.cylinders = parseIntAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_CYLINDERS, attrs);
    engineParameters.minRpm = parseIntAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_MINRPM, attrs);
    engineParameters.maxRpm = parseIntAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_MAXRPM, attrs);
    engineParameters.tauEx_s = parseDoubleAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_TAU_EX, attrs);
    if (existsAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_TAU_BURN, attrs) != -1) {
        engineParameters.tauBurn_s = parseDoubleAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_TAU_BURN, attrs);
        engineParameters.fixedTauBurn = true;
    } else {
        engineParameters.fixedTauBurn = false;
    }
    engineParameters.maxRpm = parseIntAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_MAXRPM, attrs);
    std::string mapType = parseStringAttribute(ENGINE_TAG_ENGINE, ENGINE_TAG_ENGINE_TYPE, attrs);
    if (mapType == "poly") {
        engineMapType = POLYNOMIAL;
    } else {
        raiseError("Invalid engine map type. Only \"poly\" is supported for now");
    }
}
void VehicleEngineHandler::loadGearData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {

    int number = parseIntAttribute(ENGINE_TAG_GEAR, ENGINE_TAG_GEAR_N, attrs);
    if (number != currentGear) {
        //fatal
        std::stringstream ss;
        ss << "Invalid gear number " << number << ". Please check that gears are inserted in order";
        raiseError(ss.str());
    }
    gearRatios.push_back(parseDoubleAttribute(ENGINE_TAG_GEAR, ENGINE_TAG_GEAR_RATIO, attrs));
    currentGear++;

}
void VehicleEngineHandler::loadDifferentialData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.differentialRatio = parseDoubleAttribute(ENGINE_TAG_GEAR_DIFFERENTIAL, ENGINE_TAG_GEAR_RATIO, attrs);
}
void VehicleEngineHandler::loadEngineMapData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    double rpm = parseDoubleAttribute(ENGINE_TAG_ENGINE_POWER, ENGINE_TAG_ENGINE_POWER_RPM, attrs);
    double power_hp;
    if (existsAttribute(ENGINE_TAG_ENGINE_POWER, ENGINE_TAG_ENGINE_POWER_HP, attrs)) {
        power_hp = parseDoubleAttribute(ENGINE_TAG_ENGINE_POWER, ENGINE_TAG_ENGINE_POWER_HP, attrs);
    } else if (existsAttribute(ENGINE_TAG_ENGINE_POWER, ENGINE_TAG_ENGINE_POWER_KW, attrs)) {
        power_hp = parseDoubleAttribute(ENGINE_TAG_ENGINE_POWER, ENGINE_TAG_ENGINE_POWER_KW, attrs) / 1000 / HP_TO_W;
    } else {
        raiseError("Cannot find power for rpm. Please use either hp or kw");
    }
    engineMap.push_back(std::pair<double, double> (rpm, power_hp));
}
void VehicleEngineHandler::loadEngineModelData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    uint8_t i;
    //check that the degree is within the maximum supported
    if (attrs.getLength() > MAX_POLY_DEGREE) {
        std::stringstream ss;
        ss << "Maximum degree for the engine polynomial is " << MAX_POLY_DEGREE << ". Please check your model's data";
        raiseError(ss.str());
    }
    //parse all polynomial coefficients
    for (i = 0; i < attrs.getLength(); i++) {
        engineParameters.engineMapping.x[i] = parsePolynomialCoefficient(i, attrs);
    }
    //save the actual degree
    engineParameters.engineMapping.degree = attrs.getLength();
}
void VehicleEngineHandler::loadShiftingData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.shiftingRule.rpm = parseDoubleAttribute(ENGINE_TAG_SHIFTING, ENGINE_TAG_SHIFTING_RPM, attrs);
    engineParameters.shiftingRule.deltaRpm = parseDoubleAttribute(ENGINE_TAG_SHIFTING, ENGINE_TAG_SHIFTING_DELTARPM, attrs);
}
void VehicleEngineHandler::loadBrakesData(const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    engineParameters.brakesTau_s = parseDoubleAttribute(ENGINE_TAG_BRAKES, ENGINE_TAG_BRAKES_TAU, attrs);
}

int VehicleEngineHandler::existsAttribute(std::string tag, const char* attribute, const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    return attrs.getIndex(transcode(attribute));
}
std::string VehicleEngineHandler::parseStringAttribute(std::string tag, const char* attribute, const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    int attributeIndex;
    std::string strValue;
    attributeIndex = existsAttribute(tag, attribute, attrs);
    if (attributeIndex == -1) {
        //raise will stop execution
        raiseMissingAttributeError(tag, attribute);
    }
    return transcode(attrs.getValue(attributeIndex));
}
int VehicleEngineHandler::parseIntAttribute(std::string tag, const char* attribute, const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    std::string strValue;
    int intValue;
    strValue = parseStringAttribute(tag, attribute, attrs);
    if (!toInt(strValue, intValue)) {
        //raise will stop execution
        raiseParsingError(tag, attribute, strValue);
    }
    return intValue;
}
double VehicleEngineHandler::parseDoubleAttribute(std::string tag, const char* attribute, const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    std::string strValue;
    double doubleValue;
    strValue = parseStringAttribute(tag, attribute, attrs);
    if (!toDouble(strValue, doubleValue)) {
        //raise will stop execution
        raiseParsingError(tag, attribute, strValue);
    }
    return doubleValue;
}
double VehicleEngineHandler::parsePolynomialCoefficient(uint8_t index, const XERCES_CPP_NAMESPACE::Attributes& attrs) {
    std::stringstream ss;
    ss << "x" << (int)index;
    return parseDoubleAttribute(ENGINE_TAG_ENGINE_POWER, ss.str().c_str(), attrs);
}
void VehicleEngineHandler::raiseError(std::string error) {
    std::cerr << error << std::endl;
    exit(1);
}
void VehicleEngineHandler::raiseParsingError(std::string tag, std::string attribute, std::string value) {
    std::stringstream ss;
    ss << "Invalid value \"" << value << "\" for attribute " << attribute << ", inside tag " << tag;
    raiseError(ss.str());
}
void VehicleEngineHandler::raiseMissingAttributeError(std::string tag, std::string attribute) {
    std::stringstream ss;
    ss << "Missing attribute \"" << attribute << "\" for tag " << tag;
    raiseError(ss.str());
}
void VehicleEngineHandler::raiseUnknownTagError(std::string tag) {
    std::stringstream ss;
    ss << "I don't know what to do with this tag: " << tag;
    raiseError(ss.str());
}

bool VehicleEngineHandler::toInt(std::string val, int& value) {
    if (sscanf(val.c_str(), "%d", &value) != 1) {
        return false;
    }
    return true;
}
bool VehicleEngineHandler::toDouble(std::string val, double& value) {
    if (sscanf(val.c_str(), "%lf", &value) != 1) {
        return false;
    }
    return true;
}

/****************************************************************************/

