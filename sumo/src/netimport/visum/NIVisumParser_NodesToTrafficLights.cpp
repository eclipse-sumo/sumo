/***************************************************************************
                          NIVisumParser_NodesToTrafficLights.cpp
			  Parser for visum- NodesToTrafficLights relation
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_NodesToTrafficLights.h"
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
NIVisumParser_NodesToTrafficLights::NIVisumParser_NodesToTrafficLights(
    NIVisumLoader &parent, NBNodeCont &nc,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs), myNodeCont(nc)
{
}


NIVisumParser_NodesToTrafficLights::~NIVisumParser_NodesToTrafficLights()
{
}


void
NIVisumParser_NodesToTrafficLights::myDependentReport()
{
	std::string TrafficLight;
	std::string Node;
    try {
		Node = myLineParser.get("KnotNr").c_str();
		TrafficLight = myLineParser.get("LsaNr").c_str();
        // add to the list
		myNIVisumTLs[TrafficLight]->GetNodes()->push_back(myNodeCont.retrieve(Node));
    } catch (OutOfBoundsException) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "NumberFormat");
    } catch (UnknownElement) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
