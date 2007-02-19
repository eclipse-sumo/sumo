/****************************************************************************/
/// @file    NIOptionsIO.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A class for the initialisation, input and veryfying of the
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
#ifndef NIOptionsIO_h
#define NIOptionsIO_h
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
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * NIOptionsIO
 * This class is used to build the container of possible commands
 * (OptionsCont-typed) and process reading of options from the command
 * line and the configuration file. The options are also veryfied inside
 * this class
 */
class NIOptionsIO
{
public:
    static void fillOptions(OptionsCont &oc);

    static bool checkOptions(OptionsCont &oc);

private:

    /** checks whether the given option settings are ok */
    static bool check(OptionsCont &oc);

    /** checks whether all needed informations are given */
    static bool checkCompleteDescription(OptionsCont &oc);

    /** checks whether the node file definition is given */
    static bool checkNodes(OptionsCont &oc);

    /** checks whether the edge file definition is given */
    static bool checkEdges(OptionsCont &oc);

    /** checks whether the output can be build */
    static bool checkOutput(OptionsCont &oc);

};


#endif

/****************************************************************************/

