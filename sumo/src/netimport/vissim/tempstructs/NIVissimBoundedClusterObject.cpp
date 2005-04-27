//---------------------------------------------------------------------------//
//                        NIVissimBoundedClusterObject.cpp -  ccc
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
// Revision 1.8  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.7  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.6  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.5  2003/06/05 11:46:55  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <cassert>
#include <utils/geom/Boundary.h>
#include "NIVissimBoundedClusterObject.h"

NIVissimBoundedClusterObject::ContType NIVissimBoundedClusterObject::myDict;

NIVissimBoundedClusterObject::NIVissimBoundedClusterObject()
    : myBoundary(0), myClusterID(-1)
{
    myDict.insert(this);
}


NIVissimBoundedClusterObject::~NIVissimBoundedClusterObject()
{
    delete myBoundary;
}


bool
NIVissimBoundedClusterObject::crosses(const AbstractPoly &poly,
                                      double offset) const
{
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
    return myBoundary->overlapsWith(poly, offset);
}


void
NIVissimBoundedClusterObject::inCluster(int id)
{
    myClusterID = id;
}


bool
NIVissimBoundedClusterObject::clustered() const
{
    return myClusterID>0;
}


void
NIVissimBoundedClusterObject::closeLoading()
{
    for(ContType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i)->computeBounding();
    }
}


const Boundary &
NIVissimBoundedClusterObject::getBoundary() const
{
    return *myBoundary;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


