/***************************************************************************
                          NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.cpp

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
// Revision 1.3  2003/03/20 16:32:24  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 16:26:56  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimExtendedEdgePoint.h"
#include "../tempstructs/NIVissimDisturbance.h"
#include "NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::~NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parse(std::istream &from)
{
    string tag;
	tag = myRead(from);
    if(tag=="nureigenestrecke") {
        return parseOnlyMe(from);
    } else if(tag=="ort") {
        return parsePositionDescribed(from);
    } else if(tag=="nummer") {
        return parseNumbered(from);
    }
    cout << "NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition: format problem" << endl;
    throw 1;
}

bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parseOnlyMe(std::istream &from)
{
    return true;
}


bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parsePositionDescribed(std::istream &from)
{
    string tag = myRead(from);
    NIVissimExtendedEdgePoint edge = parsePos(from);
//    from >> tag; // "Durch"
    bool ok = true;
    do {
        from >> tag; // "Strecke"
        NIVissimExtendedEdgePoint by = parsePos(from);
        //
        double timegap;
        from >> timegap;

        from >> tag;
        double waygap;
        from >> waygap;

        double vmax = -1;
        tag = readEndSecure(from);
        if(tag=="vmax") {
            from >> vmax;
        }
        ok = NIVissimDisturbance::dictionary(-1, "", edge, by,
            timegap, waygap, vmax);
        if(tag!="DATAEND") {
            tag = readEndSecure(from);
        }
    } while(tag!="DATAEND"&&ok);
    return ok;
}



bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parseNumbered(std::istream &from)
{
    //
    int id;
    from >> id;
    //
    string tag;
    from >> tag;
    string name = readName(from);
    // skip optional "Beschriftung"
    while(tag!="ort") {
        tag = myRead(from);
    }
    //
    from >> tag; // "Strecke"
    NIVissimExtendedEdgePoint edge = parsePos(from);
    bool ok = true;
    do {
        from >> tag; // "Ort"
        from >> tag; // "Strecke"
        NIVissimExtendedEdgePoint by = parsePos(from);
        //
        double timegap;
        from >> timegap;

        double waygap;
        from >> tag;
        from >> waygap;

        double vmax = -1;
        tag = readEndSecure(from);
        if(tag=="vmax") {
            from >> vmax;
        }

        ok = NIVissimDisturbance::dictionary(id, name, edge, by,
            timegap, waygap, vmax);
        if(tag!="DATAEND") {
            tag = readEndSecure(from);
        }
    } while(tag!="DATAEND"&&ok);
    return ok;
}



NIVissimExtendedEdgePoint
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parsePos(std::istream &from)
{
    int edgeid;
    from >> edgeid; // !!!
    //
    string tag;
    int laneno;
    from >> tag;
    from >> laneno; // !!!
    IntVector lanes;
    lanes.push_back(laneno);
    //
    double position;
    from >> tag;
    from >> position;
    // assigned vehicle types
    IntVector types;
    from >> tag;
    while(tag!="zeitluecke"&&tag!="durch"&&tag!="DATAEND"&&tag!="alle") {
        tag = readEndSecure(from);
        if(tag!="DATAEND") {
            if(tag=="alle") {
                types.push_back(-1);
                from >> tag;
                tag = "alle";
            } else if(tag!="zeitluecke"&&tag!="durch"&&tag!="DATAEND") {
                int tmp = TplConvert<char>::_2int(tag.c_str());
                types.push_back(tmp);
            }
        }
    }
    return NIVissimExtendedEdgePoint(edgeid, lanes, position, types);
}

