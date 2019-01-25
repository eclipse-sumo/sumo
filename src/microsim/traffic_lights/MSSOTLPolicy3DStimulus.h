/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPolicy3DStimulus.h
/// @author  Riccardo Belletti
/// @date    Mar 2014
/// @version $Id$
///
// The class the low-level policy stimulus
/****************************************************************************/

#ifndef MSSOTLPOLICY3DSTIMULUS_H_
#define MSSOTLPOLICY3DSTIMULUS_H_

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

//#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include <utils/common/MsgHandler.h>
#include <sstream>
#include <cmath>
#include "MSSOTLPolicy5DStimulus.h"

/**
 * \class MSSOTLPolicyStimulus
 * \brief This class determines the stimulus of a MSSOTLPolicy when
 * used in combination with a high level policy.\n
 * The stimulus function is calculated as follows:\n
 * stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor)
 */
class MSSOTLPolicy3DStimulus: public MSSOTLPolicy5DStimulus {

public:

    MSSOTLPolicy3DStimulus(std::string keyPrefix, const std::map<std::string, std::string>& parameters);

};

#endif /* MSSOTLPOLICYSTIMULUS_H_ */
