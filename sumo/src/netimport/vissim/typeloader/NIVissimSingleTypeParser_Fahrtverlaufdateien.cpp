/***************************************************************************
    NIVissimSingleTypeParser_Fahrtverlaufdateien.cpp

                             -------------------
    begin                : Wed, 30 Apr 2003
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
// Revision 1.5  2005/10/07 11:40:29  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
//
// Revision 1.4  2005/09/09 12:52:11  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.3  2005/06/14 13:49:18  dksumo
// code style adapted
//
// Revision 1.2  2005/04/26 07:54:05  dksumo
// level3 warnings patched; made containers non-static
//
// Revision 1.1.2.1  2005/04/15 09:56:30  dksumo
// level3 warnings removed; made the containers unstatic
//
// Revision 1.1  2004/10/22 12:50:04  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.1  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

#include <iostream>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Fahrtverlaufdateien.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Fahrtverlaufdateien::NIVissimSingleTypeParser_Fahrtverlaufdateien(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Fahrtverlaufdateien::~NIVissimSingleTypeParser_Fahrtverlaufdateien()
{
}


bool
NIVissimSingleTypeParser_Fahrtverlaufdateien::parse(std::istream &)
{
    return true;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
