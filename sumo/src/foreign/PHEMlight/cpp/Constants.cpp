/****************************************************************************/
/// @file    Constants.cpp
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors
// PHEMlight module
// Copyright (C) 2016-2017 Technische Universitaet Graz, https://www.tugraz.at/
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


#include "Constants.h"


namespace PHEMlightdll {

const double Constants::GRAVITY_CONST = 9.81;
const double Constants::AIR_DENSITY_CONST = 1.182;
const double Constants::NORMALIZING_SPEED = 19.444;
const double Constants::NORMALIZING_ACCELARATION = 0.45;
const double Constants::SPEED_DCEL_MIN = 10 / 3.6;
const double Constants::ZERO_SPEED_ACCURACY = 0.5;
const double Constants::DRIVE_TRAIN_EFFICIENCY_All = 0.9;
const double Constants::DRIVE_TRAIN_EFFICIENCY_CB = 0.8;
const std::string Constants::HeavyVehicle = "HV";
const std::string Constants::strPKW = "PC";
const std::string Constants::strLNF = "LCV";
const std::string Constants::strLKW = "HDV_RT";
const std::string Constants::strLSZ = "HDV_TT";
const std::string Constants::strRB = "HDV_CO";
const std::string Constants::strLB = "HDV_CB";
const std::string Constants::strMR2 = "MC_2S";
const std::string Constants::strMR4 = "MC_4S";
const std::string Constants::strKKR = "MOP";
const std::string Constants::strGasoline = "G";
const std::string Constants::strDiesel = "D";
const std::string Constants::strCNG = "CNG";
const std::string Constants::strLPG = "LPG";
const std::string Constants::strHybrid = "HEV";
const std::string Constants::strBEV = "BEV";
const std::string Constants::strEU = "EU";
const std::string Constants::strSI = "I";
const std::string Constants::strSII = "II";
const std::string Constants::strSIII = "III";
double Constants::_DRIVE_TRAIN_EFFICIENCY = 0;

    const double& Constants::getDRIVE_TRAIN_EFFICIENCY() {
        return _DRIVE_TRAIN_EFFICIENCY;
    }

    void Constants::setDRIVE_TRAIN_EFFICIENCY(const double& value) {
        _DRIVE_TRAIN_EFFICIENCY = value;
    }
}
