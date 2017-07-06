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

    /// @brief Reads the given random number options and initialises the random number generator in accordance
    static void initRandGlobal(std::mt19937* which = 0);

    /// @brief Returns a random real number in [0, 1)
    static inline double rand(std::mt19937* rng = 0) {
        std::uniform_real_distribution<> dis(0., 1.);
        return dis(rng != 0 ? *rng : myRandomNumberGenerator);
    }

    /// @brief Returns a random real number in [0, maxV)
    static inline double rand(double maxV, std::mt19937* rng = 0) {
        return maxV * rand(rng);
    }

    /// @brief Returns a random real number in [minV, maxV)
    static inline double rand(double minV, double maxV) {
        return minV + (maxV - minV) * rand();
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV, std::mt19937* rng = 0) {
        std::uniform_int_distribution<> dis(0, maxV - 1);
        return dis(rng != 0 ? *rng : myRandomNumberGenerator);
    }

    /// @brief Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV) {
        return minV + rand(maxV - minV);
    }

    /// @brief Returns a random 64 bit integer in [0, maxV-1]
    static inline long long int rand(long long int maxV) {
        std::uniform_int_distribution<long long int> dis(0, maxV - 1);
        return dis(RandHelper::myRandomNumberGenerator);
    }

    /// @brief Returns a random 64 bit integer in [minV, maxV-1]
    static inline long long int rand(long long int minV, long long int maxV) {
        return minV + rand(maxV - minV);
    }

    /// @brief Access to a random number from a normal distribution
    static inline double randNorm(double mean, double variance, std::mt19937* rng = 0) {
        std::normal_distribution<> dis(mean, variance);
        return dis(rng != 0 ? *rng : myRandomNumberGenerator);
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
    static std::mt19937 myRandomNumberGenerator;

};

#endif

/****************************************************************************/

