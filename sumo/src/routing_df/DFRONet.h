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
	DFRONet( RONet * ro, bool amInHighwayMode );
	DFRONet();
	~DFRONet();
	void buildApproachList();
    size_t numberOfEdgesBefore(ROEdge *edge) const;
    const std::vector<ROEdge*> &getEdgesBefore(ROEdge *edge) const;
    size_t numberOfEdgesAfter(ROEdge *edge) const;
    const std::vector<ROEdge*> &getEdgesAfter(ROEdge *edge) const;

    void computeTypes(DFDetectorCon &dets) const;
    void buildRoutes(DFDetectorCon &det) const;
	SUMOReal getAbsPos(const DFDetector &det) const;


protected:
    bool isSource(const DFDetector &det,
		const DFDetectorCon &detectors) const;
    bool isFalseSource(const DFDetector &det,
		const DFDetectorCon &detectors) const;
    bool isDestination(const DFDetector &det,
		const DFDetectorCon &detectors) const;

    ROEdge *getDetectorEdge(const DFDetector &det) const;
    bool isSource(const DFDetector &det, ROEdge *edge,
		std::vector<ROEdge*> seen, const DFDetectorCon &detectors) const;
    bool isFalseSource(const DFDetector &det, ROEdge *edge,
		std::vector<ROEdge*> seen, const DFDetectorCon &detectors) const;
    bool isDestination(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen,
		const DFDetectorCon &detectors) const;

    void computeRoutesFor(ROEdge *edge, DFRORouteDesc *base, int no,
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

protected:
    class DFRouteDescByTimeComperator {
    public:
        /// Constructor
        explicit DFRouteDescByTimeComperator() { }

        /// Destructor
        ~DFRouteDescByTimeComperator() { }

        /// Comparing method
        bool operator()(const DFRORouteDesc *nod1, const DFRORouteDesc *nod2) const {
            return nod1->duration>nod2->duration;
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

};

#endif
