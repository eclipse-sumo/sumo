#ifndef NIVissimDisturbance_h
#define NIVissimDisturbance_h
//---------------------------------------------------------------------------//
//                        NIVissimDisturbance.h -  ccc
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
// Revision 1.8  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2003/06/05 11:46:56  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


#include <map>
#include <string>
#include <utils/common/IntVector.h>
#include <utils/geom/AbstractPoly.h>
#include <netbuild/NBConnection.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimBoundedClusterObject.h"
#include "NIVissimNodeParticipatingEdgeVector.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;
class NBEdge;
class NBDistrictCont;

class NIVissimDisturbance
        : public NIVissimBoundedClusterObject {
public:
    NIVissimDisturbance(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &edge,
        const NIVissimExtendedEdgePoint &by,
        SUMOReal timegap, SUMOReal waygap, SUMOReal vmax);
    virtual ~NIVissimDisturbance();
    void computeBounding();
    bool addToNode(NBNode *node, NBDistrictCont &dc,
        NBNodeCont &nc, NBEdgeCont &ec);
    int getEdgeID() const { return myEdge.getEdgeID(); }
    int getDisturbanceID() const { return myDisturbance.getEdgeID(); }
    NBConnection getConnection(NBNode *node, int aedgeid);

public:
    static bool dictionary(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &edge,
        const NIVissimExtendedEdgePoint &by,
        SUMOReal timegap, SUMOReal waygap, SUMOReal vmax);
    static bool dictionary(int id, NIVissimDisturbance *o);
    static NIVissimDisturbance *dictionary(int id);
    static IntVector getWithin(const AbstractPoly &poly);
    static void clearDict();
    static void dict_SetDisturbances();
    static void reportRefused();

private:
    int myID;
    int myNode;
    std::string myName;
    NIVissimExtendedEdgePoint myEdge;
    NIVissimExtendedEdgePoint myDisturbance;
    SUMOReal myTimeGap, myWayGap, myVMax;

    typedef std::map<int, NIVissimDisturbance*> DictType;
    static DictType myDict;
    static int myRunningID;
    static int refusedProhibits;
};




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

