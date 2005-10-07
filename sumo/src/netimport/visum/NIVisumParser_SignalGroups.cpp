/***************************************************************************
                          NIVisumParser_SignalGroups.cpp
			  Parser for visum-SignalGroups
                             -------------------
    project              : SUMO
    begin                : Fri, 09 May 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
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
// Revision 1.6  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.1  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
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

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_SignalGroups.h"
#include "NIVisumTL.h"

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
NIVisumParser_SignalGroups::NIVisumParser_SignalGroups(NIVisumLoader &parent,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs)
{
}


NIVisumParser_SignalGroups::~NIVisumParser_SignalGroups()
{
}


void
NIVisumParser_SignalGroups::myDependentReport()
{
	std::string id;
	std::string LSAid;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        // StartTime
        SUMOReal StartTime = TplConvert<char>::_2SUMOReal(myLineParser.get("GzStart").c_str());
		// EndTime
        SUMOReal EndTime = TplConvert<char>::_2SUMOReal(myLineParser.get("GzEnd").c_str());
		// add to the list
		(*myNIVisumTLs.find(LSAid)).second->AddSignalGroup(id, (SUMOTime) StartTime, (SUMOTime) EndTime);
    } catch (OutOfBoundsException) {
        addError2("SignalGroups", "LSA:" + LSAid + " SignalGroup:" + id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("SignalGroups", "LSA:" + LSAid + " SignalGroup:" + id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("SignalGroups", "LSA:" + LSAid + " SignalGroup:" + id, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
