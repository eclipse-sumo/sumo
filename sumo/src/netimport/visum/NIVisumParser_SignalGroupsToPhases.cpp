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
#include "NIVisumParser_SignalGroupsToPhases.h"
#include "NIVisumTL.h"


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
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_SignalGroupsToPhases.icc"
//#endif

// Local Variables:
// mode:C++
// End:
