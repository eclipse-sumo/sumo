/****************************************************************************/
/// @file    OptionsParser.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Parses command line arguments
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OptionsParser_h
#define OptionsParser_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


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
     * @param[in] argc The number of given command line arguments
     * @param[in] argv The command line arguments
     * @return Whether the parsing was successfull
     * @exception InvalidArgument If a performed setting of an option failed (see Option::set)
     */
    static bool parse(int argc, char **argv);

private:
    /** @brief parses the previous arguments
     *
     * @param[in] arg1 The first token to parse
     * @param[in] arg2 The second token to parse, 0 if there is none
     * @param[in, out] ok Whether the parsing was successfull
     * @return Number of read tokens (1 or 2)
     * @exception InvalidArgument If a performed setting of an option failed (see Option::set)
     */
    static int check(char *arg1, char *arg2, bool &ok);


    /** @brief Returns the whether the given token is an option
     *
     * The given token is assumed to be an option if it starts with a '-'.
     *
     * @param[in] arg1 The token to check
     * @return Whether the token is an option
     */
    static bool checkParameter(char *arg1);


    /** @brief returns the whether the given token is an abbreviation
     *
     * The given token is assumed to be an option if it starts with two '-'.
     *
     * @param[in] arg1 The token to check
     * @return Whether the token is an abbreviation
     */
    static bool isAbbreviation(char *arg1);


    /** @brief Converts char* to string
     *
     * @param[in] arg The c-string to convert
     * @return The string converted into a std::string
     */
    static std::string convert(char *arg);


    /** @brief converts char to string
     *
     * @param[in] abbr The char to convert
     * @return The char converted into a std::string
     */
    static std::string convert(char abbr);


    /** @brief Extracts the parameter directly attached to an option
     *
     * Parses single tokens which contain an option and the parameter
     *  (like -c=myconfig.cfg)
     *
     * @param[in] oc The container to store the result into
     * @param[in] arg The token to parse
     * @exception InvalidArgument If a performed setting of an option failed (see Option::set)
     */
    static bool processNonBooleanSingleSwitch(OptionsCont &oc, char *arg);


};


#endif

/****************************************************************************/

