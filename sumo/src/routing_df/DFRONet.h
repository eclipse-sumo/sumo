/****************************************************************************/
/// @file    DFRONet.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// missing_desc
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
#ifndef DFRONet_h
#define DFRONet_h
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

#include <router/RONet.h>
#include <utils/options/OptionsCont.h>
#include <routing_df/DFDetector.h>
#include <routing_df/DFRORouteDesc.h>
#include <routing_df/DFRORouteCont.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DFRONet
 * @brief A DFROUTER-network
 */
class DFRONet
{
public:
    DFRONet(RONet * ro, bool amInHighwayMode);
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
                     bool keepUnfoundEnds, bool includeInBetween,
                     bool keepShortestOnly) const;
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

    void buildDetectorDependencies(DFDetectorCon &detectors);
#ifdef HAVE_MESOSIM
    void mesoJoin(DFDetectorCon &detectors, DFDetectorFlows &flows);
#endif
    void computeID4Route(DFRORouteDesc &desc) const;


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
                  std::vector<ROEdge*> &seen, const DFDetectorCon &detectors,
                  bool strict) const;
    bool isFalseSource(const DFDetector &det, ROEdge *edge,
                       std::vector<ROEdge*> &seen, const DFDetectorCon &detectors) const;
    bool isDestination(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> &seen,
                       const DFDetectorCon &detectors) const;

    void computeRoutesFor(ROEdge *edge, DFRORouteDesc *base, int no,
                          bool allEndFollower, bool keepUnfoundEnds,
                          bool keepShortestOnly,
                          std::vector<ROEdge*> &visited, const DFDetector &det,
                          DFRORouteCont &into, const DFDetectorCon &detectors,
                          std::vector<ROEdge*> &seen) const;

    void buildDetectorEdgeDependencies(DFDetectorCon &dets) const;

    bool hasApproaching(ROEdge *edge) const;
    bool hasApproached(ROEdge *edge) const;
    bool hasDetector(ROEdge *edge) const;

    bool hasInBetweenDetectorsOnly(ROEdge *edge,
                                   const DFDetectorCon &detectors) const;
    bool hasSourceDetector(ROEdge *edge,
                           const DFDetectorCon &detectors) const;

    struct IterationEdge
    {
        int depth;
        ROEdge *edge;
    };

protected:
    class DFRouteDescByTimeComperator
    {
    public:
        /// Constructor
        explicit DFRouteDescByTimeComperator()
        { }

        /// Destructor
        ~DFRouteDescByTimeComperator()
        { }

        /// Comparing method
        bool operator()(const DFRORouteDesc *nod1, const DFRORouteDesc *nod2) const
        {
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
    mutable std::map<std::pair<ROEdge*, ROEdge*>, int> myConnectionOccurences;

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


#endif

/****************************************************************************/

