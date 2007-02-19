/****************************************************************************/
/// @file    OptionsIO.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Loads the configuration file using "OptionsLoader"
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
#ifndef OptionsIO_h
#define OptionsIO_h
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


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsIO
 * A wrapper for the complete IO of options containing the reading of a
 * configuration file and the parsing of the command line arguments.
 * The only accessable method returns fale, when something failed during the
 * process. This may happen when the configuration file is broken or its
 * or the command line arguments syntax is invalid. This may also happen
 * when the configuration or the command line arguments do specify a value
 * more than once (independent of each other as the command line arguments
 * may overwrite the configuration settings)
 */
class OptionsIO
{
public:
    /** loads the configuration and parses the command line arguments */
    static bool getOptions(bool loadConfig, OptionsCont *oc,
                           int argv, char **argc);

    /** loads and parses the configuration */
    static bool loadConfiguration(OptionsCont *oc);

};


#endif

/****************************************************************************/

