/***************************************************************************
                          NLSourceBuilder.cpp
                          A building helper for the sources
                             -------------------
    begin                : Mon, 22 Jul 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
     const char rcsid[] = "$Id: ";
}
// $Log$
// Revision 1.2  2003/06/06 10:40:18  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.1  2002/07/31 17:34:41  roessel
// Initial sourceforge commit.
//
// Revision 1.2  2002/07/23 06:37:37  dkrajzew
// Single Source Definition Files may now be specified using relative pathnames within the Source Files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fstream>
#include <microsim/MSSource.h>
#include <microsim/MSTriggeredSource.h>
#include <microsim/MSEventControl.h>
#include <utils/common/FileHelpers.h>
#include "NLSourceBuilder.h"

/* =========================================================================
 * method definitions
 * ======================================================================= */
MSSource *
NLSourceBuilder::buildTriggeredSource(const std::string &id,
                                      std::string file,
                                      std::string base)
{
    // check whether absolute or relative filenames are given
    if(FileHelpers::isAbsolute(file)) {
        return new MSTriggeredSource(file);
    } else {
        return new MSTriggeredSource(
            FileHelpers::getConfigurationRelative(base, file));
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLSourceBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
