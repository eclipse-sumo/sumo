/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2005-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    RandHelper.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <cassert>
#include <cstring>
#include <vector>
#include <map>
#include <random>
#include <sstream>
#include <iostream>
#include <algorithm>

// TODO make this configurable
#define SAVE_ONLY_COUNT 1000000

// ===========================================================================
// helper function
// ===========================================================================

#ifdef __clang__
__attribute__((no_sanitize("unsigned-integer-overflow")))
#endif
inline uint64_t splitmix64(const uint64_t seed) {
    uint64_t z = (seed + 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}


// ===========================================================================
// class declaration
// ===========================================================================

class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class XoShiRo256PlusPlus
 * @brief A random number generator as proposed here https://prng.di.unimi.it/xoshiro256plusplus.c
 */
class XoShiRo256PlusPlus {
public:
    inline void seed(const uint64_t seed)     {
        this->state[0] = splitmix64(splitmix64(seed));
        this->state[1] = splitmix64(this->state[0]);
        this->state[2] = splitmix64(this->state[1]);
        this->state[3] = splitmix64(this->state[2]);
    }

    inline uint64_t operator()()     {
        const uint64_t result = rotl64(this->state[0] + this->state[3], 23) + this->state[0];
        const uint64_t t = this->state[1] << 17;
        this->state[2] ^= this->state[0];
        this->state[3] ^= this->state[1];
        this->state[1] ^= this->state[2];
        this->state[0] ^= this->state[3];
        this->state[2] ^= t;
        this->state[3] = rotl64(this->state[3], 45);
        return result;
    }

    void discard(unsigned long long skip) {
        while (skip-- > 0) {
            (*this)();
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const XoShiRo256PlusPlus& r) {
        os << r.state[0] << r.state[1] << r.state[2] << r.state[3];
        return os;
    }

    friend std::istream& operator>>(std::istream& is, XoShiRo256PlusPlus& r) {
        is >> r.state[0] >> r.state[1] >> r.state[2] >> r.state[3];
        return is;
    }


private:
    static inline uint64_t rotl64(const uint64_t x, const int k) {
        return (x << k) | (x >> (64 - k));
    }

    uint64_t state[4];

};


//class SumoRNG : public XoShiRo256PlusPlus {
class SumoRNG : public std::mt19937 {
public:
    SumoRNG(const std::string& _id) : id(_id) {}

    void setSeed(int _seed) {
        origSeed = _seed;
        seed(_seed);
    }

    unsigned long long int count = 0;
    int origSeed = default_seed;
    std::string id;
};


/**
 * @class RandHelper
 * @brief Utility functions for using a global, resetable random number generator
 */
class RandHelper {

public:
    /// @brief Initialises the given options container with random number options
    static void insertRandOptions(OptionsCont& oc);

    /// @brief Initialises the random number generator with hardware randomness or seed
    static void initRand(SumoRNG* which = nullptr, const bool random = false, const int seed = 23423);

    /// @brief Reads the given random number options and initialises the random number generator in accordance
    static void initRandGlobal(SumoRNG* which = nullptr);

    static int getSeed(SumoRNG* which = nullptr);

    /// @brief Returns a random real number in [0, 1)
    static double rand(SumoRNG* rng = nullptr);

    /// @brief Returns a random real number in [0, maxV)
    static inline double rand(double maxV, SumoRNG* rng = nullptr) {
        return maxV * rand(rng);
    }

    /// @brief Returns a random real number in [minV, maxV)
    static inline double rand(double minV, double maxV, SumoRNG* rng = nullptr) {
        return minV + (maxV - minV) * rand(rng);
    }

    /// @brief Returns a random integer in [0, maxV-1]
    static inline int rand(int maxV, SumoRNG* rng = nullptr) {
        if (rng == nullptr) {
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
            rng->count++;
        } while (result >= maxV);
        return result;
    }

    /// @brief Returns a random integer in [minV, maxV-1]
    static inline int rand(int minV, int maxV, SumoRNG* rng = nullptr) {
        return minV + rand(maxV - minV, rng);
    }

    /// @brief Returns a random 64 bit integer in [0, maxV-1]
    static inline long long int rand(long long int maxV, SumoRNG* rng = nullptr) {
        if (maxV <= std::numeric_limits<int>::max()) {
            return rand((int)maxV, rng);
        }
        if (rng == nullptr) {
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
            rng->count += 2;
        } while (result >= maxV);
        return result;
    }

    /// @brief Returns a random 64 bit integer in [minV, maxV-1]
    static inline long long int rand(long long int minV, long long int maxV, SumoRNG* rng = nullptr) {
        return minV + rand(maxV - minV, rng);
    }

    /// @brief Access to a random number from a normal distribution
    static double randNorm(double mean, double variance, SumoRNG* rng = nullptr);

    /// @brief Access to a random number from an exponential distribution
    static double randExp(double rate, SumoRNG* rng = nullptr);

    /// @brief Returns a random element from the given vector
    template<class T>
    static inline const T&
    getRandomFrom(const std::vector<T>& v, SumoRNG* rng = nullptr) {
        assert(v.size() > 0);
        return v[rand((int)v.size(), rng)];
    }

    /// @brief save rng state to string
    static std::string saveState(SumoRNG* rng = nullptr) {
        if (rng == nullptr) {
            rng = &myRandomNumberGenerator;
        }
        std::ostringstream oss;
        oss << rng->count;
        if (rng->count >= SAVE_ONLY_COUNT) {
            oss << " " << (*rng);
        }
        return oss.str();
    }

    /// @brief load rng state from string
    static void loadState(const std::string& state, SumoRNG* rng = nullptr) {
        if (rng == nullptr) {
            rng = &myRandomNumberGenerator;
        }
        std::istringstream iss(state);
        iss >> rng->count;
        if (rng->count < SAVE_ONLY_COUNT) {
            rng->discard(rng->count);
        } else {
            iss >> (*rng);
        }
    }

    template<class T>
    static void shuffle(std::vector<T>& v, SumoRNG* rng = nullptr) {
        for (int i = (int)(v.size() - 1); i > 0; --i) {
            std::swap(*(v.begin() + i), *(v.begin() + rand(i, rng)));
        }
    }

    static long long int count() {
        return myRandomNumberGenerator.count;
    }

    /// @brief return a value scrambled value from [0, 1]
    static double randHash(long long int x) {
        const uint64_t h = splitmix64(x) >> 11;
        return (double)h / (double(1ULL << 53));
    }

    /// @brief string hashing adapted from https://en.wikipedia.org/wiki/MurmurHash
#ifdef __clang__
    __attribute__((no_sanitize("integer"))) // left-shift and unsigned-integer-overflow
#endif
    static uint32_t murmur3_32(const std::string& key2, int seed) {
        const uint8_t* key = reinterpret_cast<const uint8_t*>(key2.data());
        const size_t len = key2.size();
        uint32_t h = seed;
        uint32_t k;
        /* Read in groups of 4. */
        for (size_t i = len >> 2; i; i--) {
            memcpy(&k, key, sizeof(uint32_t));
            key += sizeof(uint32_t);
            h ^= murmur_32_scramble(k);
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        }
        /* Read the rest. */
        k = 0;
        for (size_t i = len & 3; i; i--) {
            k <<= 8;
            k |= key[i - 1];
        }
        h ^= murmur_32_scramble(k);
        /* Finalize. */
        h ^= len;
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }


protected:
    /// @brief the default random number generator to use
    static SumoRNG myRandomNumberGenerator;

    /// @brief helper function for murmur_32_scramble from https://en.wikipedia.org/wiki/MurmurHash
#ifdef __clang__
    __attribute__((no_sanitize("integer"))) // left-shift and unsigned-integer-overflow
#endif
    static inline uint32_t murmur_32_scramble(uint32_t k) {
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        return k;
    }

};
