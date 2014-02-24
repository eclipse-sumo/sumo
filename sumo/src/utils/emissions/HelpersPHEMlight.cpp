/****************************************************************************/
/// @file    HelpersPHEMlight.cpp
/// @author  Daniel Krajzewicz
/// @date    Sat, 20.04.2013
/// @version $Id: HelpersPHEMlight.cpp 13107 2012-12-02 13:57:34Z behrisch $
///
// Helper methods for PHEMlight-based emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

#include "HelpersPHEMlight.h"
#include "PHEMCEPHandler.h"
#include "PHEMConstants.h"
#include <limits>
#include <cmath>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
HelpersPHEMlight::getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) {
    return -1;
}


SUMOReal
HelpersPHEMlight::computeCO(SUMOEmissionClass c, double v, double a, double slope) {
	PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
	double power = currCep->CalcPower(v, a, slope);
	return currCep->GetEmission("CO", power) / SECONDS_PER_HOUR * 1000.;
}


SUMOReal
HelpersPHEMlight::computeCO2(SUMOEmissionClass c, double v, double a, double slope) {
	PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
	double power = currCep->CalcPower(v, a, slope);
	return currCep->GetEmission("FC", power) * 3.15 / SECONDS_PER_HOUR * 1000.;
}


SUMOReal
HelpersPHEMlight::computeHC(SUMOEmissionClass c, double v, double a, double slope) {
	PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
	double power = currCep->CalcPower(v, a, slope);
	return currCep->GetEmission("HC", power) / SECONDS_PER_HOUR * 1000.;
} 


SUMOReal
HelpersPHEMlight::computeNOx(SUMOEmissionClass c, double v, double a, double slope) {
	PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
	double power = currCep->CalcPower(v, a, slope);
	return currCep->GetEmission("NOx", power) / SECONDS_PER_HOUR * 1000.;
}


SUMOReal
HelpersPHEMlight::computePMx(SUMOEmissionClass c, double v, double a, double slope) {
	PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
	double power = currCep->CalcPower(v, a, slope);
	return currCep->GetEmission("PM", power) / SECONDS_PER_HOUR * 1000.;
}


SUMOReal
HelpersPHEMlight::computeFuel(SUMOEmissionClass c, double v, double a, double slope) {
	PHEMCEP* currCep = PHEMCEPHandler::getHandlerInstance().GetCep(c);
	double power = currCep->CalcPower(v, a, slope);
	
	if(currCep->GetVehicleFuelType() == "D")
		return currCep->GetEmission("FC", power) / 0.836 / SECONDS_PER_HOUR * 1000.;
	else
		return currCep->GetEmission("FC", power) / 0.742 / SECONDS_PER_HOUR * 1000.;
}

/****************************************************************************/

