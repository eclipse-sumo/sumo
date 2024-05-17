/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    LinearApproxHelpers.cpp
/// @author  Mirko Barthauer
/// @date    17 May 2024
///
// Provides a tabular data points map with parsing and interpolation support
/****************************************************************************/
#include <config.h>

#include <string>
#include <map>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include "LinearApproxHelpers.h"


double
LinearApproxHelpers::getMinimumValue(const LinearApproxMap& map) {
    if (map.empty()) {
        throw ProcessError(TL("Cannot determine the minimum value from an empty map."));
    }
    auto it = std::min_element(map.begin(), map.end(), [](const auto & x, const auto & y) {
        return x.second < y.second;
    });
    return it->second;
}


double
LinearApproxHelpers::getMaximumValue(const LinearApproxMap& map) {
    if (map.empty()) {
        throw ProcessError(TL("Cannot determine the maximum value from an empty map."));
    }
    auto it = std::max_element(map.begin(), map.end(), [](const auto & x, const auto & y) {
        return x.second < y.second;
    });
    return it->second;
}


double
LinearApproxHelpers::getInterpolatedValue(const LinearApproxMap& map, double axisValue) {
    LinearApproxHelpers::LinearApproxMap::const_iterator low, prev;
    low = map.lower_bound(axisValue);
    if (low == map.end()) {
        return (map.rbegin())->second;
    }
    if (low == map.begin()) {
        return low->second;
    }
    prev = low;
    --prev;
    double range = low->first - prev->first;
    double dist = axisValue - prev->first;
    assert(range > 0);
    assert(dist > 0);
    double weight = dist / range;
    double res = (1 - weight) * prev->second + weight * low->second;
    return res;
}


std::vector<double>
LinearApproxHelpers::getValueTable(const std::string& dataString) {
    std::vector<double> result;
    if (!dataString.empty()) {
        for (std::string value : StringTokenizer(dataString).getVector()) {
            result.push_back(StringUtils::toDouble(value));
        }
    }
    return result;
}


bool
LinearApproxHelpers::setPoints(LinearApproxMap& map, const std::string& axisString, const std::string& heightString) {
    std::vector<double> axisData = getValueTable(axisString);
    std::vector<double> heightData = getValueTable(heightString);
    if (heightData.size() > 0 && heightData.size() != axisData.size()) {
        throw ProcessError(TLF("Mismatching data rows of % axis and % height values.", axisData.size(), heightData.size()));
    } else {
        auto itA = axisData.begin();
        auto itB = heightData.begin();
        for (; itA != axisData.end() && itB != heightData.end(); ++itA, ++itB) {
            map.insert({ *itA, *itB });
        }
    }
    return true;
}


void
LinearApproxHelpers::scalePoints(LinearApproxMap& map, double keyFactor, double valueFactor) {
    LinearApproxMap map2;
    for (auto item : map) {
        map2[item.first * keyFactor] = item.second * valueFactor;
    }
    map.clear();
    map.insert(map2.begin(), map2.end());
}


void
LinearApproxHelpers::scaleValues(LinearApproxMap& map, const double factor) {
    for (auto& p : map) {
        p.second *= factor;
    }
}


void LinearApproxHelpers::setValues(LinearApproxMap& map, const std::string& heightString) {
    std::vector<double> heightData = getValueTable(heightString);
    if (heightData.size() > 0 && heightData.size() != map.size()) {
        throw ProcessError(TLF("Mismatching data rows of % axis and % height values.", map.size(), heightData.size()));
    } else {
        std::vector<double>::const_iterator heightIt = heightData.begin();
        for (auto& p : map) {
            p.second = *heightIt;
            ++heightIt;
        }
    }
}
