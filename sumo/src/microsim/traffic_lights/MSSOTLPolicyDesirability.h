/****************************************************************************/
/// @file    MSSOTLPolicyDesirability.h
/// @author  Riccardo Belletti
/// @date    Mar 2014
/// @version $Id: MSSOTLPolicyDesirability.h 0  $
///
// The class a low-level policy desirability
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

#ifndef MSSOTLPOLICYDESIRABILITY_H_
#define MSSOTLPOLICYDESIRABILITY_H_

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
#include <sstream>
#include <utils/common/Parameterised.h>

using namespace std;
/**
 * \class MSSOTLPolicyDesirability
 * \brief This class determines the desirability algorithm of a MSSOTLPolicy when
 * used in combination with a high level policy
 */
class MSSOTLPolicyDesirability: public Parameterised {

private:
	string myKeyPrefix;

protected:
	double s2f(string str) {
		istringstream buffer(str);
		double temp;
		buffer >> temp;
		return temp;
	}
	double readParameter(string parName, double defValue) {
		std::ostringstream key;
		key << parName;
		std::ostringstream def;
		def << defValue;
		return s2f(getParameter(key.str(), def.str()));
	}

public:

	MSSOTLPolicyDesirability(string keyPrefix,
			const std::map<std::string, std::string>& parameters);
	virtual ~MSSOTLPolicyDesirability();

	/**
	 * \brief Calculates the desirability of the policy
	 */
	virtual double computeDesirability(double vehInMeasure,
			double vehOutMeasure) = 0;
	/**
	 * \brief Calculates the desirability of the policy
	 */
	virtual double computeDesirability(double vehInMeasure, double vehOutMeasure,
			double vehInDispersionMeasure,
			double vehOutDispersionMeasure) = 0;
	virtual string getMessage() = 0;
	void setKeyPrefix(string val) {
		myKeyPrefix = val;
	}
	string getKeyPrefix() {
		return myKeyPrefix;
	}

};

#endif /* MSSOTLPOLICYDESIRABILITY_H_ */
