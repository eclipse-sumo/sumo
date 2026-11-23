/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    RandHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
///
//
/****************************************************************************/
#include <config.h>

#include <array>
#include <cstdint>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/common/SysUtils.h>
#include "StdDefs.h"
#include "Translation.h"
#include "UtilExceptions.h"
#include "RandHelper.h"


namespace {
using MTWord = std::uint32_t;
constexpr std::size_t MT_SIZE = std::mt19937::state_size;
constexpr MTWord MT_LOW_MASK = (MTWord{1} << std::mt19937::mask_bits) - 1;


MTWord
undoMTTwist(MTWord mixed) {
    // The top bit of the twist result determines whether the xor mask was used.
    const MTWord lowBit = mixed >> (std::mt19937::word_size - 1);
    return ((mixed ^ (lowBit * static_cast<MTWord>(std::mt19937::xor_mask))) << 1) | lowBit;
}


void
writePortableMTState(std::istream& input, std::ostream& output) {
    std::array<MTWord, MT_SIZE> words;
    for (MTWord& word : words) {
        if (!(input >> word)) {
            throw ProcessError(TL("Invalid random number generator state."));
        }
    }
    // MSVC and libc++ already write the last MT_SIZE words in chronological order.
    // libstdc++ writes a buffer and a cursor counting the consumed words in that buffer.
    std::size_t cursor = MT_SIZE;
    input >> std::ws;
    if (!input.eof()) {
        if (!(input >> cursor) || cursor > MT_SIZE || !(input >> std::ws).eof()) {
            throw ProcessError(TL("Invalid random number generator state."));
        }
    }
    if (cursor < MT_SIZE) {
        // Recover the previous block's tail from the unused suffix of the current block.
        // Reverse order keeps the recurrence's source words available in place.
        for (std::size_t i = MT_SIZE; i-- > cursor;) {
            const MTWord pair = undoMTTwist(words[i] ^ words[(i + std::mt19937::shift_size) % MT_SIZE]);
            words[i] = pair & ~MT_LOW_MASK;
            if (i + 1 < MT_SIZE) {
                words[i + 1] |= pair & MT_LOW_MASK;
            }
        }
        // The oldest word's low bits do not affect future draws. Recover them when possible
        // so equivalent states written by different libraries have the same text.
        if (cursor > 0) {
            const std::size_t i = cursor - 1;
            words[cursor] |= undoMTTwist(words[i] ^ words[(i + std::mt19937::shift_size) % MT_SIZE]) & MT_LOW_MASK;
        }
        std::rotate(words.begin(), words.begin() + cursor, words.end());
    }
    for (const MTWord word : words) {
        output << word << ' ';
    }
    // A cursor at the end of the block makes libstdc++ start at the next twist.
    // Readers of the chronological representation consume just the preceding words.
    output << MT_SIZE;
}
}


// debug vehicle movement randomness (dawdling in finalizeSpeed)
//#define DEBUG_FLAG1
//#define DEBUG_RANDCALLS
//#define DEBUG_RANDCALLS_PARALLEL


// ===========================================================================
// static member variables
// ===========================================================================
SumoRNG RandHelper::myRandomNumberGenerator("default");

#ifdef DEBUG_RANDCALLS
unsigned long long int myDebugIndex(7);
std::string myDebugId("");
#endif

#ifdef DEBUG_RANDCALLS_PARALLEL
#include <thread>
#include <utils/iodevices/OutputDevice.h>
std::map<std::thread::id, int> threadIndices;
std::map<std::string, int> lastThreadIndex; // by rng
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

std::string
RandHelper::saveState(SumoRNG* rng) {
    if (rng == nullptr) {
        rng = &myRandomNumberGenerator;
    }
    std::ostringstream output;
    output << rng->count;
    if (rng->count >= SAVE_ONLY_COUNT) {
        std::stringstream nativeState;
        nativeState << *rng;
        output << ' ';
        writePortableMTState(nativeState, output);
    }
    return output.str();
}


void
RandHelper::loadState(const std::string& state, SumoRNG* rng) {
    if (rng == nullptr) {
        rng = &myRandomNumberGenerator;
    }
    std::istringstream input(state);
    unsigned long long count;
    if (!(input >> count)) {
        throw ProcessError(TL("Invalid random number generator state."));
    }
    if (count < SAVE_ONLY_COUNT) {
        rng->discard(count);
    } else {
        std::stringstream portableState;
        writePortableMTState(input, portableState);
        std::mt19937 restored;
        if (!(portableState >> restored)) {
            throw ProcessError(TL("Invalid random number generator state."));
        }
        static_cast<std::mt19937&>(*rng) = restored;
    }
    rng->count = count;
}


