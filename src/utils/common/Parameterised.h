/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    Parameterised.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Sept 2002
///
// A super class for objects with additional parameters
/****************************************************************************/
#pragma once
#include <config.h>
#include <map>
#include <string>
#include <vector>

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Parameterised
 * @brief An upper class for objects with additional parameters
 */
class Parameterised {

public:
    /// @brief parameters map
    typedef std::map<std::string, std::string> Map;

    /// @brief Default constructor
    Parameterised();

    /**@brief Constructor with parameters (for Strings)
     * @param[in] mapArg Pre-given parameter
     */
    Parameterised(const Parameterised::Map& mapArg);

    /// @brief Destructor
    virtual ~Parameterised();

    /**@brief Sets a parameter
     * @param[in] key The parameter's name
     * @param[in] value The parameter's value
     */
    virtual void setParameter(const std::string& key, const std::string& value);

    /**@brief Removes a parameter
     * @param[in] key The parameter's name
     */
    void unsetParameter(const std::string& key);

    /**@brief Adds or updates all given parameters from the map
     * @param[in] mapArg The keys/values to insert
     */
    void updateParameters(const Parameterised::Map& mapArg);

    /**@brief Returns whether the parameter is known
     * @param[in] key The key to ask for
     * @return Whether the key is known
     */
    bool knowsParameter(const std::string& key) const;

    /**@brief Returns the value for a given key
     * @param[in] key The key to ask for
     * @param[in] defaultValue The default value to return if no value is stored under the key
     * @return The value stored under the key
     */
    virtual const std::string getParameter(const std::string& key, const std::string defaultValue = "") const;

    /**@brief Returns the value for a given key converted to a double
     * @param[in] key The key to ask for
     * @param[in] defaultValue The default value to return if no value is stored under the key
     * @return The value stored under the key
     */
    double getDouble(const std::string& key, const double defaultValue) const;

    /**@brief Returns the value for a given key converted to a list of doubles
     * @param[in] key The key to ask for
     * @param[in] defaultValue The default value to return if no value is stored under the key
     * @return The value stored under the key
     */
    std::vector<double> getDoubles(const std::string& key, std::vector<double> defaultValue = std::vector<double>()) const;

    /// @brief Clears the parameter map
    void clearParameter();

    /// @brief Returns the inner key/value map
    const Parameterised::Map& getParametersMap() const;

    /// @brief Returns the inner key/value map in string format "key1=value1|key2=value2|...|keyN=valueN"
    std::string getParametersStr(const std::string kvsep = "=", const std::string sep = "|") const;

    /// @brief set the inner key/value map in map<string, string> format
    void setParameters(const Parameterised& params);

    /// @brief set the inner key/value map in map<string, string> format
    void setParametersMap(const Parameterised::Map& paramsMap);

    /**@brief set the inner key/value map in string format "key1=value1|key2=value2|...|keyN=valueN"
     * @param[in] paramsString A serialized key-value map
     * @param[in] kvsep The separater between key and value
     * @param[in] sep The separater between map entries
     */
    void setParametersStr(const std::string& paramsString, const std::string kvsep = "=", const std::string sep = "|");

    /// @brief write Params in the given outputdevice
    void writeParams(OutputDevice& device) const;

    /// @brief check if given string can be parsed to a parameters map "key1=value1|key2=value2|...|keyN=valueN"
    static bool areParametersValid(const std::string& value, bool report = false, const std::string kvsep = "=", const std::string sep = "|");

    /// @brief check if given string can be parsed to an attributes map "key1=value1|key2=value2|...|keyN=valueN" (used in generic datas)
    static bool areAttributesValid(const std::string& value, bool report = false, const std::string kvsep = "=", const std::string sep = "|");

private:
    /// @brief check if given string can be parsed to a parameter of type "key=value"
    static bool isParameterValid(const std::string& value, const std::string& kvsep, const std::string& sep);

    /// @brief The key->value map
    Parameterised::Map myMap;
};
