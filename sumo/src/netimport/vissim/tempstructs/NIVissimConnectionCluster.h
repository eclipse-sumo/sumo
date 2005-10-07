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
// Revision 1.18  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.17  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.16  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.15  2005/02/17 10:33:39  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.14  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.13  2003/11/11 08:24:52  dkrajzew
// debug values removed
//
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


#include <iostream>
#include <vector>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
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
        Build the boundary; The boundary includes both incoming and outgoing nodes */
    NIVissimConnectionCluster(const IntVector &connections, int nodeCluster,
        int edgeid);

    NIVissimConnectionCluster(const IntVector &connections,
        const Boundary &boundary, int nodeCluster, const IntVector &edges);

    /// Destructor
    ~NIVissimConnectionCluster();

    /// Returns the information whether the given cluster overlaps the current
    bool overlapsWith(NIVissimConnectionCluster *c, SUMOReal offset=0) const;

    bool hasNodeCluster() const;

    NBNode *getNBNode() const;

    bool around(const Position2D &p, SUMOReal offset=0) const;

    SUMOReal getPositionForEdge(int edgeid) const;

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
    static void joinBySameEdges(SUMOReal offset);

    static void joinByDisturbances(SUMOReal offset);

    static void buildNodeClusters();

    static void searchForConnection(int id);

    static void _debugOut(std::ostream &into);

    static size_t dictSize();

    static void dict_recheckNodes(SUMOReal offset);

    static int getNextFreeNodeID();

    static void clearDict();
/*
    static void addNodes(SUMOReal offset);

    //static void dict_checkDoubleNodes();

    static void addTLs(SUMOReal offset);
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
        bool overlapsWith(const NodeSubCluster &c, SUMOReal offset=0);
        IntVector getConnectionIDs() const;
        friend class NIVissimConnectionCluster;
    public:
        Boundary myBoundary;
        typedef std::vector<NIVissimConnection*> ConnectionCont;
        ConnectionCont myConnections;
    };

    class same_direction_sorter {
    private:
        SUMOReal _angle;

    public:
        /// constructor
        explicit same_direction_sorter(SUMOReal angle)
            : _angle(angle) { }

    public:
        /// comparing operation
        int operator() (NIVissimConnection *c1, NIVissimConnection *c2) const {
            return
                fabs(c1->getGeometry().beginEndAngle()-_angle)
                <
                fabs(c2->getGeometry().beginEndAngle()-_angle);
        }
    };



private:
    /// Adds the second cluster
    void add(NIVissimConnectionCluster *c);

    void removeConnections(const NodeSubCluster &c);

    void recomputeBoundary();

    void recheckEdges();

    bool joinable(NIVissimConnectionCluster *c2, SUMOReal offset);


    IntVector getDisturbanceParticipators();

    IntVector extendByToTreatAsSame(const IntVector &iv1,
        const IntVector &iv2) const;

    bool isWeakDistrictConnRealisation(NIVissimConnectionCluster *c2);

    bool liesOnSameEdgesEnd(NIVissimConnectionCluster *cc2);



private:
    /// List of connection-ids which participate within this cluster
    IntVector myConnections;

    /// The boundary of the cluster
    Boundary myBoundary;

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

#endif

// Local Variables:
// mode:C++
// End:

