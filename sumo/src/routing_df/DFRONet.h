#ifndef DFRONet_h
#define DFRONet_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <router/RONet.h>
#include <utils/options/OptionsCont.h>
#include <routing_df/DFDetector.h>
#include <routing_df/DFRORouteDesc.h>
#include <routing_df/DFRORouteCont.h>

class DFRONet

{
public:
	DFRONet( RONet * ro );
	DFRONet();
	~DFRONet();
	void buildApproachList();
    size_t numberOfEdgesBefore(ROEdge *edge) const;
    const std::vector<ROEdge*> &getEdgesBefore(ROEdge *edge) const;
    size_t numberOfEdgesAfter(ROEdge *edge) const;
    const std::vector<ROEdge*> &getEdgesAfter(ROEdge *edge) const;

    void computeTypes(DFDetectorCon &dets) const;
    void buildRoutes(DFDetectorCon &det, DFRORouteCont &into) const;


protected:
    bool isSource(const DFDetector &det) const;
    bool isHighwaySource(const DFDetector &det) const;
    bool isDestination(const DFDetector &det) const;

    ROEdge *getDetectorEdge(const DFDetector &det) const;
    bool isSource(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen) const;
    bool isHighwaySource(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen) const;
    bool isDestination(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen) const;

    void computeRoutesFor(ROEdge *edge, DFRORouteDesc base, int no,
        std::vector<ROEdge*> &visited, const DFDetector &det,
        DFRORouteCont &into) const;

    void buildDetectorEdgeDependencies(DFDetectorCon &dets) const;


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
};

#endif
