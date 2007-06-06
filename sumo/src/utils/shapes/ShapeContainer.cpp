/****************************************************************************/
/// @file    ShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A view on the simulation; this view is a microscopic one
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <string>
#include <cmath>
#include <utils/helpers/NamedObjectCont.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StdDefs.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ShapeContainer::ShapeContainer()
        : myCurrentLayer(-10), myMinLayer(100), myMaxLayer(-100)
{}


ShapeContainer::~ShapeContainer()
{}


bool
ShapeContainer::add(int layer, Polygon2D *p)
{
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon2D*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer].add(p->getName(), p);
}


bool
ShapeContainer::add(int layer, PointOfInterest *p)
{
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer].add(p->getID(), p);
}

/*
bool
ShapeContainer::add(Polygon2D *p)
{
    return add(myCurrentLayer, p);
}


bool
ShapeContainer::add(PointOfInterest *p)
{
    return add(myCurrentLayer, p);
}
*/


bool
ShapeContainer::save(const std::string &/*file*/)
{

    /*
    ofstream out(file.c_str());

    if (!out) {
        MsgHandler::getErrorInstance()->inform(
          "Cannot open file "+file+"." );
        throw ProcessError();
    return 0;
    }

    NamedObjectCont<PointOfInterest*> it= getPOICont();
    std::map< std::string, PointOfInterest*> t = it.getMyMap();
    map< std::string, PointOfInterest*>::iterator i;

    out<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>"<<endl;
    out<<"<PointOfInterests>"<<endl;
    for(i=t.begin(); i!= t.end(); i++){

    PointOfInterest *p =(*i).second;

    out<<"<PointOfInterest id=\"";
        out<<p->getID().c_str();

    out<<"\"	type=\"";
    out<<p->getType().c_str();

    out<<"\"	xpos=\"";
    out<<toString<float>(p->x()).c_str();

    out<<"\"	ypos=\"";
    out<<toString<float>(p->y()).c_str();

    out<<"\"	color=\"";
    out<<toString<int>(p->red()).c_str();
    out<<toString<int>(p->green()).c_str();
    out<<toString<int>(p->blue()).c_str();

    out<<"\" />"<<endl;
    }

    out<<"</PointOfInterests>";
    out.flush();
    out.close();
    */
    return 1;

}


NamedObjectCont<Polygon2D*> &
ShapeContainer::getPolygonCont(int layer) const
{
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon2D*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer];
}


NamedObjectCont<PointOfInterest*> &
ShapeContainer::getPOICont(int layer) const
{
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer];
}


int
ShapeContainer::getMinLayer() const
{
    return myMinLayer;
}


int
ShapeContainer::getMaxLayer() const
{
    return myMaxLayer;
}



/****************************************************************************/

