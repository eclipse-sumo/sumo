/****************************************************************************/
/// @file    MSSOTLPolicy5DListStimulus.h
/// @author  Riccardo Belletti
/// @author  Simone Bacchilega
/// @date    Aug 2014
/// @version $Id: MSSOTLPolicyDesirability.h 0  $
///
// The class the low-level policy stimulus
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef MSSOTLPOLICY5DFAMILYSTIMULUS_H_
#define MSSOTLPOLICY5DFAMILYSTIMULUS_H_

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
#include <stdlib.h>
#include <string.h>
#include "MSSOTLPolicy5DStimulus.h"

using namespace std;
/**
 * \class MSSOTLPolicy5DListStimulus
 * \brief This class determines the stimulus of a MSSOTLPolicy when
 * used in combination with a high level policy.\n
 * The stimulus function is calculated as follows:\n
 * stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor)
 */
class MSSOTLPolicy5DFamilyStimulus: public MSSOTLPolicyDesirability {

private:

	/*double stimCoxDVal,
	stimOffsetInDVal, stimOffsetOutDVal, stimOffsetDispersionInDVal, stimOffsetDispersionOutDVal,
	stimDivInDVal, stimDivOutDVal, stimDivDispersionInDVal, stimDivDispersionOutDVal,
	stimCoxExpInDVal, stimCoxExpOutDVal, stimCoxExpDispersionInDVal, stimCoxExpDispersionOutDVal;*/

	map<string,string> default_values;
	vector<string> params_names;
	vector<MSSOTLPolicy5DStimulus*> family;

	//std::vector<std::string> inline StringSplit(const std::string &source, const char *delimiter = " ", bool keepEmpty = false);

public:

	MSSOTLPolicy5DFamilyStimulus(string keyPrefix,const std::map<std::string, std::string>& parameters);

	string getMessage();

	vector<MSSOTLPolicy5DStimulus*> getFamilies(){
		return family;
	}

	/*
	 *	@brief Computes stimulus function
	 *  stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor);
	 */
	virtual double computeDesirability(double vehInMeasure,
			double vehOutMeasure);

	virtual double computeDesirability(double vehInMeasure, double vehOutMeasure,double vehInDispersionMeasure,	double vehOutDispersionMeasure);
};

#endif /* MSSOTLPOLICY5DFAMILYSTIMULUS_H_ */
