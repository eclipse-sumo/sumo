/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Distribution_Parameterized.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A distribution described by parameters such as the mean value and std-dev
/****************************************************************************/
#ifndef Distribution_MeanDev_h
#define Distribution_MeanDev_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <random>
#include "Distribution.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Distribution_Parameterized
 * A description of distribution by the distribution's mean value and a
 *  standard deviation.
 * Incomplete and unused yet. This class should be overridden by derived
 *  classes
 */
class Distribution_Parameterized :
    public Distribution {
public:
    /// Constructor for standard normal distribution
    Distribution_Parameterized(const std::string& id, double mean,
                               double deviation);

    /// Constructor for normal distribution with cutoff
    Distribution_Parameterized(const std::string& id, double mean,
                               double deviation, double min, double max);

    /// Destructor
    virtual ~Distribution_Parameterized();

    /// Overwrite by parsable distribution description
    void parse(const std::string& description);

    /** @brief Draw a sample of the distribution.
    *
    * A random sample is drawn according to the assigned probabilities.
    *
    * @param[in] which The random number generator to use; the static one will be used if 0 is passed
    * @return the drawn member
    */
    double sample(std::mt19937* which = 0) const;

    /// Returns the maximum value of this distribution
    double getMax() const;

    /// Returns the parameters of this distribution
    std::vector<double>& getParameter() {
        return myParameter;
    }

    /// Returns the unmodifiable parameters of this distribution
    const std::vector<double>& getParameter() const {
        return myParameter;
    }

    /// Returns the string representation of this distribution
    std::string toStr(std::streamsize accuracy) const;

private:
    /// The distribution's parameters
    std::vector<double> myParameter;

};


#endif

/****************************************************************************/

