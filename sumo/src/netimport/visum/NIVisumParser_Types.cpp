/***************************************************************************
                          NIVisumParser_Types.cpp
			  Parser for visum-road types
                             -------------------
    project              : SUMO
    begin                : Thu, 14 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBType.h>
#include <netbuild/NBTypeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Types.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_Types::NIVisumParser_Types(NIVisumLoader &parent,
        const std::string &dataName,
        NBCapacity2Lanes &cap2lanes)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
    myCap2Lanes(cap2lanes)
{
}


NIVisumParser_Types::~NIVisumParser_Types()
{
}


void
NIVisumParser_Types::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the maximum speed
        float speed = TplConvert<char>::_2float(myLineParser.get("v0-IV").c_str());
        // get the priority
        int priority = TplConvert<char>::_2int(myLineParser.get("Rang").c_str());
        // try to retrieve the number of lanes
        int nolanes = myCap2Lanes.get(
            TplConvert<char>::_2float(myLineParser.get("Kap-IV").c_str()));
        // insert the type
        NBType *type = new NBType(id, nolanes, speed/3.6, 100-priority);
        if(!NBTypeCont::insert(type)) {
            addError(
                string(" Duplicate type occured ('") + id + string("')."));
            delete type;
        }
    } catch (OutOfBoundsException) {
        addError2("STRECKENTYP", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("STRECKENTYP", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("STRECKENTYP", id, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_Types.icc"
//#endif

// Local Variables:
// mode:C++
// End:


