/***************************************************************************
                          NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.cpp

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
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/distribution/DistributionCont.h>
#include <utils/distribution/Distribution_Points.h>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::~NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::parse(std::istream &from)
{
	// id
    string id;
    from >> id;
    // list of points
    Position2DVector points;
    string tag;
    do {
        tag = readEndSecure(from);
        if(tag!="DATAEND") {
            double p1 = TplConvert<char>::_2float(tag.c_str());
            from >> tag;
            double p2 = TplConvert<char>::_2float(tag.c_str());
            points.push_back(Position2D(p1, p2));
        }
    } while(tag!="DATAEND");
    DistributionCont::dictionary("speed",
       id, new Distribution_Points(id, points));
    return true;
}

