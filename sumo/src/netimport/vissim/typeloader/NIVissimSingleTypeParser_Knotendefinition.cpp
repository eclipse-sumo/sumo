/***************************************************************************
                          NIVissimSingleTypeParser_Knotendefinition.cpp

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
// Revision 1.4  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
// Revision 1.3  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
//
// Revision 1.2  2003/03/03 15:01:30  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimNodeParticipatingEdge.h"
#include "../tempstructs/NIVissimNodeParticipatingEdgeVector.h"
#include "../tempstructs/NIVissimNodeDef_Edges.h"
#include "../tempstructs/NIVissimNodeDef_Poly.h"
#include "../tempstructs/NIVissimNodeDef.h"
#include "NIVissimSingleTypeParser_Knotendefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Knotendefinition::NIVissimSingleTypeParser_Knotendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Knotendefinition::~NIVissimSingleTypeParser_Knotendefinition()
{
}


bool
NIVissimSingleTypeParser_Knotendefinition::parse(std::istream &from)
{
	//
    int id;
    from >> id;
    //
    string tag;
    from >> tag;
    string name = readName(from);
    //
    tag = overrideOptionalLabel(from);
    //
    while(tag!="netzausschnitt") {
        tag = myRead(from);
    }
    //
    tag = myRead(from);
    if(tag=="strecke") {
        NIVissimNodeParticipatingEdgeVector edges;
        while(tag=="strecke") {
            int edgeid;
            double from_pos, to_pos;
            from_pos = to_pos = -1.0;
            from >> edgeid;
            tag = readEndSecure(from, "strecke");
            if(tag=="von") {
                from >> from_pos; // type-checking is missing!
                from >> tag;
                from >> to_pos; // type-checking is missing!
                tag = readEndSecure(from, "strecke");
            }
            edges.push_back(new NIVissimNodeParticipatingEdge(edgeid, from_pos, to_pos));
        }
        NIVissimNodeDef_Edges::dictionary(id, name, edges);
    } else {
        int no = TplConvert<char>::_2int(tag.c_str());
        Position2DVector poly;
        for(int i=0; i<no; i++) {
            poly.push_back(getPosition2D(from));
        }
        NIVissimNodeDef_Poly::dictionary(id, name, poly);
    }
    return true;
}

