/***************************************************************************
                          FileHelpers.cpp
			  Functions for an easier usage of files
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.3  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/11 13:43:35  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.1  2002/03/20 08:19:06  dkrajzew
// removeDir - method added
//
// Revision 2.0  2002/02/14 14:43:26  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:18  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <cstring>
#include <sys/stat.h>
#include "FileHelpers.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
FileHelpers::exists(string path)
{
    // !!! not really well
    while(path.at(path.length()-1)=='/'||path.at(path.length()-1)=='\\')
        path.erase(path.end()-1);
    if(path.length()==0) return false;
    struct stat st;
    bool ret = ( stat( path.c_str(), &st ) == 0 );
    return ret;
}

std::string
FileHelpers::removeDir(std::string path) {
    size_t beg = path.find_last_not_of("\\/");
    if(beg==string::npos||beg==0)
        return path;
    return path.substr(beg);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "FileHelpers.icc"
//#endif

// Local Variables:
// mode:C++
// End:
