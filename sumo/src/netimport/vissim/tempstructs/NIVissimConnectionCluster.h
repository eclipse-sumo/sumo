#ifndef NIVissimConnectionCluster_h
#define NIVissimConnectionCluster_h
//---------------------------------------------------------------------------//
//                        NIVissimConnectionCluster.h -  ccc
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
// Revision 1.12  2003/10/30 13:44:10  dkrajzew
// vissim-import seems to work
//
// Revision 1.11  2003/10/30 09:12:59  dkrajzew
// further work on vissim-import
//
// Revision 1.10  2003/10/15 11:51:28  dkrajzew
// further work on vissim-import
//
// Revision 1.9  2003/09/22 12:42:17  dkrajzew
// further work on vissim-import
//
// Revision 1.8  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.7  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <iostream>
#include <vector>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include <utils/common/IntVector.h>
#include "NIVissimConnection.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVissimConnectionCluster
 * This class holds a list of connections either all outgoing or all
 * incoming within an edge, which do lies close together.
 * This shall be the connections which belong to a single node.
 * It still are not all of the node's connections, as other edges
 * may participate to this node, too.
 */
class NIVissimConnectionCluster {
public:
    /** @brief Constructor
        Build the boundery; The boundery includes both incoming and outgoing nodes */
    NIVissimConnectionCluster(const IntVector &connections, int nodeCluster,
        int edgeid);

    NIVissimConnectionCluster(const IntVector &connections,
        const Boundery &boundery, int nodeCluster, const IntVector &edges);

    /// Destructor
    ~NIVissimConnectionCluster();

    /// Returns the information whether the given cluster overlaps the current
    bool overlapsWith(NIVissimConnectionCluster *c, double offset=0) const;

    bool hasNodeCluster() const;

    NBNode *getNBNode() const;

    bool around(const Position2D &p, double offset=0) const;

    double getPositionForEdge(int edgeid) const;

    friend class NIVissimEdge; // !!! debug

	const IntVector &getConnections() const {
		return myConnections;
	}

    Position2DVector getIncomingContinuationGeometry(NIVissimEdge *e) const;
    Position2DVector getOutgoingContinuationGeometry(NIVissimEdge *e) const;
    NIVissimConnection *getIncomingContinuation(NIVissimEdge *e) const;
    NIVissimConnection *getOutgoingContinuation(NIVissimEdge *e) const;


public:
    /** @brief Tries to joind clusters participating within a node
        This is done by joining clusters which overlap */
    static void joinBySameEdges(double offset);

    static void joinByDisturbances(double offset);

    static void buildNodeClusters();

    static void searchForConnection(int id);

    static void _debugOut(std::ostream &into);

    static size_t dictSize();

    static void dict_recheckNodes(double offset);

    static int getNextFreeNodeID();

    static void clearDict();
/*
    static void addNodes(double offset);

    //static void dict_checkDoubleNodes();

    static void addTLs(double offset);
*/
private:
    class NodeSubCluster {
    public:
        NodeSubCluster(NIVissimConnection *c);
        ~NodeSubCluster();
        void add(NIVissimConnection *c);
        void add(const NodeSubCluster &c);
        size_t size() const;
        void setConnectionsFree();
        bool overlapsWith(const NodeSubCluster &c, double offset=0);
        IntVector getConnectionIDs() const;
        friend class NIVissimConnectionCluster;
    public:
        Boundery myBoundery;
        typedef std::vector<NIVissimConnection*> ConnectionCont;
        ConnectionCont myConnections;
    };

    class same_direction_sorter {
    private:
        double _angle;

    public:
        /// constructor
        explicit same_direction_sorter(double angle)
            : _angle(angle) { }

    public:
        /// comparing operation
        int operator() (NIVissimConnection *c1, NIVissimConnection *c2) const {
            return
                abs(c1->getGeometry().beginEndAngle()-_angle)
                <
                abs(c2->getGeometry().beginEndAngle()-_angle);
        }
    };



private:
    /// Adds the second cluster
    void add(NIVissimConnectionCluster *c);

    void removeConnections(const NodeSubCluster &c);

    void recomputeBoundery();

    void recheckEdges();

    bool joinable(NIVissimConnectionCluster *c2, double offset);


    IntVector getDisturbanceParticipators();

    IntVector extendByToTreatAsSame(const IntVector &iv1,
        const IntVector &iv2) const;

    bool isWeakDistrictConnRealisation(NIVissimConnectionCluster *c2);


private:
    /// List of connection-ids which participate within this cluster
    IntVector myConnections;

    /// The boundery of the cluster
    Boundery myBoundery;

    /// The node the cluster is assigned to
    int myNodeCluster;

    // The edge which holds the cluster
    IntVector myEdges;

    IntVector myNodes;

    IntVector myTLs;

	IntVector myOutgoingEdges, myIncomingEdges;

private:
    typedef std::vector<NIVissimConnectionCluster*> ContType;
    static ContType myClusters;
    static int myFirstFreeID;
    static int myStaticBlaID;
    int myBlaID;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimConnectionCluster.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

