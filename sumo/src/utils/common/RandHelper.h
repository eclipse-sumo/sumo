/****************************************************************************/
/// @file    RandHelper.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RandHelper_h
#define RandHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <foreign/mersenne/MersenneTwister.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RandHelper
 * @brief Utility functions for using a global, resetable random number generator
 */
class RandHelper {
public:
    /// Initialises the given options container with random number options
    static void insertRandOptions();

    /// Reads the given random number options and initialises the random number generator in accordance
    static void initRandGlobal();

    /// Returns a random real number in [0, 1)
    static inline SUMOReal rand() {
        return (SUMOReal) RandHelper::myRandomNumberGenerator.randExc();
    }

    /// Returns a random real number in [0, maxV)
    static inline SUMOReal rand(SUMOReal maxV) {
        return maxV * rand();
    }

    /// Returns a random real number in [minV, maxV)
    static inline SUMOReal rand(SUMOReal minV, SUMOReal maxV) {
        return minV + (maxV - minV) * rand();
    }

    /// Returns a random integer in [0, maxV-1]
    static inline size_t rand(size_t maxV) {
        return (size_t) RandHelper::myRandomNumberGenerator.randInt((MTRand::uint32)(maxV-1));
    }

    /// Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV) {
        return (int) RandHelper::myRandomNumberGenerator.randInt((MTRand::uint32)(maxV-1));
    }

    /// Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV) {
        return minV + rand(maxV - minV);
    }

    /// Access to a random number from a normal distribution
    static inline SUMOReal randNorm(SUMOReal mean, SUMOReal variance) {
        return (SUMOReal) RandHelper::myRandomNumberGenerator.randNorm(mean, variance);
    }

    /// Returns a random element from the given vector
    template<class T>
    static inline T
    getRandomFrom(const std::vector<T> &v) {
        return v[rand(v.size())];
    }

protected:
    /// the random number generator to use
    static MTRand myRandomNumberGenerator;

};

#endif

/****************************************************************************/

