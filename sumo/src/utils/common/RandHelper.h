/****************************************************************************/
/// @file    RandHelper.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
class RandHelper
{
public:
    /// Initialises the given options container with random number options
    static void insertRandOptions();

    /// Reads the given random number options and initialises the random number geerator in accordance
    static void initRandGlobal();

    /// Returns a random integer in [0, len-1]
    static inline size_t getRandomIndex(size_t len)
    {
        return (size_t) RandHelper::myRandomNumberGenerator.randInt(len-1);
    }

    /// Returns a random real number in [0, 1)
    static inline SUMOReal rand()
    {
        return (SUMOReal) RandHelper::myRandomNumberGenerator.randExc();
    }

    /// Returns a random real number in [0, maxV)
    static inline SUMOReal rand(SUMOReal maxV)
    {
        return maxV *(SUMOReal) RandHelper::myRandomNumberGenerator.randExc();
    }

    /// Returns a random real number in [minV, maxV)
    static inline SUMOReal rand(SUMOReal minV, SUMOReal maxV)
    {
        SUMOReal range = maxV - minV;
        return minV + (range *(SUMOReal) RandHelper::myRandomNumberGenerator.randExc());
    }

protected:
    /// the random number generator to use
    static MTRand myRandomNumberGenerator;

};


/// Returns a random element from the given vector
template<class T>
inline T
getRandomFrom(const std::vector<T> &v)
{
    return v[RandHelper::getRandomIndex(v.size())];
}


/// Returns a random real number between [0, 1)
inline SUMOReal
randSUMO()
{
    return RandHelper::rand();
}


/// Returns a random number between [0, maxV)
template<class T>
inline T
randSUMO(T maxV)
{
    return (T) RandHelper::rand((SUMOReal) maxV);
}


/// Returns a random number between [minV, maxV)
template<class T>
inline T
randSUMO(T minV, T maxV)
{
    return (T) RandHelper::rand((SUMOReal) minV, (SUMOReal) maxV);
}


#endif

/****************************************************************************/

