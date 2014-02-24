/****************************************************************************/
/// @file    PollutantsInterface.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 19.08.2013
/// @version $Id: PollutantsInterface.cpp 13107 2012-12-02 13:57:34Z behrisch $
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

#include "PollutantsInterface.h"
#include "HelpersHBEFA.h"
#include "HelpersPHEMlight.h"
#include <limits>
#include <cmath>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
PollutantsInterface::getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return -1;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::getMaxAccel(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeCO(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeCO(c, v, a);
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeCO(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeCO2(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeCO2(c, v, a);
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeCO2(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeHC(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeHC(c, v, a);
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeHC(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeNOx(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeNOx(c, v, a);
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeNOx(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computePMx(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computePMx(c, v, a);
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computePMx(c, v, a, slope);
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeFuel(SUMOEmissionClass c, double v, double a, double slope) {
    if(c<SVE_META_HBEFA21_END) {
        return HelpersHBEFA::computeFuel(c, v, a);
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return HelpersPHEMlight::computeFuel(c, v, a, slope);
    } else {
        return 0;
    }
}




SUMOReal
PollutantsInterface::computeDefaultCO(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if(c<SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeCO(c, v, 0) + HelpersHBEFA::computeCO(c, v - a, a)) * tt / 2.;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeCO(c, v, 0, slope) + HelpersPHEMlight::computeCO(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultCO2(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if(c<SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeCO2(c, v, 0) + HelpersHBEFA::computeCO2(c, v - a, a)) * tt / 2.;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeCO2(c, v, 0, slope) + HelpersPHEMlight::computeCO2(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultHC(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if(c<SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeHC(c, v, 0) + HelpersHBEFA::computeHC(c, v - a, a)) * tt / 2.;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeHC(c, v, 0, slope) + HelpersPHEMlight::computeHC(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultNOx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if(c<SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeNOx(c, v, 0) + HelpersHBEFA::computeNOx(c, v - a, a)) * tt / 2.;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeNOx(c, v, 0, slope) + HelpersPHEMlight::computeNOx(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultPMx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if(c<SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computePMx(c, v, 0) + HelpersHBEFA::computePMx(c, v - a, a)) * tt / 2.;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computePMx(c, v, 0, slope) + HelpersPHEMlight::computePMx(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


SUMOReal
PollutantsInterface::computeDefaultFuel(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt) {
    if(c<SVE_META_HBEFA21_END) {
        return (HelpersHBEFA::computeFuel(c, v, 0) + HelpersHBEFA::computeFuel(c, v - a, a)) * tt / 2.;
    } else if(c>SVE_META_HBEFA21_END && c<SVE_META_PHEMLIGHT_END) {
        return (HelpersPHEMlight::computeFuel(c, v, 0, slope) + HelpersPHEMlight::computeFuel(c, v - a, a, slope)) * tt / 2.;
    } else {
        return 0;
    }
}


/****************************************************************************/

