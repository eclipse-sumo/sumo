/****************************************************************************/
/// @file    RandomDistributor.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2005-09-15
/// @version $Id$
///
// Represents a generic random distribution
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
#ifndef RandomDistributor_h
#define RandomDistributor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <limits>
#include <utils/common/RandHelper.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RandomDistributor
 * @brief Represents a generic random distribution
 *
 * This class allows to create random distributions by assigning
 *  arbitrary (non-negative) probabilities to its elements. The
 *  random number generator used is specified in RandHelper.
 *
 * @see RandHelper
 */

template<class T>
class RandomDistributor {
public:
    /** @brief Constructor for an empty distribution
     * @param[in] maximumSize The maximum size to maintain
     *   older entrys will be removed when adding more than the maximumSize
     */
    RandomDistributor() :
        myProb(0) {
    }

    /// @brief Destructor
    ~RandomDistributor() { }

    /** @brief Adds a value with an assigned probability to the distribution.
     *
     * If the value is already member of the distribution and checkDuplicates is
     * true (the default) the given probability is added to the current.
     * The probability has to be non-negative but values larger than one are
     * allowed (and scaled accordingly when an element is drawn).
     *
     * @param[in] prob The probability assigned to the value
     * @param[in] val The value to add to the distribution
     * @return true if a new value was added, false if just the probability of an existing one was updated
     */
    bool add(SUMOReal prob, T val, bool checkDuplicates = true) {
        assert(prob >= 0);
        myProb += prob;
        if (checkDuplicates) {
            for (int i = 0; i < (int)myVals.size(); i++) {
                if (val == myVals[i]) {
                    myProbs[i] += prob;
                    return false;
                }
            }
        }
        myVals.push_back(val);
        myProbs.push_back(prob);
        return true;
    }

    /** @brief Draw a sample of the distribution.
     *
     * A random sample is drawn according to the assigned probabilities.
     *
     * @param[in] which The random number generator to use; the static one will be used if 0 is passed
     * @return the drawn member
     */
    T get(MTRand* which = 0) const {
        if (myProb == 0) {
            throw OutOfBoundsException();
        }
        SUMOReal prob = which == 0 ? RandHelper::rand(myProb) : which->rand(myProb);
        for (int i = 0; i < (int)myVals.size(); i++) {
            if (prob < myProbs[i]) {
                return myVals[i];
            }
            prob -= myProbs[i];
        }
        return myVals.back();
    }

    /** @brief Return the sum of the probabilites assigned to the members.
     *
     * This should be zero if and only if the distribution is empty.
     *
     * @return the total probability
     */
    SUMOReal getOverallProb() const {
        return myProb;
    }

    /// @brief Clears the distribution
    void clear() {
        myProb = 0;
        myVals.clear();
        myProbs.clear();
    }

    /** @brief Returns the members of the distribution.
     *
     * See getProbs for the corresponding probabilities.
     *
     * @return the members of the distribution
     * @see RandomDistributor::getProbs
     */
    const std::vector<T>& getVals() const {
        return myVals;
    }

    /** @brief Returns the probabilities assigned to the members of the distribution.
     *
     * See getVals for the corresponding members.
     *
     * @return the probabilities assigned to the distribution
     * @see RandomDistributor::getVals
     */
    const std::vector<SUMOReal>& getProbs() const {
        return myProbs;
    }

private:
    /// @brief the total probability
    SUMOReal myProb;
    /// @brief the members (acts as a ring buffer if myMaximumSize is reached)
    std::vector<T> myVals;
    /// @brief the corresponding probabilities (acts as a ring buffer if myMaximumSize is reached)
    std::vector<SUMOReal> myProbs;

};


#endif

/****************************************************************************/
