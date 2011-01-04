/****************************************************************************/
/// @file    RODFNet.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A DFROUTER-network
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODFNet_h
#define RODFNet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/RONet.h>
#include <utils/options/OptionsCont.h>
#include "RODFDetector.h"
#include "RODFRouteDesc.h"
#include "RODFRouteCont.h"
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFNet
 * @brief A DFROUTER-network
 */
class RODFNet : public RONet {
public:
    /** @brief Constructor
     * @param[in] amInHighwayMode Whether search for following edges shall stop at slow edges
     */
    RODFNet(bool amInHighwayMode) throw();


    /// @brief Destructor
    ~RODFNet() throw();


    void buildApproachList();

    void computeTypes(RODFDetectorCon &dets,
                      bool sourcesStrict) const;
    void buildRoutes(RODFDetectorCon &det, bool allEndFollower,
                     bool keepUnfoundEnds, bool includeInBetween,
                     bool keepShortestOnly, int maxFollowingLength) const;
    SUMOReal getAbsPos(const RODFDetector &det) const;

    void buildEdgeFlowMap(const RODFDetectorFlows &flows,
                          const RODFDetectorCon &detectors,
                          SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

    void revalidateFlows(const RODFDetectorCon &detectors,
                         RODFDetectorFlows &flows,
                         SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);


    void removeEmptyDetectors(RODFDetectorCon &detectors,
                              RODFDetectorFlows &flows,  SUMOTime startTime, SUMOTime endTime,
                              SUMOTime stepOffset);

    void reportEmptyDetectors(RODFDetectorCon &detectors,
                              RODFDetectorFlows &flows);

    void buildDetectorDependencies(RODFDetectorCon &detectors);

    void mesoJoin(RODFDetectorCon &detectors, RODFDetectorFlows &flows);

    bool hasDetector(ROEdge *edge) const;
    const std::vector<std::string> &getDetectorList(ROEdge *edge) const;


protected:
    void revalidateFlows(const RODFDetector *detector,
                         RODFDetectorFlows &flows,
                         SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
    bool isSource(const RODFDetector &det,
                  const RODFDetectorCon &detectors, bool strict) const;
    bool isFalseSource(const RODFDetector &det,
                       const RODFDetectorCon &detectors) const;
    bool isDestination(const RODFDetector &det,
                       const RODFDetectorCon &detectors) const;

    ROEdge *getDetectorEdge(const RODFDetector &det) const;
    bool isSource(const RODFDetector &det, ROEdge *edge,
                  std::vector<ROEdge*> &seen, const RODFDetectorCon &detectors,
                  bool strict) const;
    bool isFalseSource(const RODFDetector &det, ROEdge *edge,
                       std::vector<ROEdge*> &seen, const RODFDetectorCon &detectors) const;
    bool isDestination(const RODFDetector &det, ROEdge *edge, std::vector<ROEdge*> &seen,
                       const RODFDetectorCon &detectors) const;

    void computeRoutesFor(ROEdge *edge, RODFRouteDesc &base, int no,
                          bool keepUnfoundEnds,
                          bool keepShortestOnly,
                          std::vector<ROEdge*> &visited, const RODFDetector &det,
                          RODFRouteCont &into, const RODFDetectorCon &detectors,
                          int maxFollowingLength,
                          std::vector<ROEdge*> &seen) const;

    void buildDetectorEdgeDependencies(RODFDetectorCon &dets) const;

    bool hasApproaching(ROEdge *edge) const;
    bool hasApproached(ROEdge *edge) const;

    bool hasInBetweenDetectorsOnly(ROEdge *edge,
                                   const RODFDetectorCon &detectors) const;
    bool hasSourceDetector(ROEdge *edge,
                           const RODFDetectorCon &detectors) const;

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
        bool operator()(const RODFRouteDesc &nod1, const RODFRouteDesc &nod2) const {
            return nod1.duration_2>nod2.duration_2;
        }
    };

private:
    /// @brief Map of edge name->list of names of this edge approaching edges
    std::map<ROEdge*, std::vector<ROEdge*> > myApproachingEdges;

    /// @brief Map of edge name->list of names of edges approached by this edge
    std::map<ROEdge*, std::vector<ROEdge*> > myApproachedEdges;

    mutable std::map<ROEdge*, std::vector<std::string> > myDetectorsOnEdges;
    mutable std::map<std::string, ROEdge*> myDetectorEdges;

    bool myAmInHighwayMode;
    mutable size_t mySourceNumber, mySinkNumber, myInBetweenNumber, myInvalidNumber;

    /// @brief List of ids of edges that shall not be used
    std::vector<std::string> myDisallowedEdges;


    bool myKeepTurnarounds;

};


#endif

/****************************************************************************/

