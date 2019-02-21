/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FirstOrderLagModel.cpp
/// @author  Michele Segata
/// @date    4 Feb 2015
/// @version $Id$
///
// An engine model using a first order lag
/****************************************************************************/

#include "FirstOrderLagModel.h"
#include <microsim/cfmodels/CC_Const.h>
#include "utils/common/StdDefs.h"
#include <algorithm>

FirstOrderLagModel::FirstOrderLagModel() : GenericEngineModel(),
    tau_s(0.5), dt_s(0.01) {
    className = "FirstOrderLagModel";
    computeParameters();
}
FirstOrderLagModel::~FirstOrderLagModel() {}

void FirstOrderLagModel::computeParameters() {
    alpha = dt_s / (tau_s + dt_s);
    oneMinusAlpha = 1 - alpha;
}

double FirstOrderLagModel::getRealAcceleration(double speed_mps, double accel_mps2, double reqAccel_mps2, SUMOTime timeStep) {
    UNUSED_PARAMETER(speed_mps);
    UNUSED_PARAMETER(timeStep);
    return std::min(
               maxAcceleration_mpsps,
               std::max(
                   -maxDeceleration_mpsps,
                   alpha * reqAccel_mps2 + oneMinusAlpha * accel_mps2
               )
           );
}

void FirstOrderLagModel::loadParameters(const ParMap& parameters) {
    parseParameter(parameters, std::string(FOLM_PAR_TAU), tau_s);
    parseParameter(parameters, std::string(FOLM_PAR_DT), dt_s);
    computeParameters();
}

void FirstOrderLagModel::setParameter(const std::string parameter, const std::string& value) {
    UNUSED_PARAMETER(parameter);
    UNUSED_PARAMETER(value);
}
void FirstOrderLagModel::setParameter(const std::string parameter, double value) {
    if (parameter == FOLM_PAR_TAU) {
        tau_s = value;
    }
    if (parameter == FOLM_PAR_DT) {
        dt_s = value;
    }
    computeParameters();
}
void FirstOrderLagModel::setParameter(const std::string parameter, int value) {
    UNUSED_PARAMETER(parameter);
    UNUSED_PARAMETER(value);
}
