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
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/06/11 16:00:39  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.3  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.2  2002/04/25 14:15:07  dkrajzew
// The assignement of priorities of incoming edges improved; now, if having equal priorities, the opposite edges are chosen as higher priorised
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <iostream>
#include <cmath>
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBEdge.h"
#include "NBNode.h"

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
    /** Returns the next edge from a clockwise sorted container
        When the edges are sorted clockwise in the given container, the
        method returns the next edge from the edge the given iterator is
        pointing at, in the clockwise direction.
        A pointer to the first edge of the list is returned when the given
        iterator is pointing at the last edge. */
    static EdgeCont::const_iterator nextCW(const EdgeCont * edges, 
        EdgeCont::const_iterator from);
    /** Returns the previous edge from a clockwise sorted cotainer
        When the edges are sorted clockwise in the given container, the
        method returns the next edge from the edge the given iterator
        is pointing at, in the counter-clockwise direction.
        A pointer to the last edge of the list is returned when the given
        iterator is pointing at the first edge */
    static EdgeCont::const_iterator nextCCW(const EdgeCont * edges, 
        EdgeCont::const_iterator from);



    static std::ostream &out(std::ostream &os, const std::vector<bool> &v);

    static int countPriorities(const EdgeCont &s);

    /**
     * edge_by_angle_sorter
     * Class to sort edges by their angle
     */
    class edge_by_junction_angle_sorter {
    private:
        /// the edge to compute the relative angle of
        NBNode *_node;
    public:
        explicit edge_by_junction_angle_sorter(NBNode *n) : _node(n) {}
    public:
        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const {
            return e1->getJunctionAngle(_node) < e2->getJunctionAngle(_node);
        }
    };

    /**
     * relative_edge_sorter
     * Class to sort edges by their angle
     */
    class relative_edge_sorter {
    private:
        /// the edge to compute the relative angle of
        NBEdge *_edge;
        /// the node to use
        NBNode *_node;
    public:
        explicit relative_edge_sorter(NBEdge *e, NBNode *n)
            : _edge(e), _node(n) {}
    public:
        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const {
            double relAngle1 = NBHelpers::normRelAngle(
                _edge->getAngle(), e1->getAngle());
            double relAngle2 = NBHelpers::normRelAngle(
                _edge->getAngle(), e2->getAngle());
            return relAngle1 > relAngle2;
        }
    };

    /**
     * relative_edgelane_sorter
     * Class to sort edges by their angle
     */
    class relative_edgelane_sorter {
    private:
        /// the edge to compute the relative angle of
        NBEdge *_edge;
        /// the node to use
        NBNode *_node;
    public:
        explicit relative_edgelane_sorter(NBEdge *e, NBNode *n)
            : _edge(e), _node(n) {}
    public:
        /// comparing operation
        int operator() (EdgeLane e1, EdgeLane e2) const {
            double relAngle1 = NBHelpers::normRelAngle(
                _edge->getAngle(), e1.edge->getAngle());
            double relAngle2 = NBHelpers::normRelAngle(
                _edge->getAngle(), e2.edge->getAngle());
            return relAngle1 > relAngle2;
        }
    };

    /**
     * edge_by_priority_sorter
     * Class to sort edges by their priority
     */
    class edge_by_priority_sorter {
    public:
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
    private:
        /// the angle to find the edge with the opposite direction
        double _angle;
        /// the edge - to avoid comparison of an edge with itself
        NBEdge *_edge;
    public:
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
            double d1 = getDiff(e1);
            double d2 = getDiff(e2);
            return d1 < d2;
        }

        /// helping method for the computation of the absolut difference between the edges' angles
        double getDiff(NBEdge *e) const {
            double d = e->getAngle()+180;
            if(d>=360) {
                d -= 360;
            }
            return fabs(d - _angle);
        }
    };

    /**
     * edge_similar_direction_sorter
     * Class to sort edges by their angle in relation to the given edge
     * The resulting list should have the edge in the most similar direction
     * to the given edge as her first entry
     */
    class edge_similar_direction_sorter {
    private:
        /// the angle to find the edge with the opposite direction
        double _angle;
    public:
        explicit edge_similar_direction_sorter(NBEdge *e)
            : _angle(e->getAngle()) {}
    public:
        /// comparing operation
        int operator() (NBEdge *e1, NBEdge *e2) const {
            double d1 = getDiff(e1);
            double d2 = getDiff(e2);
            return d1 < d2;
        }

        /// helping method for the computation of the absolut difference between the edges' angles
        double getDiff(NBEdge *e) const {
            double d = e->getAngle();
            return fabs(d - _angle);
        }
    };
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBContHelper.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
