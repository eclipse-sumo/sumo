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
// Revision 1.5  2003/06/05 11:46:55  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <cassert>
#include <utils/geom/Boundery.h>
#include "NIVissimBoundedClusterObject.h"

NIVissimBoundedClusterObject::ContType NIVissimBoundedClusterObject::myDict;

NIVissimBoundedClusterObject::NIVissimBoundedClusterObject()
    : myClusterID(-1),
    myBoundery(0)
{
    myDict.insert(this);
}


NIVissimBoundedClusterObject::~NIVissimBoundedClusterObject()
{
    delete myBoundery;
}


bool
NIVissimBoundedClusterObject::crosses(const AbstractPoly &poly,
                                      double offset) const
{
    assert(myBoundery!=0&&myBoundery->xmax()>=myBoundery->xmin());
    return myBoundery->overlapsWith(poly, offset);
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


const Boundery &
NIVissimBoundedClusterObject::getBoundery() const
{
    return *myBoundery;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimBoundedClusterObject.icc"
//#endif

// Local Variables:
// mode:C++
// End:


