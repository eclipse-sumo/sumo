/****************************************************************************/
/// @file    OptionsIO.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Helper for parsing command line arguments and reading configuration files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OptionsIO_h
#define OptionsIO_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsIO
 *
 * Helping methods for parsing of command line arguments and reading a
 *  configuration file.
 * Any errors are reported by throwing a ProcessError exception which
 *  contains a description about the failure.
 */
class OptionsIO {
public:
    /** @brief Stores the command line arguments for later parsing
     *
     * @param[in] argc number of arguments given at the command line
     * @param[in] argv arguments given at the command line
     */
    static void setArgs(int argc, char** argv);


    /** @brief Parses the command line arguments and loads the configuration
     *
     * Command line arguments are parsed, first, throwing a ProcessError
     *  if something fails. Then options are reset to being writeable and the
     *  configuration is loaded using "loadConfiguration". After this,
     *  the options are reset again and the command line arguments are
     *  reparsed.
     *
     * This workflow allows to read the name of a configuration file from
     *  command line arguments, first, then to load values from this configuration
     *  file and reset them by other values from the command line.
     */
    static void getOptions(const bool commandLineOnly = false);


    /** @brief Loads and parses the configuration
     *
     * The name of the configuration file is extracted from the global
     *  OptionsCont ("configuration-file" is used as the name of the option to get
     *  the name of the configuration).
     */
    static void loadConfiguration();


private:
    /** @brief Retrieves the XML root element of a supposed configuration or net
     *
     * @param[in] filename the XML file to parse
     * @return the root element if any
     */
    static std::string getRoot(const std::string& filename);


private:
    static int myArgC;
    static char** myArgV;

};


#endif

/****************************************************************************/

