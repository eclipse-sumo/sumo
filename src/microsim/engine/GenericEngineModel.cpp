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
/// @file    GenericEngineModel.cpp
/// @author  Michele Segata
/// @date    4 Feb 2015
///
// Generic interface for an engine model
/****************************************************************************/

#include <config.h>

#include <iostream>
#include <utils/common/StringUtils.h>
#include "GenericEngineModel.h"


void GenericEngineModel::printParameterError(std::string parameter, std::string value) {
    std::cerr << className << ": invalid value " << value << " for parameter " << parameter << std::endl;
}

void GenericEngineModel::parseParameter(const Parameterised::Map& parameters, std::string parameter, double& value) {
    Parameterised::Map::const_iterator par = parameters.find(parameter);
    if (par != parameters.end()) {
        try {
            value = StringUtils::toDouble(par->second);
        } catch (ProcessError&) {
            printParameterError(par->first, par->second);
        }
    }
}
void GenericEngineModel::parseParameter(const Parameterised::Map& parameters, std::string parameter, int& value) {
    Parameterised::Map::const_iterator par = parameters.find(parameter);
    if (par != parameters.end()) {
        try {
            value = StringUtils::toInt(par->second);
        } catch (ProcessError&) {
            printParameterError(par->first, par->second);
        }
    }
}
void GenericEngineModel::parseParameter(const Parameterised::Map& parameters, std::string parameter, std::string& value) {
    Parameterised::Map::const_iterator par = parameters.find(parameter);
    if (par != parameters.end()) {
        value = par->second;
    }
}
void GenericEngineModel::setMaximumAcceleration(double maxAcc) {
    maxAcceleration_mpsps = maxAcc;
}
void GenericEngineModel::setMaximumDeceleration(double maxDec) {
    maxDeceleration_mpsps = maxDec;
}
