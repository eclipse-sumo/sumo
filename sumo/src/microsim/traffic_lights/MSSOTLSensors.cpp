/****************************************************************************/
/// @file    MSSOTLSensors.cpp
/// @author  Gianfilippo Slager
/// @date    Feb 2010
/// @version $Id$
///
// The base abstract class for SOTL sensors
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include "MSSOTLSensors.h"

MSSOTLSensors::MSSOTLSensors(std::string tlLogicID, const MSTrafficLightLogic::Phases* phases) {
    this->tlLogicID = tlLogicID;
    this->myPhases = phases;
}

//MSSOTLSensors does not handle directly any data structure
MSSOTLSensors::~MSSOTLSensors() {}


/****************************************************************************/
