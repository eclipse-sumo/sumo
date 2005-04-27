#ifndef NIVissimBoundedClusterObject_h
#define NIVissimBoundedClusterObject_h
//---------------------------------------------------------------------------//
//                        NIVissimBoundedClusterObject.h -  ccc
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
// $Log$
// Revision 1.6  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.4  2003/06/05 11:46:55  dkrajzew
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


#include <set>
#include <string>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class Boundary;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimBoundedClusterObject {
public:
    NIVissimBoundedClusterObject();
    virtual ~NIVissimBoundedClusterObject();
    virtual void computeBounding() = 0;
    bool crosses(const AbstractPoly &poly, double offset=0) const;
    void inCluster(int id);
    bool clustered() const;
    const Boundary &getBoundary() const;
public:
    static void closeLoading();
protected:
    typedef std::set<NIVissimBoundedClusterObject*> ContType;
    static ContType myDict;
    Boundary *myBoundary;
    int myClusterID;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

