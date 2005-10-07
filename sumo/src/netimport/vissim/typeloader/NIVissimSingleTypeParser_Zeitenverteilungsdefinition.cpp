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
// Revision 1.5  2005/10/07 11:40:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/03/18 13:11:53  dkrajzew
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
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include <utils/distribution/Distribution_Points.h>
#include <utils/distribution/Distribution_MeanDev.h>
#include <netbuild/NBDistribution.h>
#include "NIVissimSingleTypeParser_Zeitenverteilungsdefinition.h"

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
            SUMOReal mean, deviation;
            from >> mean;
            from >> tag;
            from >> deviation;
            return NBDistribution::dictionary("times", id,
                new Distribution_MeanDev(id, mean, deviation));
        }
        if(tag!="DATAEND") {
            SUMOReal p1 = TplConvert<char>::_2SUMOReal(tag.c_str());
            from >> tag;
            SUMOReal p2 = TplConvert<char>::_2SUMOReal(tag.c_str());
            points.push_back(Position2D(p1, p2));
        }
    } while(tag!="DATAEND");
    return NBDistribution::dictionary("times",
        id, new Distribution_Points(id, points));
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
