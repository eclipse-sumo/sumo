#ifndef GUIJunctionWrapper_h
#define GUIJunctionWrapper_h
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
// $Log$
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
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/HaveBoundery.h>
#include <utils/qutils/NewQMutex.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIGlObject.h>
#include <gui/TableTypes.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSJunction;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class GUIJunctionWrapper :
            public GUIGlObject,
            public HaveBoundery {
public:
    /// constructor
    GUIJunctionWrapper( GUIGlObjectStorage &idStorage,
        MSJunction &junction,
        const Position2DVector &shape);

    /// destructor
    virtual ~GUIJunctionWrapper();

    /// Returns a popup-menu fpr lanes
    QGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow *app,
        GUISUMOAbstractView *parent);

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the shape of the junction
    const Position2DVector &getShape() const;

    double getTableParameter(size_t pos) const;

    void fillTableParameter(double *parameter) const;

    const char * const getTableItem(size_t pos) const;

    Boundery getBoundery() const;

    static void fill(std::vector<GUIJunctionWrapper*> &list,
        GUIGlObjectStorage &idStorage);

protected:

    TableType getTableType(size_t pos) const;

    const char *getTableBeginValue(size_t pos) const { throw 1; }

	bool active() const { return true; }

protected:

    Position2DVector myShape;

    MSJunction &myJunction;

    static const char * const myTableItems[];

    static const TableType myTableItemTypes[];

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIJunctionWrapper.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

