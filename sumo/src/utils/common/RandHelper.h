/****************************************************************************/
/// @file    RandHelper.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
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
    static inline int rand(int maxV) {
        return (int) RandHelper::myRandomNumberGenerator.randInt((MTRand::uint32)(maxV - 1));
    }

    /// @brief Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV) {
        return minV + rand(maxV - minV);
    }

    /// @brief Returns a random 64 bit integer in [0, maxV-1]
    static inline long long int rand(long long int maxV) {
        return (long long int) RandHelper::myRandomNumberGenerator.randInt64((unsigned long long int)(maxV - 1));
    }

    /// @brief Returns a random 64 bit integer in [minV, maxV-1]
    static inline long long int rand(long long int minV, long long int maxV) {
        return minV + rand(maxV - minV);
    }

    /// @brief Access to a random number from a normal distribution
    static inline SUMOReal randNorm(SUMOReal mean, SUMOReal variance, MTRand* rng = 0) {
        if (rng == 0) {
            rng = &myRandomNumberGenerator;
        }
        // Polar method to avoid cosine
        double u, q;
        do {
            u = rng->randExc(2.0) - 1;
            const double v = rng->randExc(2.0) - 1;
            q  = u * u + v * v;
        } while (q == 0.0 || q >= 1.0);
        return (SUMOReal)(mean + variance * u * sqrt(-2 * log(q) / q));
    }

    /// @brief Returns a random element from the given vector
    template<class T>
    static inline const T&
    getRandomFrom(const std::vector<T>& v) {
        assert(v.size() > 0);
        return v[rand((int)v.size())];
    }


protected:
    /// @brief the random number generator to use
    static MTRand myRandomNumberGenerator;

};

#endif

/****************************************************************************/

