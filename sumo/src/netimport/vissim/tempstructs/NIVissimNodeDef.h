#ifndef NIVissimNodeDef_h
#define NIVissimNodeDef_h
//---------------------------------------------------------------------------//
//                        NIVissimNodeDef.h -  ccc
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
// Revision 1.7  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.6  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.5  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Boundary.h>
//#include "NIVissimBoundedClusterObject.h"
#include "NIVissimExtendedEdgePointVector.h"
#include "NIVissimNodeCluster.h"

class NIVissimNodeDef
/*        : public NIVissimBoundedClusterObject*/ {
public:
    NIVissimNodeDef(int id, const std::string &name);
    virtual ~NIVissimNodeDef();
    int buildNodeCluster();
//    virtual void computeBounding() = 0;
//    bool partialWithin(const AbstractPoly &p, double off=0.0) const;
    virtual void searchAndSetConnections() = 0;
    virtual double getEdgePosition(int edgeid) const = 0;

public:
    static bool dictionary(int id, NIVissimNodeDef *o);
    static NIVissimNodeDef *dictionary(int id);
//    static IntVector getWithin(const AbstractPoly &p, double off=0.0);
//    static void buildNodeClusters();
    static void dict_assignConnectionsToNodes();
    static size_t dictSize();
    static void clearDict();
    static int getMaxID();
protected:
    int myID;
    std::string myName;

private:
    typedef std::map<int, NIVissimNodeDef*> DictType;
    static DictType myDict;
    static int myMaxID;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

