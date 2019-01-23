/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Distribution.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for distribution descriptions.
/****************************************************************************/
#ifndef Distribution_h
#define Distribution_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <random>
#include <utils/common/Named.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Distribution
 * The base class for distribution descriptions. Only an interface
 *  specification.
 */
class Distribution : public Named {
public:
    /// Constructor
    Distribution(const std::string& id) : Named(id) { }

    /// Destructor
    virtual ~Distribution() { }

    /** @brief Draw a sample of the distribution.
    *
    * A random sample is drawn according to the assigned probabilities.
    *
    * @param[in] which The random number generator to use; the static one will be used if 0 is passed
    * @return the drawn member
    */
    virtual double sample(std::mt19937* which = 0) const = 0;

    /// Returns the maximum value of this distribution
    virtual double getMax() const = 0;

    /// Returns the string representation of this distribution
    virtual std::string toStr(std::streamsize accuracy) const = 0;

};


#endif

/****************************************************************************/

