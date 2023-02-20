/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
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
/// @file    libsumocpp2c.cpp
/// @author  Robert Hilbrich
/// @author  Matthias Schwamborn
/// @date    Mon, 17 Aug 2020
///
// Implementation of the libsumo c++ to c wrapper
/****************************************************************************/

#ifdef _MSC_VER
// Avoid warnings in windows build because of strcpy instead of strcpy_s,
// because the latter is not available on all platforms
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4820 4514 5045 4668 4710)
#endif

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/Simulation.h>
#include <libsumo/Vehicle.h>

#include "libsumocpp2c.h"

#define DELIMITER ' '

inline char*
allocateAndCopyString(ModelInstance* comp, const std::string& s) {
    char* buf = NULL;
    buf = (char*)comp->allocateMemory(1 + s.length(), sizeof(char));
    s.copy(buf, 1 + s.length());
    return buf;
}

void
libsumo_load(char* callOptions) {
    // Tokenize the string, because Simulation::load expects a vector
    std::vector<std::string> options;
    std::stringstream ss(callOptions);
    std::string temp_str;
    while (std::getline(ss, temp_str, DELIMITER)) {
        options.push_back(temp_str);
    }

    try {
        libsumo::Simulation::load(options);
    } catch (const libsumo::TraCIException& e) {
        std::cerr << "libsumo::Simulation::load() failed - reason: " << e.what() << std::endl;
        abort();
    }
}

void
libsumo_close(void) {
    libsumo::Simulation::close();
}

void
libsumo_step(double time) {
    libsumo::Simulation::step(time);
}

int
libsumo_vehicle_getIDCount(void) {
    return libsumo::Vehicle::getIDCount();
}

void
libsumo_vehicle_moveToXY(const char* paramString) {
    try {
        std::vector<std::string> params;
        std::stringstream ss(paramString);
        std::string temp_str;
        while (std::getline(ss, temp_str, DELIMITER)) {
            params.push_back(temp_str);
        }
        char* pEnd;
        const std::string vehID = params[0];
        const std::string edgeID = params[1];
        int laneIndex = strtol(params[2].c_str(), &pEnd, 10);
        double x = strtod(params[3].c_str(), &pEnd);
        double y = strtod(params[4].c_str(), &pEnd);
        double angle = params.size() >= 6 ? strtod(params[5].c_str(), &pEnd) : libsumo::INVALID_DOUBLE_VALUE;
        int keepRoute = params.size() >= 7 ? strtol(params[6].c_str(), &pEnd, 10) : 1;
        double matchThreshold = params.size() >= 8 ? strtod(params[7].c_str(), &pEnd) : 100;
        libsumo::Vehicle::moveToXY(vehID, edgeID, laneIndex, x, y, angle, keepRoute, matchThreshold);
    } catch (const std::runtime_error& e) {
        std::cerr << "libsumo::Vehicle::moveToXY() failed - reason: " << e.what() << std::endl;
        abort();
    }
}

void
libsumo_vehicle_getParameterWithKey(ModelInstance* comp, const char** result) {
    try {
        std::vector<std::string> params;
        std::stringstream ss(comp->getterParameters);
        std::string temp_str;
        while (std::getline(ss, temp_str, DELIMITER)) {
            params.push_back(temp_str);
        }

        const std::string vehID = params[0];
        const std::string key = params[1];
        std::pair<std::string, std::string> p = libsumo::Vehicle::getParameterWithKey(vehID, key);
        const std::string resultString = p.first + DELIMITER + p.second;
        *result = allocateAndCopyString(comp, resultString);
    } catch (const std::runtime_error& e) {
        std::cerr << "libsumo::Vehicle::getParameterWithKey() failed - reason: " << e.what() << std::endl;
        abort();
    }
}

void
libsumo_vehicle_getLaneID(ModelInstance* comp, const char** result) {
    try {
        std::vector<std::string> params;
        std::stringstream ss(comp->getterParameters);
        std::string temp_str;
        while (std::getline(ss, temp_str, DELIMITER)) {
            params.push_back(temp_str);
        }

        const std::string vehID = params[0];
        *result = allocateAndCopyString(comp, libsumo::Vehicle::getLaneID(vehID));
    } catch (const std::runtime_error& e) {
        std::cerr << "libsumo::Vehicle::getLaneID() failed - reason: " << e.what() << std::endl;
        abort();
    }
}

void
libsumo_vehicle_getPosition(ModelInstance* comp, const char** result) {
    try {
        std::vector<std::string> params;
        std::stringstream ss(comp->getterParameters);
        std::string temp_str;
        while (std::getline(ss, temp_str, DELIMITER)) {
            params.push_back(temp_str);
        }

        const std::string vehID = params[0];
        libsumo::TraCIPosition pos = libsumo::Vehicle::getPosition(vehID);
        std::ostringstream os;
        os << pos.x << DELIMITER << pos.y;
        *result = allocateAndCopyString(comp, os.str());
    } catch (const std::runtime_error& e) {
        std::cerr << "libsumo::Vehicle::getPosition() failed - reason: " << e.what() << std::endl;
        abort();
    }
}

