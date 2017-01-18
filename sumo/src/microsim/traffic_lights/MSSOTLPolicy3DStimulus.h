/****************************************************************************/
/// @file    MSSOTLPolicyStimulus.h
/// @author  Riccardo Belletti
/// @date    Mar 2014
/// @version $Id$
///
// The class the low-level policy stimulus
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef MSSOTLPOLICY3DSTIMULUS_H_
#define MSSOTLPOLICY3DSTIMULUS_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include <utils/common/MsgHandler.h>
#include <sstream>
#include <math.h>
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
