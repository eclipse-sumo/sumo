/****************************************************************************/
/// @file    NBContHelper.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Some methods for traversing lists of edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBContHelper_h
#define NBContHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBEdge.h"
#include "NBNode.h"
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * NBContHelper
 * Some static helper methods that traverse a sorted list of edges in both
 * directions
 */
class NBContHelper {
public:
    /** Moves the given iterator clockwise within the given container
        of edges sorted clockwise */
    static void nextCW(const EdgeVector * edges,
                       EdgeVector::const_iterator &from);

    /** Moves the given iterator counter clockwise within the given container
        of edges sorted clockwise */
    static void nextCCW(const EdgeVector * edges,
                        EdgeVector::const_iterator &from);

    static SUMOReal getMaxSpeed(const EdgeVector &edges);

    static SUMOReal getMinSpeed(const EdgeVector &edges);

    /** writes the vector of bools to the given stream */
    static std::ostream &out(std::ostream &os, const std::vector<bool> &v);

    /**
     * edge_by_angle_sorter
     * Class to sort edges by their angle
     */
    class edge_by_junction_angle_sorter {
    public:
        /// constructor
        explicit edge_by_junction_angle_sorter(NBNode *n) : myNode(n) {}

        /// comparing operation
        int operator()(NBEdge *e1, NBEdge *e2) const;

    private:
        /// Converts the angle of the edge if it is an incoming edge
        SUMOReal getConvAngle(NBEdge *e) const;

    private:
        /// the edge to compute the relative angle of
        NBNode *myNode;

    };

    /**
     * relative_edge_sorter
     * Class to sort edges by their angle in relation to the node the
     * edge using this class is incoming into. This is normally done to
     * sort edges outgoing from the node the using edge is incoming in
     * by their angle in relation to the using edge's angle (this angle
     * is the reference angle).
     */
    class relative_edge_sorter {
    public:
        /// constructor
        explicit relative_edge_sorter(NBEdge *e, NBNode *n)
                : myEdge(e), myNode(n) {}

    public:
        /// comparing operation
        int operator()(NBEdge *e1, NBEdge *e2) const {
            if (e1==0||e2==0) {
                return -1;
            }
            SUMOReal relAngle1 = NBHelpers::normRelAngle(
                                     myEdge->getAngle(), e1->getAngle());
            SUMOReal relAngle2 = NBHelpers::normRelAngle(
                                     myEdge->getAngle(), e2->getAngle());
            return relAngle1 > relAngle2;
        }

    private:
        /// the edge to compute the relative angle of
        NBEdge *myEdge;

        /// the node to use
        NBNode *myNode;

    };


    /**
     * edge_by_priority_sorter
     * Class to sort edges by their priority
     */
    class edge_by_priority_sorter {
    public:
        /// comparing operator
        int operator()(NBEdge *e1, NBEdge *e2) const {
            if (e1->getPriority()!=e2->getPriority()) {
                return e1->getPriority() > e2->getPriority();
            }
            if (e1->getSpeed()!=e2->getSpeed()) {
                return e1->getSpeed() > e2->getSpeed();
            }
            return e1->getNoLanes() > e2->getNoLanes();
        }
    };

    // ---------------------------

    /**
     * @class edge_opposite_direction_sorter
     * @brief Class to sort edges by their angle in relation to the given edge
     *
     * The resulting sorted list has the edge in the most opposite direction
     *  to the given edge as her first entry.
     */
    class edge_opposite_direction_sorter {
    public:
        /** @brief Constructor
         * @param[in] e The edge to which the sorting relates
         * @param[in] n The node to consider
         */
        explicit edge_opposite_direction_sorter(const NBEdge * const e, const NBNode * const n) throw()
            : myNode(n) {
                myAngle = getEdgeAngleAt(e, n);
        }

        /** @brief Comparing operation
         * @param[in] e1 The first edge to compare
         * @param[in] e2 The second edge to compare
         * @return Which edge is more opposite to the related one
         */
        int operator()(NBEdge *e1, NBEdge *e2) const throw() {
            SUMOReal d1 = getDiff(e1);
            SUMOReal d2 = getDiff(e2);
            return d1 > d2;
        }

    protected:
        /** @brief Computes the angle difference between the related and the given edge
         * @param[in] e The edge to compare the angle difference of
         * @return The angle difference
         */
        SUMOReal getDiff(const NBEdge * const e) const throw() {
            SUMOReal d = getEdgeAngleAt(e, myNode);
            return GeomHelper::getMinAngleDiff(d, myAngle);
        }

        /** @brief Returns the given edge's angle at the given node
         *
         * Please note that we always consider the "outgoing direction".
         * @param[in] e The edge to which the sorting relates
         * @param[in] n The node to consider
         */
        SUMOReal getEdgeAngleAt(const NBEdge * const e, const NBNode * const n) const throw() {
            if(e->getFromNode()==n) {
                return e->getGeometry().getBegLine().atan2DegreeAngle();
            } else {
                return e->getGeometry().getEndLine().reverse().atan2DegreeAngle();
            }
        }

