/***************************************************************************
                          CVSHelpers.cpp
			  Some helping functions to read csv-files
                             -------------------
    project              : SUMO
    begin                : Thu, 22 Jan 2004
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2004/03/19 13:02:06  dkrajzew
// some style adaptions
//
// Revision 1.1  2004/01/26 07:22:46  dkrajzew
// added a class theat allows to use csv-files more easily
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "CSVHelpers.h"
#include "LineHandler.h"
#include "LineReader.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
CSVHelpers::runParser(LineHandler &lh, const std::string &file)
{
    LineReader lr(file);
    lr.readAll(lh);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
