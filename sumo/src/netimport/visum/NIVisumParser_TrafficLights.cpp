/***************************************************************************
                          NIVisumParser_TrafficLights.cpp
			  Parser for visum-TrafficLights
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
// Revision 1.2  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.1  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_TrafficLights.h"
#include "NIVisumTL.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_TrafficLights::NIVisumParser_TrafficLights(NIVisumLoader &parent,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs)
{
}


NIVisumParser_TrafficLights::~NIVisumParser_TrafficLights()
{
}


void
NIVisumParser_TrafficLights::myDependentReport()
{
	std::string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // cycle time
        double CycleTime = TplConvert<char>::_2float(myLineParser.get("Umlaufzeit").c_str());
		// IntermediateTime
        double IntermediateTime = TplConvert<char>::_2float(myLineParser.get("StdZwischenzeit").c_str());
		// PhaseBased
        bool PhaseBased = TplConvert<char>::_2bool(myLineParser.get("PhasenBasiert").c_str());
        // add to the list
		myNIVisumTLs[id] = new NIVisumTL(id, CycleTime, IntermediateTime, PhaseBased);
    } catch (OutOfBoundsException) {
        addError2("LSA", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("LSA", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("LSA", id, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_TrafficLights.icc"
//#endif

// Local Variables:
// mode:C++
// End:
