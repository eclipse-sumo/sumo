//---------------------------------------------------------------------------//
//                        GUIJunctionWrapper.h -
//  Holds geometrical values for a junction
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 1 Jul 2003
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
// Revision 1.4  2003/08/14 13:47:44  dkrajzew
// false usage of function-pointers patched; false inclusion of .moc-files removed
//
// Revision 1.3  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.2  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.1  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream> // !!!
#include <utility>
#include <utils/qutils/NewQMutex.h>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSNet.h>
#include <gui/GUISUMOAbstractView.h>
#include "GUIJunctionWrapper.h"
#include <gui/popup/QGLObjectPopupMenu.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
/*const char * const
GUIJunctionWrapper::myTableItems[] =
{
    "type", 0
};

const TableType
GUIJunctionWrapper::myTableItemTypes[] =
{
    TT_DOUBLE
};
*/



/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIJunctionWrapper::GUIJunctionWrapper( GUIGlObjectStorage &idStorage,
        MSJunction &junction,
        const Position2DVector &shape)
    : GUIGlObject(idStorage, string("junction:")+junction.id()),
    myJunction(junction), myShape(shape)
{
}


GUIJunctionWrapper::~GUIJunctionWrapper()
{
}


QGLObjectPopupMenu *
GUIJunctionWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                 GUISUMOAbstractView &parent)
{
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id;
    // insert name
    id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add view options
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->setItemEnabled(id, TRUE);
    ret->insertSeparator();
    // add views adding options
    ret->insertSeparator();
    id = ret->insertItem("Show Logic");
    ret->setItemEnabled(id, FALSE);
    // add simulation options
    ret->insertSeparator();
    id = ret->insertItem("Close");
    ret->setItemEnabled(id, FALSE);
    return ret;
}


GUIParameterTableWindow *
GUIJunctionWrapper::getParameterWindow(GUIApplicationWindow &app,
                                       GUISUMOAbstractView &parent)
{
    throw 1;
}



GUIGlObjectType
GUIJunctionWrapper::getType() const
{
    return GLO_JUNCTION;
}


std::string
GUIJunctionWrapper::microsimID() const
{
    return myJunction.id();
}



/*

const char * const
GUIJunctionWrapper::getTableItem(size_t pos) const
{
    return myTableItems[pos];
}


TableType
GUIJunctionWrapper::getTableType(size_t pos) const
{
    return myTableItemTypes[pos];
}


double
GUIJunctionWrapper::getTableParameter(size_t pos) const
{
    throw 1;
}
*/
/*

void
GUIJunctionWrapper::fillTableParameter(double *parameter) const
{
}

*/

Boundery
GUIJunctionWrapper::getBoundery() const
{
    Boundery boundery;
    size_t shapeLength = myShape.size();
    for(size_t i=0; i<shapeLength; i++) {
        const Position2D &pos = myShape.at(i);
        boundery.add(pos.x(), pos.y());
    }
    return boundery;
}


const Position2DVector &
GUIJunctionWrapper::getShape() const
{
    return myShape;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIJunctionWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


