#ifndef NBContHelper_h
#define NBContHelper_h
/***************************************************************************
                          NBContHelper.h
              Some methods for traversing lists of edges
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.16  2005/09/23 06:01:05  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.15  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.14  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.13  2005/01/27 14:26:08  dkrajzew
// patched several problems on determination of the turning direction; code beautifying
//
// Revision 1.12  2004/07/02 09:32:26  dkrajzew
// mapping of joined edges names added; removal of edges with a too
//  low speed added
//
// Revision 1.11  2004/01/13 07:46:04  dkrajzew
// method for determining the back direction edge in a edge container added
//
// Revision 1.10  2004/01/12 15:25:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.9  2003/09/22 12:40:11  dkrajzew
// further work on vissim-import
//
// Revision 1.8  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.7  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.6  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
// Revision 1.5  2003/04/04 07:43:03  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of
//  edge sorting (false lane connections) debugged
//
// Revision 1.4  2003/03/17 14:22:32  dkrajzew
// further debug and windows eol removed
//
// Revision 1.3  2003/03/12 16:47:52  dkrajzew
// extension for artemis-import
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/06/11 16:00:39  dkrajzew
// windows eol removed; template class definition inclusion depends now on
//  the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.3  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.2  2002/04/25 14:15:07  dkrajzew
// The assignement of priorities of incoming edges improved; now,
//  if having equal priorities, the opposite edges are chosen as higher
//  priorised
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.2  2002/04/09 12:21:58  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBEdge.h"
#include "nodes/NBNode.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
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

    /** counts the prioirities of the edges within the given container */
    static int countPriorities(const EdgeVector &s);

    /**
     * edge_by_angle_sorter
     * Class to sort edges by their angle
     */
    class edge_by_junction_angle_sorter {
    public:
        /// constructor
        explicit edge_by_junction_angle_sorter(NBNode *n) : _node(n) {}

        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const;

    private:
        /// Converts the angle of the edge if it is an incoming edge
        SUMOReal getConvAngle(NBEdge *e) const;

    private:
        /// the edge to compute the relative angle of
        NBNode *_node;

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
            : _edge(e), _node(n) {}

    public:
        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const {
            SUMOReal relAngle1 = NBHelpers::normRelAngle(
                _edge->getAngle(), e1->getAngle());
            SUMOReal relAngle2 = NBHelpers::normRelAngle(
                _edge->getAngle(), e2->getAngle());
            return relAngle1 > relAngle2;
        }

    private:
        /// the edge to compute the relative angle of
        NBEdge *_edge;

        /// the node to use
        NBNode *_node;

    };

    /**
     * relative_edgelane_sorter
     * Class to sort edges by their angle
     */
    class relative_edgelane_sorter {
    public:
        /// constructor
        explicit relative_edgelane_sorter(NBEdge *e, NBNode *n)
            : _edge(e), _node(n) {}

    public:
        /// comparing operation
        int operator() (EdgeLane e1, EdgeLane e2) const {
            SUMOReal relAngle1 = NBHelpers::normRelAngle(
                _edge->getAngle(), e1.edge->getAngle());
            SUMOReal relAngle2 = NBHelpers::normRelAngle(
                _edge->getAngle(), e2.edge->getAngle());
            return relAngle1 > relAngle2;
        }

    private:
        /// the edge to compute the relative angle of
        NBEdge *_edge;

        /// the node to use
        NBNode *_node;

    };

    /**
     * edge_by_priority_sorter
     * Class to sort edges by their priority
     */
    class edge_by_priority_sorter {
    public:
        /// comparing operator
        int operator() (NBEdge *e1, NBEdge *e2) const {
            return e1->getPriority() > e2->getPriority();
        }
    };

    /**
     * edge_opposite_direction_sorter
     * Class to sort edges by their angle in relation to the given edge
     * The resulting list should have the edge in the most opposite direction
     * to the given edge as her first entry
     */
    class edge_opposite_direction_sorter {
    public:
        /// constructor
        explicit edge_opposite_direction_sorter(NBEdge *e)
            : _angle(e->getAngle()), _edge(e) {}

    public:
        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const {
            if(_edge==e2) {
                return 1;
            }
            if(_edge==e1) {
                return 0;
            }
            SUMOReal d1 = getDiff(e1);
            SUMOReal d2 = getDiff(e2);
            return d1 < d2;
        }

        /** helping method for the computation of the absolut difference
         * between the edges' angles
         */
        SUMOReal getDiff(NBEdge *e) const {
            SUMOReal d = e->getAngle()+180;
            if(d>=360) {
                d -= 360;
            }
            return fabs(d - _angle);
        }

    private:
        /// the angle to find the edge with the opposite direction
        SUMOReal _angle;

        /// the edge - to avoid comparison of an edge with itself
        NBEdge *_edge;

    };

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
            : _angle(e->getAngle()) {}

        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const {
            SUMOReal d1 = getDiff(e1);
            SUMOReal d2 = getDiff(e2);
            return d1 < d2;
        }

        /** helping method for the computation of the absolut difference
         * between the edges' angles
         */
        SUMOReal getDiff(NBEdge *e) const {
            SUMOReal d = e->getAngle();
            return fabs(d - _angle);
        }

    private:
        /// the angle to find the edge with the opposite direction
        SUMOReal _angle;
    };


    /**
     * @class node_with_incoming_finder
     */
    class node_with_incoming_finder {
    public:
        /// constructor
        node_with_incoming_finder(NBEdge *e);

        bool operator() (const NBNode * const n) const;

    private:
        NBEdge *_edge;

    };


    /**
     * @class node_with_outgoing_finder
     */
    class node_with_outgoing_finder {
    public:
        /// constructor
        node_with_outgoing_finder(NBEdge *e);

        bool operator() (const NBNode * const n) const;

    private:
        NBEdge *_edge;

    };


    /**
     * @class edgelane_finder
     */
    class edgelane_finder {
    public:
        /// constructor
        edgelane_finder(NBEdge *toEdge, int toLane);

        bool operator() (const EdgeLane &el) const;

    private:
        NBEdge *myDestinationEdge;

        int myDestinationLane;

    };


    class edge_with_destination_finder {
    public:
        /// constructor
        edge_with_destination_finder(NBNode *dest);

        bool operator() (NBEdge *e) const;

    private:
        NBNode *myDestinationNode;

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
        int operator() (NBEdge *e1, NBEdge *e2) const {
            std::pair<SUMOReal, SUMOReal> mm1 = getMinMaxRelAngles(e1);
            std::pair<SUMOReal, SUMOReal> mm2 = getMinMaxRelAngles(e2);
            assert(
                (mm1.first<=mm2.first&&mm1.second<=mm2.second)
                ||
                (mm1.first>=mm2.first&&mm1.second>=mm2.second) );
            return (mm1.first>=mm2.first&&mm1.second>=mm2.second);
        }

        /**
         *
         */
        std::pair<SUMOReal, SUMOReal> getMinMaxRelAngles(NBEdge *e) const {
            SUMOReal min = 360;
            SUMOReal max = 360;
            const EdgeVector &ev = e->getConnected();
            for(EdgeVector::const_iterator i=ev.begin(); i!=ev.end(); i++) {
                SUMOReal angle = NBHelpers::normRelAngle(
                    e->getAngle(), (*i)->getAngle());
                if(min==360||min>angle) {
                     min = angle;
                }
                if(max==360||max<angle) {
                    max = angle;
                }
            }
            return std::pair<SUMOReal, SUMOReal>(min, max);
        }
    };

    class edge_to_lane_sorter {
    private:
        bool flip;

    public:
        explicit edge_to_lane_sorter(NBNode *from, NBNode *to);
        int operator() (NBEdge *e1, NBEdge *e2) const;
    };

    friend std::ostream &operator<<(std::ostream &os, const EdgeVector &ev);

    class opposite_finder {
    public:
        /// constructor
        opposite_finder(NBEdge *edge, const NBNode *n)
            : myReferenceEdge(edge), myAtNode(n) { }

        bool operator() (NBEdge *e) const {
            return e->isTurningDirectionAt(myAtNode, myReferenceEdge)||
                myReferenceEdge->isTurningDirectionAt(myAtNode, e);
        }

    private:
        NBEdge *myReferenceEdge;
        const NBNode *myAtNode;

    };


};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
