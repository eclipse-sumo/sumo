/****************************************************************************/
/// @file    NIVissimNodeCluster.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
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
#ifndef NIVissimNodeCluster_h
#define NIVissimNodeCluster_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Position2D.h>

// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class NBNodeCont;
class NBEdgeCont;
class NBDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimNodeCluster
{
public:
    NIVissimNodeCluster(int id, int nodeid, int tlid,
                        const IntVector &connectors,
                        const IntVector &disturbances,
                        bool amEdgeSplitOnly);
    ~NIVissimNodeCluster();
    int getID() const
    {
        return myID;
    }
    void buildNBNode(NBNodeCont &nc);
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
    static void buildNBNodes(NBNodeCont &nc);
    static void dict_recheckEdgeChanges();
    static int getFromNode(int edgeid);
    static int getToNode(int edgeid);
    static void _debugOut(std::ostream &into);
    static void dict_addDisturbances(NBDistrictCont &dc,
                                     NBNodeCont &nc, NBEdgeCont &ec);
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


#endif

/****************************************************************************/