    private:
        /// @brief The angle of the related edge at the given node
        SUMOReal myAngle;

        /// @brief The related node
        const NBNode * const myNode;

    };

    // ---------------------------

    /**
     * edge_similar_direction_sorter
     * Class to sort edges by their angle in relation to the given edge
     * The resulting list should have the edge in the most similar direction
     * to the given edge as her first entry
     */
    class edge_similar_direction_sorter {
    public:
        /// constructor
        explicit edge_similar_direction_sorter(NBEdge *e)
                : myAngle(e->getAngle()) {}

        /// comparing operation
        int operator()(NBEdge *e1, NBEdge *e2) const {
            SUMOReal d1 = GeomHelper::getMinAngleDiff(e1->getAngle(), myAngle);
            SUMOReal d2 = GeomHelper::getMinAngleDiff(e2->getAngle(), myAngle);
            return d1 < d2;
        }

    private:
        /// the angle to find the edge with the opposite direction
        SUMOReal myAngle;
    };


    /**
     * @class node_with_incoming_finder
     */
    class node_with_incoming_finder {
    public:
        /// constructor
        node_with_incoming_finder(const NBEdge * const e);

        bool operator()(const NBNode * const n) const;

    private:
        const NBEdge * const myEdge;

    private:
        /// @brief invalidated assignment operator
        node_with_incoming_finder &operator=(const node_with_incoming_finder &s);

    };


    /**
     * @class node_with_outgoing_finder
     */
    class node_with_outgoing_finder {
    public:
        /// constructor
        node_with_outgoing_finder(const NBEdge * const e);

        bool operator()(const NBNode * const n) const;

    private:
        const NBEdge * const myEdge;

    private:
        /// @brief invalidated assignment operator
        node_with_outgoing_finder &operator=(const node_with_outgoing_finder &s);

    };




    class edge_with_destination_finder {
    public:
        /// constructor
        edge_with_destination_finder(NBNode *dest);

        bool operator()(NBEdge *e) const;

    private:
        NBNode *myDestinationNode;

    private:
        /// @brief invalidated assignment operator
        edge_with_destination_finder &operator=(const edge_with_destination_finder &s);

    };


    /** Tries to return the first edge within the given container which
        connects both given nodes */
    static NBEdge *findConnectingEdge(const EdgeVector &edges,
                                      NBNode *from, NBNode *to);


    /** returns the maximum speed allowed on the edges */
    static SUMOReal maxSpeed(const EdgeVector &ev);

    /**
     * same_connection_edge_sorter
     * This class is used to sort edges which connect the same nodes.
     * The edges are sorted in dependence to edges connecting them. The
     * rightmost will be the first in the list; the leftmost the last one.
     */
    class same_connection_edge_sorter {
    public:
        /// constructor
        explicit same_connection_edge_sorter() { }

        /// comparing operation
        int operator()(NBEdge *e1, NBEdge *e2) const {
            std::pair<SUMOReal, SUMOReal> mm1 = getMinMaxRelAngles(e1);
            std::pair<SUMOReal, SUMOReal> mm2 = getMinMaxRelAngles(e2);
            if (mm1.first==mm2.first && mm1.second==mm2.second) {
                // ok, let's simply sort them arbitrarily
                return e1->getID() < e2->getID();
            }

            assert(
                (mm1.first<=mm2.first&&mm1.second<=mm2.second)
                ||
                (mm1.first>=mm2.first&&mm1.second>=mm2.second));
            return (mm1.first>=mm2.first&&mm1.second>=mm2.second);
        }

        /**
         *
         */
        std::pair<SUMOReal, SUMOReal> getMinMaxRelAngles(NBEdge *e) const {
            SUMOReal min = 360;
            SUMOReal max = 360;
            const EdgeVector &ev = e->getConnectedEdges();
            for (EdgeVector::const_iterator i=ev.begin(); i!=ev.end(); ++i) {
                SUMOReal angle = NBHelpers::normRelAngle(
                                     e->getAngle(), (*i)->getAngle());
                if (min==360||min>angle) {
                    min = angle;
                }
                if (max==360||max<angle) {
                    max = angle;
                }
            }
            return std::pair<SUMOReal, SUMOReal>(min, max);
        }
    };


    friend std::ostream &operator<<(std::ostream &os, const EdgeVector &ev);

    class opposite_finder {
    public:
        /// constructor
        opposite_finder(NBEdge *edge, const NBNode *n)
                : myReferenceEdge(edge), myAtNode(n) { }

        bool operator()(NBEdge *e) const {
            return e->isTurningDirectionAt(myAtNode, myReferenceEdge)||
                   myReferenceEdge->isTurningDirectionAt(myAtNode, e);
        }

    private:
        NBEdge *myReferenceEdge;
        const NBNode *myAtNode;

    };


};


#endif

/****************************************************************************/

