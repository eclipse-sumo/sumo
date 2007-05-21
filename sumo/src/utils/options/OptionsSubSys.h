/****************************************************************************/
/// @file    OptionsSubSys.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id$
///
// Static methods for options initialisation and parsing
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
#ifndef OptionsSubSys_h
#define OptionsSubSys_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// abstract method definitions
// ===========================================================================
typedef void(fill_options)(OptionsCont &);
typedef bool(check_options)(OptionsCont &);


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsSubSys
 * @brief Static methods for options initialisation and parsing
 *
 * This shall not only allow an easier access to the command line options,
 * but also generalise the performing of option-related functions common
 * to all applications such as checking whether to print the help-screen,
 * validating the input etc.
 */
class OptionsSubSys
{
public:
    /** @brief Initialises the options container
     *
     * fill_f is used to fill the given OptionsCont with values, check_f
     * to validate them. This method performs the parsing of options and
     * also other options-related function (printing the help screen etc.) */
    static bool init(bool loadConfig, int argc, char **argv,
                     fill_options *fill_f, check_options *check_f=0);

    /** @brief Initialisation method for gui-based applications
     *
     * We may be sure that we set our options when performing gui-based
     * options processes right (not via the command line), so checking
     * is left out here; we also do not have to parse command line options
     * or to print anything.
     *
     * The name will be inserted as the name of the configuration to load. */
    static bool guiInit(fill_options *fill_f,
                        const std::string &optionName, const std::string &optionValue);

    /** @brief Retrieves the options
     * 
     * They should be initialised before. */
    static OptionsCont &getOptions();

    /// Clears all previously set options
    static void close();

    /// Returns the information whether the given option is set for the given id
    static bool helper_CSVOptionMatches(const std::string &optionName,
                                        const std::string &itemName);

protected:
    /// The static options container used
    static OptionsCont myOptions;

protected:
    /// Invalidated constructor
    OptionsSubSys();

    /// Invalidated destructor
    ~OptionsSubSys();

};


#endif

/****************************************************************************/

