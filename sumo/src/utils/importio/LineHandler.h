#ifndef LineHandler_h
#define LineHandler_h
/***************************************************************************
                          LineHandler.h
              An interface definition for a class which retrieves
              information from a file linewise
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2005/09/15 12:20:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/11/23 10:35:28  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:51:26  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:59:12  dkrajzew
// initial commit for classes that handle import functions
//
// Revision 1.1  2002/07/25 08:55:42  dkrajzew
// support for Visum7.5 & Cell import added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class LineHandler
 * The LineHandler is an interface for a class which retrieves information
 * from a file linewise. The lines are suppoted to this class using the
 * "report"-method. The LineHandler is used together with the LineReader
 * which reads the lines from a file.
 */
class LineHandler {
public:
    /// constructor
    LineHandler() { }

    /// virtual destructor
    virtual ~LineHandler() { }

    /** real virtual interface method (to LineReader)
        retrieves lines from a file */
    virtual bool report(const std::string &result) = 0;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
