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
// Revision 1.1  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_NodesToTrafficLights.h"
#include "NIVisumTL.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_NodesToTrafficLights::NIVisumParser_NodesToTrafficLights(NIVisumLoader &parent,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs)
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
		myNIVisumTLs[TrafficLight]->GetNodes()->push_back(NBNodeCont::retrieve(Node));
    } catch (OutOfBoundsException) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "NumberFormat");
    } catch (UnknownElement) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_NodesToTrafficLights.icc"
//#endif

// Local Variables:
// mode:C++
// End:
