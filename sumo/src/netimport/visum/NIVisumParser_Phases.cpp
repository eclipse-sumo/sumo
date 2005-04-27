/***************************************************************************
                          NIVisumParser_Phases.cpp
			  Parser for visum-Phases
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
#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Phases.h"
#include "NIVisumTL.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_Phases::NIVisumParser_Phases(NIVisumLoader &parent,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs)
{
}


NIVisumParser_Phases::~NIVisumParser_Phases()
{
}


void
NIVisumParser_Phases::myDependentReport()
{
	std::string Phaseid;
	std::string LSAid;
    try {
        // get the id
        Phaseid = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        // StartTime
        double StartTime = TplConvert<char>::_2float(myLineParser.get("GzStart").c_str());
		// EndTime
        double EndTime = TplConvert<char>::_2float(myLineParser.get("GzEnd").c_str());
		// add to the list
		(*myNIVisumTLs.find(LSAid)).second->AddPhase(Phaseid, StartTime, EndTime);
    } catch (OutOfBoundsException) {
        addError2("Phases", "LSA:" + LSAid + " Phase:" + Phaseid, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("Phases", "LSA:" + LSAid + " Phase:" + Phaseid, "NumberFormat");
    } catch (UnknownElement) {
        addError2("Phases", "LSA:" + LSAid + " Phase:" + Phaseid, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
