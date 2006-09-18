/***************************************************************************
                          PCPolyContainer.cpp
    A storage for loaded polygons
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
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
// Revision 1.2  2006/09/18 10:14:35  dkrajzew
// changed the way geocoordinates are processed
//
// Revision 1.1  2006/08/01 07:52:46  dkrajzew
// polyconvert added
//
// Revision 1.1  2006/03/27 07:22:27  dksumo
// initial checkin
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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
PCPolyContainer::PCPolyContainer()
{
}


PCPolyContainer::~PCPolyContainer()
{
    clear();
}


bool
PCPolyContainer::insert(std::string key, Polygon2D *poly, int layer)
{
    PolyCont::iterator i=myCont.find(key);
    if(i!=myCont.end()) {
        return false;
    }
    myCont[key] = poly;
    myLayerMap[poly] = layer;
    return true;
}


bool
PCPolyContainer::contains(const std::string &key)
{
	return myCont.find(key)!=myCont.end();
}


size_t
PCPolyContainer::getNo()
{
    return myCont.size();
}


void
PCPolyContainer::clear()
{
    for(PolyCont::iterator i=myCont.begin(); i!=myCont.end(); i++) {
        delete((*i).second);
    }
    myCont.clear();
}


void
PCPolyContainer::report()
{
    WRITE_MESSAGE("   " + toString<int>(getNo()) + " polygons loaded.");
}


void
PCPolyContainer::save(const std::string &file, int layer)
{
	ofstream out(file.c_str());
    if(!out.good()) {
        MsgHandler::getErrorInstance()->inform("Output file '" + file + "' could not be build.");
        throw ProcessError();
    }
	out << "<polygons>" << endl;
    for(PolyCont::iterator i=myCont.begin(); i!=myCont.end(); i++) {

        if((*i).second->getPosition2DVector().size()<3) {
            int bla = 0;
        }

        out << setprecision(2);
        out << "   <poly id=\"" << (*i).second->getName() << "\" type=\""
			<< (*i).second->getType() << "\" color=\""
            << (*i).second->getColor() << "\" fill=\""
            << (*i).second->fill() << "\"";
        out << " layer=\"" << myLayerMap[(*i).second] << "\"";
        out << setprecision(10);
        out << ">" << (*i).second->getPosition2DVector() << "</poly>" << endl;
    }
	out << "</polygons>" << endl;
}


int
PCPolyContainer::getEnumIDFor(const std::string &key)
{
	if(myIDEnums.find(key)==myIDEnums.end()) {
		myIDEnums[key] = 0;
		return 0;
	} else {
		myIDEnums[key] = myIDEnums[key] + 1;
		return myIDEnums[key];
	}
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
