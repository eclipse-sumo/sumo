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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AGPERSON_H
#define AGPERSON_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


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
    virtual bool decide(SUMOReal probability) const;

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
