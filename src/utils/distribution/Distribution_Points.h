/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Distribution_Points.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The description of a distribution by a curve
/****************************************************************************/
#ifndef Distribution_Points_h
#define Distribution_Points_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "RandomDistributor.h"
#include "Distribution.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Distribution_Points
 * A description of a distribution that uses a set of points in a 2d-space
 *  to describe the values (each points x-value) and their possibilities
 *  (each points y-value)
 */
class Distribution_Points :
    public Distribution, public RandomDistributor<double> {
public:
    /// Constructor
    Distribution_Points(const std::string& id);

    /// Destructor
    virtual ~Distribution_Points();

    /** @brief Draw a sample of the distribution.
    *
    * A random sample is drawn according to the assigned probabilities.
    *
    * @param[in] which The random number generator to use; the static one will be used if 0 is passed
    * @return the drawn member
    */
    double sample(std::mt19937* which = 0) const {
        return get(which);
    }

    /// Returns the maximum value of this distribution
    double getMax() const;

    /// Returns the string representation of this distribution
    std::string toStr(std::streamsize accuracy) const;
};


#endif

/****************************************************************************/

