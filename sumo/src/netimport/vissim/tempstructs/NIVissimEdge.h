#ifndef NIVissimEdge_h
#define NIVissimEdge_h
/***************************************************************************
                          NIVissimEdge.h
			  An edge imported from Vissim together for a container for
              these instances
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : End of 2002
    copyright            : (C) 2002 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2003/03/31 06:15:49  dkrajzew
// further work on vissim-import
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIVissimConnectionCluster.h"
#include <vector>
#include <string>
#include <map>
#include <utils/geom/Position2DVector.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimClosedLanesVector.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;

/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIVissimEdge
        : public NIVissimAbstractEdge {
public:
    /// Constructor
    NIVissimEdge(int id, const std::string &name,
        const std::string &type, int noLanes, double zuschlag1,
        double zuschlag2, double length,
        const Position2DVector &geom,
        const NIVissimClosedLanesVector &clv);

    /// Destructor
    ~NIVissimEdge();

    void setNodeCluster(int nodeid);
    void buildGeom();

    /// Adds a connection where this edge is the destination
    void addIncomingConnection(int id);

    /// Adds a connection where this edge is the source
    void addOutgoingConnection(int id);

    /** @brief Returns the node at the given position
        As this may be ambigous, a second node not to return may be supplied */
    NBNode *getNodeAt(const Position2D &p, NBNode *other=0);

    /** Returns the begin position of the edge */
    Position2D getBegin2D() const;

    /// Returns the end position of the edge
    Position2D getEnd2D() const;

    /// Returns the length of the node
    double getLength() const;


    void mergedInto(NIVissimConnectionCluster *old,
        NIVissimConnectionCluster *act);

    void removeFromConnectionCluster(NIVissimConnectionCluster *c);
    void addToConnectionCluster(NIVissimConnectionCluster *c);

    friend class NIVissimNodeDef_Edges;

public:
    /** @brief Adds the described item to the dictionary
        Builds the edge first */
    static bool dictionary(int id, const std::string &name,
        const std::string &type, int noLanes, double zuschlag1,
        double zuschlag2, double length,
        const Position2DVector &geom,
        const NIVissimClosedLanesVector &clv);

    /// Adds the edge to the dictionary
    static bool dictionary(int id, NIVissimEdge *o);

    /// Returns the named edge from the dictionary
    static NIVissimEdge *dictionary(int id);

    static void buildConnectionClusters();

    /// Builds NBEdges from the VissimEdges within the dictionary
    static void dict_buildNBEdges();


private:
    /// Builds the NBEdge from this VissimEdge
    void buildNBEdge();

    /// Returns the origin node
    NBNode *getFromNode();

    /// Returns the destination node
    NBNode *getToNode();

    /// Tries to resolve the problem that the same node has been returned as origin and destination node
    std::pair<NBNode*, NBNode*> resolveSameNode();

private:
    static NBNode *getNodeSecure(int nodeid, const Position2D &pos,
        const std::string &possibleName);

private:
    /**
     * Sorts connections the edge participates in by their position along
     * the given edge
     */
    class connection_position_sorter {
    public:
        /// constructor
        explicit connection_position_sorter(int edgeid);

        /// comparing operation
        int operator() (int c1id, int c2id) const;

    private:
        /// The id of the edge
        int myEdgeID;

    };


    /**
     * Sorts connection clusters the edge participates in by their
     * position along the given edge
     */
    class connection_cluster_position_sorter {
    public:
        /// constructor
        explicit connection_cluster_position_sorter(int edgeid);

        /// comparing operation
        int operator() (NIVissimConnectionCluster *cc1,
            NIVissimConnectionCluster *cc2) const;

    private:
        /// The id of the edge
        int myEdgeID;

    };

private:
    /// The definition for a container for connection clusters
    typedef std::vector<NIVissimConnectionCluster*> ConnectionClusters;

    /// The name of the edge
    std::string myName;

    /// The tape of the edge
    std::string myType;

    /// The number of lanes the edge has
    int myNoLanes;

    /// Additional load values for this edge
    double myZuschlag1, myZuschlag2;

    /// List of lanes closed on this edge
    NIVissimClosedLanesVector myClosedLanes;

    /// List of connection clusters along this edge
    ConnectionClusters myConnectionClusters;

    /// List of connections incoming to this edge
    IntVector myIncomingConnections;

    /// List of connections outgoing from this edge
    IntVector myOutgoingConnections;

    /// List of districts referencing this edge
    IntVector myDistricts;

private:
    /// Definition of the dictionary type
    typedef std::map<int, NIVissimEdge*> DictType;

    /// The dictionary
    static DictType myDict;

    /** @brief The current maximum id;
        needed for further id assignment */
    static int myMaxID;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
