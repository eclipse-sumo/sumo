/***************************************************************************
                          NIVissimSingleTypeParser_Kantensperrung.cpp

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
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Kantensperrung.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Kantensperrung::NIVissimSingleTypeParser_Kantensperrung(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Kantensperrung::~NIVissimSingleTypeParser_Kantensperrung()
{
}


bool
NIVissimSingleTypeParser_Kantensperrung::parse(std::istream &from)
{
    string tag;
    from >> tag;
    //
    string id;
    from >> id;
    //
    from >> tag;
    from >> tag;
    int from_node;
    from >> from_node;
    //
    from >> tag;
    from >> tag;
    int to_node;
    from >> to_node;
    //
    from >> tag;
    from >> tag;
    IntVector edges;
    while(tag!="DATAEND") {
        tag = readEndSecure(from);
        if(tag!="DATAEND") {
            edges.push_back(TplConvert<char>::_2int(tag.c_str()));
        }
    }
    NIVissimClosures::dictionary(id, from_node, to_node, edges);
    return true;
}

