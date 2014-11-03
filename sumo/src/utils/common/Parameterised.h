/****************************************************************************/
/// @file    Parameterised.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Sept 2002
/// @version $Id$
///
// An upper class for objects with additional parameters
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Parameterised_h
#define Parameterised_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Parameterised
 * @brief An upper class for objects with additional parameters
 */
class Parameterised {
public:
    /** @brief Constructor
     */
    Parameterised();


    /** @brief Constructor
     * @param[in] mapArg Pre-given parameter
     */
    Parameterised(const std::map<std::string, std::string>& mapArg);


    /// @brief Destructor
    ~Parameterised();



    /** @brief Adds a parameter
     * @param[in] key The parameter's name
     * @param[in] value The parameter's value
     */
    void addParameter(const std::string& key, const std::string& value);


    /** @brief Adds all given parameter
     * @param[in] mapArg The keys/values to insert
     */
    void addParameter(const std::map<std::string, std::string>& mapArg);


    /** @brief Adds all given parameter
     * @param[in] p The keys/values to insert
     */
    void addParameter(const Parameterised& p);


    /** @brief Returns whether the parameter is known
     * @param[in] key The key to ask for
     * @return Whether the key is known
     */
    bool knowsParameter(const std::string& key) const;


    /** @brief Returns the value for a given key
     * @param[in] key The key to ask for
     * @param[in] defaultValue The default value to return if no value is stored under the key
     * @return The value stored under the key
     */
    const std::string& getParameter(const std::string& key, const std::string& defaultValue) const;


    /** @brief Clears the parameter map
     */
    void clearParameter();


    /** @brief Returns the inner key/value map
     * @return the inner map
     */
    const std::map<std::string, std::string>& getMap() const {
        return myMap;
    }


private:
    /// @brief The key->value map
    std::map<std::string, std::string> myMap;

};


#endif

/****************************************************************************/

