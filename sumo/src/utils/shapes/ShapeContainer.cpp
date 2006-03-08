
//---------------------------------------------------------------------------//
//                        ShapeContainer.cpp -
//  A view on the simulation; this view is a microscopic one
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2006/03/08 13:09:12  dkrajzew
// possibility to insert pois on the gui added (danilo tet-boyom)
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

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

ShapeContainer::ShapeContainer()
{
}

ShapeContainer::~ShapeContainer()
{
}

bool
ShapeContainer::add(Polygon2D *p) {
    return myPolygons.add(p->getName(), p);
}

bool
ShapeContainer::add(PointOfInterest *p) {
    return myPOIs.add(p->getID(), p);
}


bool
ShapeContainer::save(const std::string &file) {

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
	return 1;

}


NamedObjectCont<Polygon2D*> &
ShapeContainer::getPolygonCont() const
{
    return myPolygons;
}

NamedObjectCont<PointOfInterest*> &
ShapeContainer::getPOICont() const
{
    return myPOIs;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End: