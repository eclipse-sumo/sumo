/****************************************************************************/
/// @file    RandHelper.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cassert>
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
    /// @brief Initialises the given options container with random number options
    static void insertRandOptions();

    /// @brief Reads the given random number options and initialises the random number generator in accordance
    static void initRandGlobal(MTRand* which = 0);

    /// @brief Returns a random real number in [0, 1)
    static inline SUMOReal rand() {
        return (SUMOReal) RandHelper::myRandomNumberGenerator.randExc();
    }

    /// @brief Returns a random real number in [0, maxV)
    static inline SUMOReal rand(SUMOReal maxV) {
        return maxV * rand();
    }

    /// @brief Returns a random real number in [minV, maxV)
    static inline SUMOReal rand(SUMOReal minV, SUMOReal maxV) {
        return minV + (maxV - minV) * rand();
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline size_t rand(size_t maxV) {
        return (size_t) RandHelper::myRandomNumberGenerator.randInt((MTRand::uint32)(maxV - 1));
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV) {
        return (int) RandHelper::myRandomNumberGenerator.randInt((MTRand::uint32)(maxV - 1));
    }

    /// @brief Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV) {
        return minV + rand(maxV - minV);
    }

    /// @brief Access to a random number from a normal distribution
    static inline SUMOReal randNorm(SUMOReal mean, SUMOReal variance, MTRand& rng = myRandomNumberGenerator) {
        return (SUMOReal) rng.randNorm(mean, variance);
    }

    /// @brief Returns a random element from the given vector
    template<class T>
    static inline T
    getRandomFrom(const std::vector<T>& v) {
        assert(v.size() > 0);
        return v[rand(v.size())];
    }


protected:
    /// @brief the random number generator to use
    static MTRand myRandomNumberGenerator;

};

#endif

/****************************************************************************/

