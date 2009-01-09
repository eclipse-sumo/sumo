/****************************************************************************/
/// @file    RandomDistributor.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Represents a generic random distribution
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
class RandomDistributor
{
public:
    /// @brief Constructor for an empty distribution
    RandomDistributor() : myProb(0) { }

    /// @brief Destructor
    ~RandomDistributor() { }

    /** @brief Adds a value with an assigned probability to the distribution.
     *
     * If the value is already member of the distribution the given probability
     * is added to the current.
     * The probability has to be non-negative but values larger than one are
     * allowed (and scaled accordingly when an element is drawn).
     *
     * @param[in] prob The probability assigned to the value
     * @param[in] val The value to add to the distribution
     */
    void add(SUMOReal prob, T val) {
        assert(prob>=0);
        myProb += prob;
        for (size_t i=0; i<myVals.size(); i++) {
            if (val==myVals[i]) {
                myProbs[i] += prob;
                return;
            }
        }
        myVals.push_back(val);
        myProbs.push_back(prob);
    }

    /** @brief Draw a sample of the distribution.
     *
     * A random sample is drawn according to the assigned probabilities.
     *
     * @return the drawn member
     */
    T get() const {
        if (myProb==0) {
            throw OutOfBoundsException();
        }
        SUMOReal prob = RandHelper::rand(myProb);
        for (size_t i=0; i<myVals.size(); i++) {
            if (prob<myProbs[i]) {
                return myVals[i];
            }
            prob -= myProbs[i];
        }
        return myVals[myVals.size()-1];
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
    const std::vector<T> &getVals() const {
        return myVals;
    }

    /** @brief Returns the probabilities assigned to the members of the distribution.
     *
     * See getVals for the corresponding members.
     *
     * @return the probabilities assigned to the distribution
     * @see RandomDistributor::getVals
     */
    const std::vector<SUMOReal> &getProbs() const {
        return myProbs;
    }

private:
    /// @brief the total probability
    SUMOReal myProb;
    /// @brief the members
    std::vector<T> myVals;
    /// @brief the corresponding probabilities
    std::vector<SUMOReal> myProbs;

};


#endif

/****************************************************************************/
