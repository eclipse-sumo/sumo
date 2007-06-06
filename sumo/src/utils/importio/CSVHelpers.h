/****************************************************************************/
/// @file    CSVHelpers.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2004
/// @version $Id$
///
// Some helping functions to read csv-files
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
#ifndef CSVHelpers_h
#define CSVHelpers_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class LineHandler;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CSVHelpers
 * This class offers helper methods to read values from csv-files
 */
class CSVHelpers
{
public:
    static void runParser(LineHandler &lh, const std::string &file);
};


#endif

/****************************************************************************/

