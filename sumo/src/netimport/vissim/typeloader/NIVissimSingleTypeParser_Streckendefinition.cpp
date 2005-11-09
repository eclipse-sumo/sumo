/***************************************************************************
                          NIVissimSingleTypeParser_Streckendefinition.cpp

                             -------------------
    begin                : Wed, 18 Dec 2002
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
// Revision 1.12  2005/11/09 06:42:07  dkrajzew
// complete geometry building rework (unfinished)
//
// Revision 1.11  2005/10/07 11:40:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.8  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.7  2003/07/30 09:24:03  dkrajzew
// reinserted the needed but removed check
//
// Revision 1.6  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.5  2003/05/20 09:42:38  dkrajzew
// all data types implemented
//
// Revision 1.4  2003/03/20 16:32:24  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/18 13:11:53  dkrajzew
// debugging
//
// Revision 1.2  2003/03/06 16:26:58  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
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
#include <utils/common/IntVector.h>
#include <utils/geom/Position2DVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimEdge.h"
#include "../tempstructs/NIVissimClosedLaneDef.h"
#include "../tempstructs/NIVissimClosedLanesVector.h"
#include "NIVissimSingleTypeParser_Streckendefinition.h"

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
NIVissimSingleTypeParser_Streckendefinition::NIVissimSingleTypeParser_Streckendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Streckendefinition::~NIVissimSingleTypeParser_Streckendefinition()
{
}


bool
NIVissimSingleTypeParser_Streckendefinition::parse(std::istream &from)
{
    // read in the id
	int id;
    from >> id;
    //
    string tag;
    // the following elements may occure: "Name", "Beschriftung", "Typ",
    //  followed by the mandatory "Laenge"
    string name, label, type;
    SUMOReal length = -1;
    while(length<0) {
        tag = overrideOptionalLabel(from);
        if(tag=="name") {
            name = readName(from);
        } else if(tag=="typ") {
            type = myRead(from);
        } else if(tag=="laenge") {
            from >> length; // type-checking is missing!
        }
    }
    // read in the number of lanes
    int noLanes;
    tag = myRead(from);
    from >> noLanes;
    // skip some parameter, except optional "Zuschlag" until "Von" (mandatory)
    //  occures
    SUMOReal zuschlag1, zuschlag2;
    zuschlag1 = zuschlag2 = 0;
    while(tag!="von") {
        tag = myRead(from);
        if(tag=="zuschlag") {
            from >> zuschlag1; // type-checking is missing!
            tag = myRead(from);
            if(tag=="zuschlag") {
                from >> zuschlag2; // type-checking is missing!
            }
        }
    }
    // Read the geometry information
    Position2DVector geom;
    while(tag!="nach") {
        geom.push_back_noDoublePos(getPosition2D(from));
        tag = myRead(from);
        try {
            SUMOReal tmp = TplConvert<char>::_2SUMOReal(tag.c_str());
            tag = myRead(from);
        } catch (NumberFormatException &) {
        }
    }
    geom.push_back_noDoublePos(getPosition2D(from));
    // Read definitions of closed lanes
    NIVissimClosedLanesVector clv;
    // check whether a next close lane definition can be found
    tag = readEndSecure(from);
    while(tag!="DATAEND") {
        if(tag=="spur") {
            // get the lane number
            int laneNo;
            from >> laneNo; // type-checking is missing!
            // get the list of assigned car classes
            IntVector assignedVehicles;
            tag = myRead(from);
            tag = myRead(from);
            while(tag!="DATAEND"&&tag!="spur") {
                int classes = TplConvert<char>::_2int(tag.c_str());
                assignedVehicles.push_back(classes);
                tag = readEndSecure(from);
            }
            // build and add the definition
            NIVissimClosedLaneDef *cld = new NIVissimClosedLaneDef(laneNo, assignedVehicles);
            clv.push_back(cld);
        } else {
            tag = readEndSecure(from);
        }
    }
    NIVissimEdge *e = new NIVissimEdge(id, name, type, noLanes,
        zuschlag1, zuschlag2, length, geom, clv);
    if(!NIVissimEdge::dictionary(id, e)) {
        return false;
    }
    return true;
    //return NIVissimAbstractEdge::dictionary(id, e);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
