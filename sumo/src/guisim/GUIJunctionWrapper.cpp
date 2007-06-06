/****************************************************************************/
/// @file    GUIJunctionWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// }
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

#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSNet.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUIJunctionWrapper.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

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
GUIJunctionWrapper::GUIJunctionWrapper(GUIGlObjectStorage &idStorage,
                                       MSJunction &junction,
                                       const Position2DVector &shape)
        : GUIGlObject(idStorage, "junction:"+junction.getID()),
        myJunction(junction), myShape(shape)
{}


GUIJunctionWrapper::~GUIJunctionWrapper()
{}


GUIGLObjectPopupMenu *
GUIJunctionWrapper::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    return ret;
}


GUIParameterTableWindow *
GUIJunctionWrapper::getParameterWindow(GUIMainWindow &app,
                                       GUISUMOAbstractView &)
{
    std::vector<std::string> s=myJunction.getNames();
    int NumberOfRows =s.size();
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 2+NumberOfRows);
    // add items
    //ret->mkItem("length [m]", false, myJunction.length());
    ret->mkItem("x-Position", false, myJunction.getPosition().x());
    ret->mkItem("y-Position", false, myJunction.getPosition().y());

    //std::vector<string>::iterator p=s.begin();
    /*
    while(p !=s.end())
    {
    	ret->mkItem("name", false, s[]);
    	p++;
    }
    */
    for (unsigned int i=0 ; i<s.size(); i++) {
        ret->mkItem("name", false, s[i]);
    }

    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUIJunctionWrapper::getType() const
{
    return GLO_JUNCTION;
}


const std::string &
GUIJunctionWrapper::microsimID() const
{
    return myJunction.getID();
}


bool
GUIJunctionWrapper::active() const
{
    return true;
}


Boundary
GUIJunctionWrapper::getBoundary() const
{
    Boundary boundary;
    size_t shapeLength = myShape.size();
    for (size_t i=0; i<shapeLength; i++) {
        const Position2D &pos = myShape[i];
        boundary.add(pos.x(), pos.y());
    }
    return boundary;
}


const Position2DVector &
GUIJunctionWrapper::getShape() const
{
    return myShape;
}


Boundary
GUIJunctionWrapper::getCenteringBoundary() const
{
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}

MSJunction &
GUIJunctionWrapper::getJunction() const
{
    return myJunction;
}



/****************************************************************************/

