#ifndef NIVissimNodeCluster_h
#define NIVissimNodeCluster_h
//---------------------------------------------------------------------------//
//                        NIVissimNodeCluster.h -  ccc
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
// Revision 1.6  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Position2D.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimNodeCluster {
public:
    NIVissimNodeCluster(int id, int nodeid, int tlid,
        const IntVector &connectors,
        const IntVector &disturbances,
		bool amEdgeSplitOnly);
    ~NIVissimNodeCluster();
    int getID() const { return myID; }
    void buildNBNode();
    bool recheckEdgeChanges();
    NBNode *getNBNode() const;
    Position2D getPos() const;
    std::string getNodeName() const;


public:
    static bool dictionary(int id, NIVissimNodeCluster *o);
    static int dictionary(int nodeid, int tlid, const IntVector &connectors,
		const IntVector &disturbances, bool amEdgeSplitOnly);
    static NIVissimNodeCluster *dictionary(int id);
    static size_t contSize();
    static void assignToEdges();
    static void buildNBNodes();
    static void dict_recheckEdgeChanges();
    static int getFromNode(int edgeid);
    static int getToNode(int edgeid);
    static void _debugOut(std::ostream &into);
    static void dict_addDisturbances();
    static void clearDict();
    static void setCurrentVirtID(int id);


private:

    int myID;
    int myNodeID;
    int myTLID;
    IntVector myConnectors;
    IntVector myDisturbances;
    Position2D myPosition;
    typedef std::map<int, NIVissimNodeCluster*> DictType;
    static DictType myDict;
    static int myCurrentID;
    NBNode *myNBNode;
	bool myAmEdgeSplit;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimNodeCluster.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

