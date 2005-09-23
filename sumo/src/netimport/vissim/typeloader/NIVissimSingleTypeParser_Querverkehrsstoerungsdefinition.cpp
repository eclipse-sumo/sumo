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
// Revision 1.7  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/04/27 12:24:38  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2003/07/07 08:29:54  dkrajzew
// Warnings are now reported to the MsgHandler
//
// Revision 1.4  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
// Revision 1.3  2003/03/20 16:32:24  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 16:26:56  dkrajzew
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimExtendedEdgePoint.h"
#include "../tempstructs/NIVissimDisturbance.h"
#include "NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h"

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
    MsgHandler::getErrorInstance()->inform(
        "NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition: format problem");
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
        SUMOReal timegap;
        from >> timegap;

        from >> tag;
        SUMOReal waygap;
        from >> waygap;

        SUMOReal vmax = -1;
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
        SUMOReal timegap;
        from >> timegap;

        SUMOReal waygap;
        from >> tag;
        from >> waygap;

        SUMOReal vmax = -1;
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
    from >> edgeid; // type-checking is missing!
    //
    string tag;
    int laneno;
    from >> tag;
    from >> laneno; // type-checking is missing!
    IntVector lanes;
    lanes.push_back(laneno);
    //
    SUMOReal position;
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
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
