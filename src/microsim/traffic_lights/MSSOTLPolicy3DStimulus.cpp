/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPolicy3DStimulus.cpp
/// @author  Riccardo Belletti
/// @date    Mar 2014
/// @version $Id$
///
// The class for Swarm-based low-level policy
/****************************************************************************/

#include "MSSOTLPolicy3DStimulus.h"

MSSOTLPolicy3DStimulus::MSSOTLPolicy3DStimulus(std::string keyPrefix,
        const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicy5DStimulus(keyPrefix, parameters) {

    setStimCoxExpDispersionInDefVal(0);
    setStimCoxExpDispersionOutDefVal(0);
}

