/***************************************************************************
                          NIVissimSingleTypeParser_VWunschentscheidungsdefinition.cpp

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
// Revision 1.5  2004/01/28 12:38:45  dkrajzew
// work on reading and setting speeds in vissim-networks
//
// Revision 1.4  2003/11/11 08:24:52  dkrajzew
// debug values removed
//
// Revision 1.3  2003/10/30 09:13:00  dkrajzew
// further work on vissim-import
//
// Revision 1.2  2003/10/27 10:52:41  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <vector>
#include <cassert>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimEdge.h"
#include "../tempstructs/NIVissimConnection.h"
#include "NIVissimSingleTypeParser_VWunschentscheidungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_VWunschentscheidungsdefinition::NIVissimSingleTypeParser_VWunschentscheidungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_VWunschentscheidungsdefinition::~NIVissimSingleTypeParser_VWunschentscheidungsdefinition()
{
}


bool
NIVissimSingleTypeParser_VWunschentscheidungsdefinition::parse(std::istream &from)
{
    string tag;
    from >> tag; // id
    from >> tag; // name
    tag = readName(from);
    tag = overrideOptionalLabel(from);
    from >> tag; // strecke
    string edgeid;
    from >> edgeid;
    from >> tag; // spur
    string lane;
    from >> lane;
    from >> tag; // bei
    string pos;
    from >> pos;
    from >> tag; // fahrzeugklasse
    from >> tag; // <fahrzeugklasse>
    from >> tag; // vwunsch
    string vwunsch;
    from >> vwunsch; // vwunsch
    std::vector<std::string> tmp;
    tmp.push_back("zeit");
    tmp.push_back("fahrzeugklasse");
    tag = readEndSecure(from, tmp);
    while(tag!="DATAEND"&&tag!="zeit") {
        from >> tag;
        from >> tag;
        from >> tag;
        tag = myRead(from);
    }
    if(tag=="zeit") {
        from >> tag;
        from >> tag;
        from >> tag;
        from >> tag;
    }
    int numid = TplConvert<char>::_2int(edgeid.c_str());
    int numlane = TplConvert<char>::_2int(lane.c_str()) - 1;
    int numv = TplConvert<char>::_2int(vwunsch.c_str());
    NIVissimEdge *e = NIVissimEdge::dictionary(numid);
    if(e==0) {
        NIVissimConnection *c = NIVissimConnection::dictionary(numid);
        const IntVector &lanes = c->getToLanes();
        e = NIVissimEdge::dictionary(c->getToEdgeID());
        for(IntVector::const_iterator j=lanes.begin(); j!=lanes.end(); j++) {
            e->setSpeed((*j), numv);
        }
        assert(e!=0);
    } else {
        e->setSpeed(numlane, numv);
    }
    return true;
}