void
RandHelper::insertRandOptions(OptionsCont& oc) {
    // registers random number options
    oc.addOptionSubTopic("Random Number");

    oc.doRegister("random", new Option_Bool(false));
    oc.addSynonyme("random", "abs-rand", true);
    oc.addDescription("random", "Random Number", TL("Initialises the random number generator with the current system time"));

    oc.doRegister("seed", new Option_Integer(23423));
    oc.addSynonyme("seed", "srand", true);
    oc.addDescription("seed", "Random Number", TL("Initialises the random number generator with the given value"));
}


void
RandHelper::initRand(SumoRNG* which, const bool random, const int seed) {
    if (which == nullptr) {
        which = &myRandomNumberGenerator;
    }
    if (random) {
        which->setSeed((unsigned long)time(nullptr));
    } else {
        which->setSeed(seed);
    }
}


void
RandHelper::initRandGlobal(SumoRNG* which) {
    OptionsCont& oc = OptionsCont::getOptions();
    initRand(which, oc.getBool("random"), oc.getInt("seed"));
}

int
RandHelper::getSeed(SumoRNG* which) {
    if (which == nullptr) {
        which = &myRandomNumberGenerator;
    }
    return which->origSeed;
}

double
RandHelper::rand(SumoRNG* rng) {
    if (rng == nullptr) {
        rng = &myRandomNumberGenerator;
    }
    const double res = double((*rng)() / 4294967296.0);
    rng->count++;
#ifdef DEBUG_RANDCALLS
    if (rng->count == myDebugIndex
            && (myDebugId == "" || rng->id == myDebugId)) {
        std::cout << "DEBUG\n"; // for setting breakpoint
    }
    std::stringstream stream; // to reduce output interleaving from different threads
#ifdef DEBUG_RANDCALLS_PARALLEL
    auto threadID = std::this_thread::get_id();
    if (threadIndices.count(threadID) == 0) {
        const int tmp = threadIndices.size();
        threadIndices[threadID] = tmp;
    }
    int threadIndex = threadIndices[threadID];
    auto it = lastThreadIndex.find(rng->id);
    if ((it == lastThreadIndex.end() || it->second != threadIndex)
            && (myDebugId == "" || rng->id == myDebugId)) {
        std::cout << "DEBUG rng " << rng->id << " change thread old=" << (it == lastThreadIndex.end() ? -1 : it->second) << " new=" << threadIndex << " (" << std::this_thread::get_id() << ")\n"; // for setting breakpoint
    }
    lastThreadIndex[rng->id] = threadIndex;
    stream << "rng " << rng->id << " call=" << rng->count << " thread=" << threadIndex << " val=" << res << "\n";
    OutputDevice::getDevice(rng->id) << stream.str();
#else
    stream << "rng " << rng->id << " call=" << rng->count << " val=" << res << "\n";
    std::cout << stream.str();
#endif
#endif
#ifdef DEBUG_FLAG1
    if (gDebugFlag1) {
        std::stringstream stream; // to reduce output interleaving from different threads
        stream << "rng " << rng->id << " call=" << rng->count << " val=" << res << "\n";
        std::cout << stream.str();
    }
#endif
    return res;
}

double
RandHelper::randNorm(double mean, double variance, SumoRNG* rng) {
    // Polar method to avoid cosine
    double u, q;
    do {
        u = rand(2.0, rng) - 1;
        const double v = rand(2.0, rng) - 1;
        q = u * u + v * v;
    } while (q == 0.0 || q >= 1.0);
    const double logRounded = ceil(log(q) * 1e14) / 1e14;
    return mean + variance * u * sqrt(-2 * logRounded / q);
}

double
RandHelper::randExp(double rate, SumoRNG* rng) {
    double r = rand(rng);
    while (r == 0) {
        r = rand(rng);
    }
    return -log(r) / rate;
}

// template<class T>
// void RandHelper::shuffle(const std::vector<T>& v) {
//     std::shuffle(v.begin(), v.end(), rng);
// }

/****************************************************************************/
