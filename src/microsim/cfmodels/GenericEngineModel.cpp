/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GenericEngineModel.cpp
/// @author  Michele Segata
/// @date    4 Feb 2015
/// @version $Id$
///
// Generic interface for an engine model
/****************************************************************************/

#include "GenericEngineModel.h"

#include <iostream>
#include <stdio.h>

void GenericEngineModel::printParameterError(std::string parameter, std::string value) {
    std::cerr << className << ": invalid value " << value << " for parameter " << parameter << std::endl;
}

void GenericEngineModel::parseParameter(const ParMap & parameters, std::string parameter, double &value) {
    ParMap::const_iterator par = parameters.find(parameter);
    double v;
    if (par != parameters.end()) {
        if (sscanf(par->second.c_str(), "%lf", &v) != 1)
            printParameterError(par->first, par->second);
        else
            value = v;
    }
}
void GenericEngineModel::parseParameter(const ParMap & parameters, std::string parameter, int &value) {
    ParMap::const_iterator par = parameters.find(parameter);
    int v;
    if (par != parameters.end()) {
        if (sscanf(par->second.c_str(), "%d", &v) != 1)
            printParameterError(par->first, par->second);
        else
            value = v;
    }
}
void GenericEngineModel::parseParameter(const ParMap & parameters, std::string parameter, std::string &value) {
    ParMap::const_iterator par = parameters.find(parameter);
    if (par != parameters.end())
        value = par->second;
}
void GenericEngineModel::setMaximumAcceleration(double maxAcceleration_mpsps) {
    this->maxAcceleration_mpsps = maxAcceleration_mpsps;
}
void GenericEngineModel::setMaximumDeceleration(double maxDeceleration_mpsps) {
    this->maxDeceleration_mpsps = maxDeceleration_mpsps;
}
double GenericEngineModel::setMaximumAcceleration() {
    return maxAcceleration_mpsps;
}
double GenericEngineModel::setMaximumDeceleration() {
    return maxDeceleration_mpsps;
}
