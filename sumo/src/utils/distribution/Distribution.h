/****************************************************************************/
/// @file    Distribution.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for distribution descriptions.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Distribution_h
#define Distribution_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

