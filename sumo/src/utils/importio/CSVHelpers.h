#ifndef CSVHelpers_h
#define CSVHelpers_h
/***************************************************************************
                          CSVHelpers.h
			  Some helping functions to read csv-files
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.4  2005/10/07 11:46:34  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 12:20:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2004/03/19 13:02:06  dkrajzew
// some style adaptions
//
// Revision 1.1  2004/01/26 07:22:46  dkrajzew
// added a class theat allows to use csv-files more easily
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class LineHandler;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class CSVHelpers
 * This class offers helper methods to read values from csv-files
 */
class CSVHelpers {
public:
    static void runParser(LineHandler &lh, const std::string &file);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
