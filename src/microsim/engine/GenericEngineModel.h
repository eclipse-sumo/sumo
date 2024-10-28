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
/// @file    GenericEngineModel.h
/// @author  Michele Segata
/// @date    4 Feb 2015
///
// Generic interface for an engine model
/****************************************************************************/

#pragma once
#include <config.h>

#include <map>
#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/common/Parameterised.h>

/**
 * This is an interface for plexe engine models. It provides two virtual methods
 * that should be overridden by implementing classes: getRealAcceleration and
 * loadParameters
 */
class GenericEngineModel {

public:
    /// @brief constructor
    GenericEngineModel() : maxAcceleration_mpsps(1.5), maxDeceleration_mpsps(7) {};

    /// @brief destructor
    virtual ~GenericEngineModel() {};

    /**
     * Computes real vehicle acceleration given current speed, current acceleration,
     * and requested acceleration. Acceleration can be negative as well. The
     * model should handle decelerations as well
     *
     * @param[in] speed_mps current speed in meters per second
     * @param[in] accel_mps2 current acceleration in meters per squared second
     * @param[in] reqAccel_mps2 requested acceleration in meters per squared second
     * @param[in] timeStep current simulation timestep
     * @return the real acceleration that the vehicle applies in meters per
     * squared second
     */
    virtual double getRealAcceleration(double speed_mps, double accel_mps2, double reqAccel_mps2, SUMOTime timeStep = 0) = 0;

    /**
     * Sets a single parameter value
     *
     * @param[in] parameter the name of the parameter
     * @param[in] value the value for the parameter
     */
    virtual void setParameter(const std::string parameter, const std::string& value) = 0;
    virtual void setParameter(const std::string parameter, double value) = 0;
    virtual void setParameter(const std::string parameter, int value) = 0;

    /**
     * Sets maximum acceleration value
     *
     * @param[in] maximum acceleration in meters per second squared
     */
    void setMaximumAcceleration(double maxAcc);
    /**
     * Sets maximum deceleration value
     *
     * @param[in] maximum deceleration (positive value) in meters per second
     * squared
     */
    void setMaximumDeceleration(double maxDec);

protected:
    //class name, used to log information
    std::string className;

    //minimum and maximum acceleration of the model, if any
    double maxAcceleration_mpsps, maxDeceleration_mpsps;

    /**
     * Prints a parameter error
     */
    void printParameterError(std::string parameter, std::string value);

    /**
     * Parses a value from the parameter map
     */
    void parseParameter(const Parameterised::Map& parameters, std::string parameter, double& value);
    void parseParameter(const Parameterised::Map& parameters, std::string parameter, int& value);
    void parseParameter(const Parameterised::Map& parameters, std::string parameter, std::string& value);
};
