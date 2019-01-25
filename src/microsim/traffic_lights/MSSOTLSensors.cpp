/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLSensors.cpp
/// @author  Gianfilippo Slager
/// @date    Feb 2010
/// @version $Id$
///
// The base abstract class for SOTL sensors
/****************************************************************************/
#include "MSSOTLSensors.h"

MSSOTLSensors::MSSOTLSensors(std::string tlLogicID, const MSTrafficLightLogic::Phases* phases) {
    this->tlLogicID = tlLogicID;
    this->myPhases = phases;
}

//MSSOTLSensors does not handle directly any data structure
MSSOTLSensors::~MSSOTLSensors() {}


/****************************************************************************/
