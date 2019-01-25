/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    PHEMConstants.h
/// @author  Nikolaus Furian
/// @author  Daniel Krajzewicz
/// @date    Thu, 13.06.2013
/// @version $$
///
// Holds constants and data version info for PHEM Light
/****************************************************************************/

#ifndef PHEMConstants_h
#define PHEMConstants_h

const double GRAVITY_CONST = 9.81;
const double AIR_DENSITY_CONST = 1.182;
const double SECONDS_PER_HOUR = 3600;
const double NORMALIZING_SPEED = 19.444;
const double NORMALIZING_ACCELARATION = 0.45;
const double DENSITY_DIESEL = 1;
const double DENSITY_GASOLINE = 1;
const double SPEED_DCEL_MIN = 10 / 3.6;
const double M_PI2 = 3.14159265358979323846;
const double ZERO_SPEED_ACCURACY = 0.1;
const double SPEED_ROTATIONAL_INCREMENT = 10 / 3.6;
const double POWER_FC_INCREMENT = 0.1;
const double POWER_POLLUTANT_INCREMENT = 0.3;
const double NNORM_INCREMENT = 0.05;

const std::string PHEM_DATA_VERSION = "V5";

#endif
