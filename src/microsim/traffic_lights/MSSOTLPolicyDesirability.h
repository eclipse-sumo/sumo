/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSSOTLPolicyDesirability.h
/// @author  Riccardo Belletti
/// @date    2014-03-20
/// @version $Id$
///
// The class a low-level policy desirability
/****************************************************************************/

#ifndef MSSOTLPOLICYDESIRABILITY_H_
#define MSSOTLPOLICYDESIRABILITY_H_

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/SwarmDebug.h>
#include <sstream>
#include <utils/common/Parameterised.h>
#include <utils/common/StringUtils.h>

/**
 * \class MSSOTLPolicyDesirability
 * \brief This class determines the desirability algorithm of a MSSOTLPolicy when
 * used in combination with a high level policy
 */
class MSSOTLPolicyDesirability: public Parameterised {

private:
    std::string myKeyPrefix;

public:

    MSSOTLPolicyDesirability(std::string keyPrefix,
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
    virtual std::string getMessage() = 0;
    void setKeyPrefix(std::string val) {
        myKeyPrefix = val;
    }
    std::string getKeyPrefix() {
        return myKeyPrefix;
    }

};

#endif /* MSSOTLPOLICYDESIRABILITY_H_ */
