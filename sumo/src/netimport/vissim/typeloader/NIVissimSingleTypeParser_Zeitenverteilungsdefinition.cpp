/***************************************************************************
                          NIVissimSingleTypeParser_Zeitenverteilungsdefinition.cpp

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
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
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
#include "../NIVissimLoader.h"
#include <utils/distribution/Distribution_Points.h>
#include <utils/distribution/Distribution_MeanDev.h>
#include <netbuild/NBDistribution.h>
#include "NIVissimSingleTypeParser_Zeitenverteilungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Zeitenverteilungsdefinition::NIVissimSingleTypeParser_Zeitenverteilungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Zeitenverteilungsdefinition::~NIVissimSingleTypeParser_Zeitenverteilungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Zeitenverteilungsdefinition::parse(std::istream &from)
{
    // id
    string id;
    from >> id;
    // list of points
    Position2DVector points;
    string tag;
    do {
        tag = readEndSecure(from);
        if(tag=="mittelwert") {
            double mean, deviation;
            from >> mean;
            from >> tag;
            from >> deviation;
            return NBDistribution::dictionary("times", id,
                new Distribution_MeanDev(id, mean, deviation));
        }
        if(tag!="DATAEND") {
            double p1 = TplConvert<char>::_2float(tag.c_str());
            from >> tag;
            double p2 = TplConvert<char>::_2float(tag.c_str());
            points.push_back(Position2D(p1, p2));
        }
    } while(tag!="DATAEND");
    return NBDistribution::dictionary("times",
        id, new Distribution_Points(id, points));
}

