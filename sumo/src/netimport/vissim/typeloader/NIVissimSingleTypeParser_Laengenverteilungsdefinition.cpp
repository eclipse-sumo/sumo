/***************************************************************************
                          NIVissimSingleTypeParser_Laengenverteilungsdefinition.cpp

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
// Revision 1.2  2003/03/18 13:11:53  dkrajzew
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
#include <utils/geom/Position2DVector.h>
#include "../NIVissimLoader.h"
#include <utils/distribution/Distribution_Points.h>
#include <netbuild/NBDistribution.h>
#include "NIVissimSingleTypeParser_Laengenverteilungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Laengenverteilungsdefinition::NIVissimSingleTypeParser_Laengenverteilungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Laengenverteilungsdefinition::~NIVissimSingleTypeParser_Laengenverteilungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Laengenverteilungsdefinition::parse(std::istream &from)
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
    NBDistribution::dictionary("length",
        id, new Distribution_Points(id, points));
    return true;
}




