/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGPerson.h
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @date    July 2010
/// @version $Id$
///
// Parent object of every person, contains age and any natural characteristic
/****************************************************************************/
#ifndef AGPERSON_H
#define AGPERSON_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AGPerson
 * @brief Base class of every person in the city (adults and children)
 *
 * This class provides the functionality common to all person in the city. It
 * is a base class for other classes and is not intended to be instantiated
 * as an object. Therefore, all constructors are protected.
 */
class AGPerson {
public:
    /** @brief Provides the age of the person.
     *
     * @return the age in years
     */
    virtual int getAge() const;

    /** @brief Lets the person make a decision.
     *
     * The higher the degree of belief is, the more likely this method returns
     * true.
     *
     * @param[in] degreeOfBelief how strong the person beliefs the proposition
     * @return whether the person agrees with the proposition
     */
    virtual bool decide(double probability) const;

    /** @brief Puts out a summary of the class properties.
     */
    virtual void print() const;

protected:
    int age;

    /** @brief Initialises the class attributes.
     *
     * @param[in] age the age of the person
     */
    AGPerson(int age);

    /** @brief Cleans up everything.
     */
    virtual ~AGPerson();
};

#endif /* AGPerson */

/****************************************************************************/
