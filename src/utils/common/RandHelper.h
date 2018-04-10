/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <random>


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

    /// @brief Initialises the random number generator with hardware randomness or seed
    static void initRand(std::mt19937* which = 0, const bool random = false, const int seed = 23423);

    /// @brief Reads the given random number options and initialises the random number generator in accordance
    static void initRandGlobal(std::mt19937* which = 0);

    /// @brief Returns a random real number in [0, 1)
    static inline double rand(std::mt19937* rng = 0) {
        if (rng == 0) {
            rng = &myRandomNumberGenerator;
        }
        return double((*rng)() / 4294967296.0);
    }

    /// @brief Returns a random real number in [0, maxV)
    static inline double rand(double maxV, std::mt19937* rng = 0) {
        return maxV * rand(rng);
    }

    /// @brief Returns a random real number in [minV, maxV)
    static inline double rand(double minV, double maxV, std::mt19937* rng = 0) {
        return minV + (maxV - minV) * rand(rng);
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV, std::mt19937* rng = 0) {
        if (rng == 0) {
            rng = &myRandomNumberGenerator;
        }
        unsigned int usedBits = maxV - 1;
        usedBits |= usedBits >> 1;
        usedBits |= usedBits >> 2;
        usedBits |= usedBits >> 4;
        usedBits |= usedBits >> 8;
        usedBits |= usedBits >> 16;

        // Draw numbers until one is found in [0, maxV-1]
        int result;
        do {
            result = (*rng)() & usedBits;
        } while (result >= maxV);
        return result;
    }

    /// @brief Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV, std::mt19937* rng = 0) {
        return minV + rand(maxV - minV, rng);
    }

    /// @brief Returns a random 64 bit integer in [0, maxV-1]
    static inline long long int rand(long long int maxV, std::mt19937* rng = 0) {
        if (maxV <= std::numeric_limits<int>::max()) {
            return rand((int)maxV, rng);
        }
        if (rng == 0) {
            rng = &myRandomNumberGenerator;
        }
        unsigned long long int usedBits = maxV - 1;
        usedBits |= usedBits >> 1;
        usedBits |= usedBits >> 2;
        usedBits |= usedBits >> 4;
        usedBits |= usedBits >> 8;
        usedBits |= usedBits >> 16;
        usedBits |= usedBits >> 32;

        // Draw numbers until one is found in [0, maxV-1]
        long long int result;
        do {
            result = (((unsigned long long int)(*rng)() << 32) | (*rng)()) & usedBits;    // toss unused bits to shorten search
        } while (result >= maxV);
        return result;
    }

    /// @brief Returns a random 64 bit integer in [minV, maxV-1]
    static inline long long int rand(long long int minV, long long int maxV, std::mt19937* rng = 0) {
        return minV + rand(maxV - minV, rng);
    }

    /// @brief Access to a random number from a normal distribution
    static inline double randNorm(double mean, double variance, std::mt19937* rng = 0) {
        // Polar method to avoid cosine
        double u, q;
        do {
            u = rand(2.0, rng) - 1;
            const double v = rand(2.0, rng) - 1;
            q = u * u + v * v;
        } while (q == 0.0 || q >= 1.0);
        return (double)(mean + variance * u * sqrt(-2 * log(q) / q));
    }

    /// @brief Returns a random element from the given vector
    template<class T>
    static inline const T&
    getRandomFrom(const std::vector<T>& v, std::mt19937* rng = 0) {
        assert(v.size() > 0);
        return v[rand((int)v.size(), rng)];
    }


protected:
    /// @brief the random number generator to use
    static std::mt19937 myRandomNumberGenerator;

};

#endif

/****************************************************************************/
