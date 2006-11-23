#ifndef DFRONet_h
#define DFRONet_h
/***************************************************************************
                          DFDetFlowLoader.h
    A DFROUTER-network
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.11  2006/11/23 12:26:08  dkrajzew
// parser for elmar deector definitions added
//
// Revision 1.10  2006/08/01 11:30:20  dkrajzew
// patching building
//
// Revision 1.10  2006/04/11 11:07:58  dksumo
// debugging
//
// Revision 1.9  2006/04/05 05:35:25  dksumo
// further work on the dfrouter
//
// Revision 1.8  2006/03/27 07:32:19  dksumo
// some further work...
//
// Revision 1.7  2006/03/17 09:04:18  dksumo
// class-documentation added/patched
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

#include <router/RONet.h>
#include <utils/options/OptionsCont.h>
#include <routing_df/DFDetector.h>
#include <routing_df/DFRORouteDesc.h>
#include <routing_df/DFRORouteCont.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DFRONet
 * @brief A DFROUTER-network
 */
class DFRONet
{
public:
	DFRONet( RONet * ro, bool amInHighwayMode );
	DFRONet();
	~DFRONet();
	void buildApproachList();
    size_t numberOfEdgesBefore(ROEdge *edge) const;
    const std::vector<ROEdge*> &getEdgesBefore(ROEdge *edge) const;
    size_t numberOfEdgesAfter(ROEdge *edge) const;
    const std::vector<ROEdge*> &getEdgesAfter(ROEdge *edge) const;

    void computeTypes(DFDetectorCon &dets,
        bool sourcesStrict) const;
    void buildRoutes(DFDetectorCon &det, bool allEndFollower,
        bool keepUnfoundEnds, bool includeInBetween) const;
	SUMOReal getAbsPos(const DFDetector &det) const;

    void buildEdgeFlowMap(const DFDetectorFlows &flows,
        const DFDetectorCon &detectors,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

    void revalidateFlows(const DFDetectorCon &detectors,
        DFDetectorFlows &flows,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);


    void removeEmptyDetectors(DFDetectorCon &detectors,
        DFDetectorFlows &flows,  SUMOTime startTime, SUMOTime endTime,
        SUMOTime stepOffset);

    ROEdge *getEdge(const std::string &name) const;


protected:
    void revalidateFlows(const DFDetector *detector,
        DFDetectorFlows &flows,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
    bool isSource(const DFDetector &det,
		const DFDetectorCon &detectors, bool strict) const;
    bool isFalseSource(const DFDetector &det,
		const DFDetectorCon &detectors) const;
    bool isDestination(const DFDetector &det,
		const DFDetectorCon &detectors) const;

    ROEdge *getDetectorEdge(const DFDetector &det) const;
    bool isSource(const DFDetector &det, ROEdge *edge,
		std::vector<ROEdge*> seen, const DFDetectorCon &detectors,
        bool strict) const;
    bool isFalseSource(const DFDetector &det, ROEdge *edge,
		std::vector<ROEdge*> seen, const DFDetectorCon &detectors) const;
    bool isDestination(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen,
		const DFDetectorCon &detectors) const;

    void computeRoutesFor(ROEdge *edge, DFRORouteDesc *base, int no,
        bool allEndFollower, bool keepUnfoundEnds,
        std::vector<ROEdge*> &visited, const DFDetector &det,
        DFRORouteCont &into, const DFDetectorCon &detectors,
		std::vector<ROEdge*> &seen) const;

    void buildDetectorEdgeDependencies(DFDetectorCon &dets) const;

	bool hasApproaching(ROEdge *edge) const;
	bool hasApproached(ROEdge *edge) const;
	bool hasDetector(ROEdge *edge) const;

	std::string buildRouteID(const DFRORouteDesc &desc) const;

	bool hasInBetweenDetectorsOnly(ROEdge *edge,
		const DFDetectorCon &detectors) const;
	bool hasSourceDetector(ROEdge *edge,
		const DFDetectorCon &detectors) const;

    struct IterationEdge {
        int depth;
        ROEdge *edge;
    };

protected:
    class DFRouteDescByTimeComperator {
    public:
        /// Constructor
        explicit DFRouteDescByTimeComperator() { }

        /// Destructor
        ~DFRouteDescByTimeComperator() { }

        /// Comparing method
        bool operator()(const DFRORouteDesc *nod1, const DFRORouteDesc *nod2) const {
            return nod1->duration_2>nod2->duration_2;
        }
    };

private:
	RONet * ro;

	// map of edge name->list of names of this edge approaching edges
	std::map<ROEdge*, std::vector<ROEdge*> > myApproachingEdges;

	// map of edge name->list of names of edges approached by this edge
	std::map<ROEdge*, std::vector<ROEdge*> > myApproachedEdges;

    mutable std::map<ROEdge*, std::vector<std::string> > myDetectorsOnEdges;
    mutable std::map<std::string, ROEdge*> myDetectorEdges;

/*
	// edge->mapped edge
	std::map<std::string, std::string> edgemap;

	// edge->speed
	std::map<std::string, SUMOReal> speedmap;

	// edge->lanes
	std::map<std::string, int> lanemap;

	// edge->length
	std::map<std::string, SUMOReal> lengthmap;
*/

	bool myAmInHighwayMode;
    mutable size_t mySourceNumber, mySinkNumber, myInBetweenNumber, myInvalidNumber;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

