/****************************************************************************/
/// @file    MSSOTLPolicyDesirability.h
/// @author  Riccardo Belletti
/// @date    2014-03-20
/// @version $Id$
///
// The class a low-level policy desirability
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

#include <utils/common/SwarmDebug.h>
#include <sstream>
#include <utils/common/Parameterised.h>
#include <utils/common/TplConvert.h>

/**
 * \class MSSOTLPolicyDesirability
 * \brief This class determines the desirability algorithm of a MSSOTLPolicy when
 * used in combination with a high level policy
 */
class MSSOTLPolicyDesirability: public Parameterised {

private:
    std::string myKeyPrefix;

protected:
    SUMOReal readParameter(std::string parName, SUMOReal defValue) {
        return TplConvert::_2SUMORealSec(getParameter(parName, "").c_str(), defValue);
    }

public:

    MSSOTLPolicyDesirability(std::string keyPrefix,
                             const std::map<std::string, std::string>& parameters);
    virtual ~MSSOTLPolicyDesirability();

    /**
     * \brief Calculates the desirability of the policy
     */
    virtual SUMOReal computeDesirability(SUMOReal vehInMeasure,
                                         SUMOReal vehOutMeasure) = 0;
    /**
     * \brief Calculates the desirability of the policy
     */
    virtual SUMOReal computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure,
                                         SUMOReal vehInDispersionMeasure,
                                         SUMOReal vehOutDispersionMeasure) = 0;
    virtual std::string getMessage() = 0;
    void setKeyPrefix(std::string val) {
        myKeyPrefix = val;
    }
    std::string getKeyPrefix() {
        return myKeyPrefix;
    }

};

#endif /* MSSOTLPOLICYDESIRABILITY_H_ */
