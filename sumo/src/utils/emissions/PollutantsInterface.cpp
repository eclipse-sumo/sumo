/****************************************************************************/
/// @file    PollutantsInterface.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 19.08.2013
/// @version $Id$
///
// Interface to capsulate different emission models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <limits>
#include <cmath>
#include <utils/common/SUMOVehicleClass.h>
#include "PollutantsInterface.h"
#include "HelpersHBEFA.h"
#include "HelpersPHEMlight.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
PollutantsInterface::getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return -1;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::getMaxAccel(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOEmissionClass
PollutantsInterface::getClass(SUMOEmissionClass base, std::string& vClass, std::string& fuel, std::string& eClass, double weight) {
    if (base > SVE_META_HBEFA21_END && base < SVE_META_PHEMLIGHT_END) {
        int eClassOffset = 0;
        if (eClass.length() == 5 && eClass.substr(0, 4) == "Euro") {
            if (eClass[4] >= '0' && eClass[4] <= '6') {
                eClassOffset = eClass[4] - '0';
            }
        }
        if (vClass == "Passenger") {
            if (fuel == "Gasoline") {
                return (SUMOEmissionClass)(SVE_PKW_G_EU0 + eClassOffset);
            } else if (fuel == "Diesel") {
                return (SUMOEmissionClass)(SVE_PKW_D_EU0 + eClassOffset);
            } else if (fuel == "HybridGasoline") {
                return (SUMOEmissionClass)(SVE_H_PKW_G_EU5 + (eClassOffset > 5));
            } else if (fuel == "HybridDiesel") {
                return (SUMOEmissionClass)(SVE_H_PKW_D_EU5 + (eClassOffset > 5));
            }
        } else if (vClass == "TwoWheeler") {
            return (SUMOEmissionClass)(SVE_KKR_G_EU0 + MIN2(eClassOffset, 4));
        } else if (vClass == "Motorcycle") {
            return (SUMOEmissionClass)(SVE_MR_G_EU0_2T + 2 * MIN2(eClassOffset, 5));
        } else if (vClass == "Delivery") {
            int offset = 3 * eClassOffset;
            if (weight > 1305.) {
                offset++;
                if (weight > 1760.) {
                    offset++;
                }
            }
            if (fuel == "Gasoline") {
                return (SUMOEmissionClass)(SVE_LNF_G_EU0_I + offset);
            } else if (fuel == "Diesel") {
                return (SUMOEmissionClass)(SVE_LNF_D_EU0_I + offset);
            }
        } else if (vClass == "UrbanBus") {
            return (SUMOEmissionClass)(SVE_LB_D_EU0 + eClassOffset);
        } else if (vClass == "Coach") {
            return (SUMOEmissionClass)(SVE_RB_D_EU0 + eClassOffset);
        } else if (vClass == "Truck") {
            return (SUMOEmissionClass)(SVE_Solo_LKW_D_EU0_I + 2 * eClassOffset + (weight > 10000.));
        } else if (vClass == "Trailer") {
            return (SUMOEmissionClass)(SVE_LSZ_D_EU0 + eClassOffset);
        }
    }
    return base;
}


std::string
PollutantsInterface::getAmitranVehicleClass(SUMOEmissionClass c) {
    if ((c >= SVE_H_PKW_D_EU5 && c <= SVE_H_PKW_G_EU6) || (c >= SVE_PKW_D_EU0 && c <= SVE_PKW_G_EU6)) {
        return "Passenger";
    } else if (c >= SVE_KKR_G_EU0 && c <= SVE_KKR_G_EU4) {
        return "TwoWheeler";
    } else if (c >= SVE_RB_D_EU0 && c <= SVE_RB_D_EU6) {
        return "Coach";
    } else if (c >= SVE_LB_D_EU0 && c <= SVE_LB_D_EU6) {
        return "UrbanBus";
    } else if (c >= SVE_LNF_D_EU0_I && c <= SVE_LNF_G_EU6_III) {
        return "Delivery";
    } else if (c >= SVE_LSZ_D_EU0 && c <= SVE_LSZ_D_EU6) {
        return "Trailer";
    } else if (c >= SVE_MR_G_EU0_2T && c <= SVE_MR_G_EU5_4T) {
        return "Motorcycle";
    } else if (c >= SVE_Solo_LKW_D_EU0_I && c <= SVE_Solo_LKW_D_EU6_II) {
        return "Truck";
    }
}


std::string
PollutantsInterface::getFuel(SUMOEmissionClass c) {
    const std::string name = getVehicleEmissionTypeName(c);
    std::string fuel = "Gasoline";
    if (name.find("_D_") != std::string::npos) {
        fuel = "Diesel";
    }
    if (name.find("_H_") != std::string::npos) {
        fuel = "Hybrid" + fuel;
    }
    return fuel;
}


int
PollutantsInterface::getEuroClass(SUMOEmissionClass c) {
    const std::string name = getVehicleEmissionTypeName(c);
    if (name.find("_EU1") != std::string::npos) {
        return 1;
    } else if (name.find("_EU2") != std::string::npos) {
        return 2;
    } else if (name.find("_EU3") != std::string::npos) {
        return 3;
    } else if (name.find("_EU4") != std::string::npos) {
        return 4;
    } else if (name.find("_EU5") != std::string::npos) {
        return 5;
    } else if (name.find("_EU6") != std::string::npos) {
        return 6;
    }
    return 0;
}


SUMOReal
PollutantsInterface::getWeight(SUMOEmissionClass c) {
    const std::string name = getVehicleEmissionTypeName(c);
    if (name.find("LNF_") != std::string::npos) {
        if (name.find("_III") != std::string::npos) {
            return 2630.;
        } else if (name.find("_II") != std::string::npos) {
            return 1532.;
        } else if (name.find("_I") != std::string::npos) {
            return 652.;
        }
    }
    if (name.find("Solo_LKW_") != std::string::npos) {
        if (name.find("_II") != std::string::npos) {
            return 8398.;
        } else if (name.find("_I") != std::string::npos) {
            return 18702.;
        }
    }
    return 0.;
}


SUMOReal
PollutantsInterface::computeCO(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeCO(c, v, a);
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeCO(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeCO2(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeCO2(c, v, a);
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeCO2(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeHC(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeHC(c, v, a);
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeHC(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeNOx(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeNOx(c, v, a);
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeNOx(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computePMx(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computePMx(c, v, a);
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computePMx(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeFuel(SUMOEmissionClass c, double v, double a, double slope) {
    if (c < SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeFuel(c, v, a);
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeFuel(c, v, a, slope);
    } else {
        return 0;
    }
}




SUMOReal
PollutantsInterface::computeDefaultCO(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if (c < SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeCO(c, v, 0) + HelpersHBEFA::computeCO(c, v - a, a)) * tt / 2.;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeCO(c, v, 0, slope) + HelpersPHEMlight::computeCO(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultCO2(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if (c < SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeCO2(c, v, 0) + HelpersHBEFA::computeCO2(c, v - a, a)) * tt / 2.;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeCO2(c, v, 0, slope) + HelpersPHEMlight::computeCO2(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultHC(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if (c < SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeHC(c, v, 0) + HelpersHBEFA::computeHC(c, v - a, a)) * tt / 2.;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeHC(c, v, 0, slope) + HelpersPHEMlight::computeHC(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultNOx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if (c < SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeNOx(c, v, 0) + HelpersHBEFA::computeNOx(c, v - a, a)) * tt / 2.;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeNOx(c, v, 0, slope) + HelpersPHEMlight::computeNOx(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultPMx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if (c < SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computePMx(c, v, 0) + HelpersHBEFA::computePMx(c, v - a, a)) * tt / 2.;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computePMx(c, v, 0, slope) + HelpersPHEMlight::computePMx(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultFuel(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if (c < SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeFuel(c, v, 0) + HelpersHBEFA::computeFuel(c, v - a, a)) * tt / 2.;
    } else if (c > SVE_META_HBEFA21_END && c < SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeFuel(c, v, 0, slope) + HelpersPHEMlight::computeFuel(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


/****************************************************************************/

