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
/// @file    LinearApproxHelpers.h
/// @author  Mirko Barthauer
/// @date    17 May 2024
///
// Provides a tabular data points map with parsing and interpolation support
/****************************************************************************/
#pragma once
#include <config.h>
#include <map>
#include <string>
#include <vector>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LinearApproxHelpers
 * @brief A generic class to operate on LinearApproxMap instances
 */

class LinearApproxHelpers {
public:
    typedef std::map<double, double> LinearApproxMap;

    /**@brief Set data points
     * @param[in] axisString string of axis points
     * @param[in] heightString string of height data
     */
    static bool setPoints(LinearApproxMap& map, const std::string& axisString, const std::string& heightString);

    /**@brief Scale both key and values
     * @param[in] keyFactor to scale the key with
     * @param[in] valueFactor to scale the value with
     */
    static void scalePoints(LinearApproxMap& map, double keyFactor, double valueFactor);

    /**@brief Scale values
     * @param[in] factor with which to scale the values
     */
    static void scaleValues(LinearApproxMap& map, const double factor);

    /**@brief Set height values for existing axis values
     * @param[in] heightString string of height data
     */
    static void setValues(LinearApproxMap& map, const std::string& heightString);

    /**@brief Get the smallest height value
     * @return Minimum height value
     */
    static double getMinimumValue(const LinearApproxMap& map);

    /**@brief Get the largest height value
     * @return Maximum height value
     */
    static double getMaximumValue(const LinearApproxMap& map);

    /**@brief Get interpolated value
     * @param[in] axisValue axis value to get the interpolated data point for
     */
    static double getInterpolatedValue(const LinearApproxMap& map, double axisValue);

    /**@brief split string into data values
     * @param[in] dataString string containing row of double values
     */
    static std::vector<double> getValueTable(const std::string& dataString);
};
