/****************************************************************************/
/// @file    NIArtemisTempSegments.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2DVector.h>
#include "NIArtemisTempSegments.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

NIArtemisTempSegments::Link2PosInfos
NIArtemisTempSegments::myLinkPosInfos;

NIArtemisTempSegments::SingleGeomPoint::SingleGeomPoint(int segment,
        SUMOReal x,
        SUMOReal y)
        : mySegment(segment), myX(x), myY(y)
{}


NIArtemisTempSegments::SingleGeomPoint::~SingleGeomPoint()
{}


Position2D
NIArtemisTempSegments::SingleGeomPoint::getPosition() const
{
    return Position2D(myX, myY);
}


int
NIArtemisTempSegments::SingleGeomPoint::getSegmentNo() const
{
    return mySegment;
}





void
NIArtemisTempSegments::add(const std::string &linkname, int segment,
                               SUMOReal x, SUMOReal y)
{
    myLinkPosInfos[linkname].push_back(
        new SingleGeomPoint(segment, x, y));
}


void
NIArtemisTempSegments::close(NBEdgeCont &ec)
{
    // go through the read values
    for (Link2PosInfos::iterator i=myLinkPosInfos.begin(); i!=myLinkPosInfos.end(); i++) {
        // get the current list of segment definitions
        PosInfos infos = (*i).second;
        // sort them
        sort(infos.begin(), infos.end(), info_sorter());
        // build the geomentry list
        Position2DVector geom;
        for (PosInfos::iterator j=infos.begin(); j!=infos.end(); j++) {
            geom.push_back((*j)->getPosition());
        }
        // add to the edge
        string name = (*i).first;
        NBEdge *edge = ec.retrieve(name);
        if (edge==0) {
            MsgHandler::getErrorInstance()->inform("Trying to parse the geometry for the unknown edge '" + name + "'.");
        } else {
            edge->setGeometry(geom);
        }
    }
}



/****************************************************************************/

