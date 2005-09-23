/***************************************************************************
                          NIVisumParser_SignalGroupsToPhases.cpp
			  Parser for visum-SignalGroupsToPhases
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_SignalGroupsToPhases.h"
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
NIVisumParser_SignalGroupsToPhases::NIVisumParser_SignalGroupsToPhases(NIVisumLoader &parent,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs)
{
}


NIVisumParser_SignalGroupsToPhases::~NIVisumParser_SignalGroupsToPhases()
{
}


void
NIVisumParser_SignalGroupsToPhases::myDependentReport()
{
	std::string Phaseid;
	std::string LSAid;
	std::string SGid;
    try {
        // get the id
        Phaseid = NBHelpers::normalIDRepresentation(myLineParser.get("PsNr"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        SGid = NBHelpers::normalIDRepresentation(myLineParser.get("SGNR"));
		// insert
		NIVisumTL::Phase *PH;
		NIVisumTL::SignalGroup *SG;
		NIVisumTL *LSA;
		LSA = (*myNIVisumTLs.find(LSAid)).second;
		SG = LSA->GetSignalGroup(SGid);
		PH = (*LSA->GetPhases()->find(Phaseid)).second;
		(*SG->GetPhases())[Phaseid] = PH;
    } catch (OutOfBoundsException) {
        addError2("SignalGroupsToPhases", "LSA:" + LSAid + " Phase:" + Phaseid, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("SignalGroupsToPhases", "LSA:" + LSAid + " Phase:" + Phaseid, "NumberFormat");
    } catch (UnknownElement) {
        addError2("SignalGroupsToPhases", "LSA:" + LSAid + " Phase:" + Phaseid, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
