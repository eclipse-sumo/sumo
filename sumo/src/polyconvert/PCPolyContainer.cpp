/****************************************************************************/
/// @file    PCPolyContainer.cpp
/// @author  unknown_author
/// @date    Mon, 05 Dec 2005
/// @version $Id: $
///
// A storage for loaded polygons and pois
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

#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/shapes/Polygon2D.h>
#include "PCPolyContainer.h"
#include <fstream>
#include <utils/common/UtilExceptions.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
PCPolyContainer::PCPolyContainer(bool prune, const Boundary &prunningBoundary)
        : myPrunningBoundary(prunningBoundary), myDoPrunne(prune)
{}


PCPolyContainer::~PCPolyContainer()
{
    clear();
}


bool
PCPolyContainer::insert(std::string key, Polygon2D *poly, int layer)
{
    // check whether the polygon lies within the wished area
    //  - if such an area was given
    if (myDoPrunne) {
        Boundary b = poly->getPosition2DVector().getBoxBoundary();
        if (!b.partialWithin(myPrunningBoundary)) {
            return true;
        }
    }
    //
    PolyCont::iterator i=myPolyCont.find(key);
    if (i!=myPolyCont.end()) {
        return false;
    }
    myPolyCont[key] = poly;
    myPolyLayerMap[poly] = layer;
    return true;
}


bool
PCPolyContainer::insert(std::string key, PointOfInterest *poi, int layer)
{
    // check whether the poi lies within the wished area
    //  - if such an area was given
    if (myDoPrunne) {
        if (!myPrunningBoundary.around(*poi)) {
            return true;
        }
    }
    //
    POICont::iterator i=myPOICont.find(key);
    if (i!=myPOICont.end()) {
        return false;
    }
    myPOICont[key] = poi;
    myPOILayerMap[poi] = layer;
    return true;
}


bool
PCPolyContainer::contains(const std::string &key)
{
    return myPolyCont.find(key)!=myPolyCont.end();
}


size_t
PCPolyContainer::getNoPolygons()
{
    return myPolyCont.size();
}


size_t
PCPolyContainer::getNoPOIs()
{
    return myPOICont.size();
}


void
PCPolyContainer::clear()
{
    {
        for (PolyCont::iterator i=myPolyCont.begin(); i!=myPolyCont.end(); i++) {
            delete((*i).second);
        }
        myPolyCont.clear();
    }
    {
        for (POICont::iterator i=myPOICont.begin(); i!=myPOICont.end(); i++) {
            delete((*i).second);
        }
        myPOICont.clear();
    }
}


void
PCPolyContainer::report()
{
    WRITE_MESSAGE("   " + toString<int>(getNoPolygons()) + " polygons loaded.");
    WRITE_MESSAGE("   " + toString<int>(getNoPOIs()) + " pois loaded.");
}


void
PCPolyContainer::save(const std::string &file, int /*layer*/)
{
    ofstream out(file.c_str());
    if (!out.good()) {
        MsgHandler::getErrorInstance()->inform("Output file '" + file + "' could not be build.");
        throw ProcessError();
    }
    out << "<shapes>" << endl;
    // write polygons
    {
        for (PolyCont::iterator i=myPolyCont.begin(); i!=myPolyCont.end(); ++i) {
            out << setprecision(2);
            out << "   <poly id=\"" << (*i).second->getName() << "\" type=\""
            << (*i).second->getType() << "\" color=\""
            << (*i).second->getColor() << "\" fill=\""
            << (*i).second->fill() << "\"";
            out << " layer=\"" << myPolyLayerMap[(*i).second] << "\"";
            out << setprecision(10);
            out << ">" << (*i).second->getPosition2DVector() << "</poly>" << endl;
        }
    }
    // write pois
    {
        for (POICont::iterator i=myPOICont.begin(); i!=myPOICont.end(); ++i) {
            out << setprecision(2);
            out << "   <poi id=\"" << (*i).second->getID() << "\" type=\""
            << (*i).second->getType() << "\" color=\""
            << *static_cast<RGBColor*>((*i).second) << '"';
            out << " layer=\"" << myPOILayerMap[(*i).second] << "\"";
            out << setprecision(10);
            out << " x=\"" << (*i).second->x() << "\""
            << " y=\"" << (*i).second->y() << "\""
            << "/>" << endl;
        }
    }
    //
    out << "</shapes>" << endl;
}


int
PCPolyContainer::getEnumIDFor(const std::string &key)
{
    if (myIDEnums.find(key)==myIDEnums.end()) {
        myIDEnums[key] = 0;
        return 0;
    } else {
        myIDEnums[key] = myIDEnums[key] + 1;
        return myIDEnums[key];
    }
}



/****************************************************************************/

