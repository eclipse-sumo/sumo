/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    OptionsParser.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
///
// Parses command line arguments
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsParser
 * @brief Parses command line arguments
 *
 * The only public method parses the given list of arguments. It returns false
 *  when something failed. This may happen if the syntax of the arguments is
 *  invalid, a value is tried to be set several times or an unknown option
 *  is tried to be set.
 *
 * The class assumes all options are unset or using default values only.
 */
class OptionsParser {
public:
    /** @brief Parses the given command line arguments
     *
     * @param[in] oc The options container to fill
     * @param[in] args The command line arguments
     * @return Whether the parsing was successfull
     * @exception InvalidArgument If a performed setting of an option failed (see Option::set)
     */
    static bool parse(const std::vector<std::string>& args, const bool ignoreAppenders = false);

private:
    /** @brief parses the previous arguments
     *
     * @param[in] arg1 The first token to parse
     * @param[in] arg2 The second token to parse, 0 if there is none
     * @param[in, out] ok Whether the parsing was successfull
     * @return Number of read tokens (1 or 2)
     * @exception InvalidArgument If a performed setting of an option failed (see Option::set)
     */
    static int check(const std::string& arg1, const std::string* const arg2, bool& ok, const bool ignoreAppenders);


    /** @brief Returns the whether the given token is an option
     *
     * The given token is assumed to be an option if it starts with a '-' or a '+'.
     *
     * @param[in] arg1 The token to check
     * @return Whether the token is an option
     */
    static bool checkParameter(const std::string& arg1);


    /** @brief Extracts the parameter directly attached to an option
     *
     * Parses single tokens which contain an option and the parameter
     *  (like -c=myconfig.cfg)
     *
     * @param[in] oc The container to store the result into
     * @param[in] arg The token to parse
     * @exception InvalidArgument If a performed setting of an option failed (see Option::set)
     */
    static bool processNonBooleanSingleSwitch(OptionsCont& oc, const std::string& arg, const bool append);


};
