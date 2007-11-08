/****************************************************************************/
/// @file    LineHandler.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// An interface definition for a class which retrieves
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
#ifndef LineHandler_h
#define LineHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LineHandler
 * The LineHandler is an interface for a class which retrieves information
 * from a file linewise. The lines are suppoted to this class using the
 * "report"-method. The LineHandler is used together with the LineReader
 * which reads the lines from a file.
 */
class LineHandler
{
public:
    /// constructor
    LineHandler() { }

    /// virtual destructor
    virtual ~LineHandler() { }

    /** real virtual interface method (to LineReader)
        retrieves lines from a file */
    virtual bool report(const std::string &result) = 0;

};


#endif

/****************************************************************************/

