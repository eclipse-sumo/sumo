#ifndef NBNetBuilder_h
#define NBNetBuilder_h

#include <string>

class OptionsCont;

class NBNetBuilder {
public:
    NBNetBuilder();
    ~NBNetBuilder();
    void buildLoaded();
    static void insertNetBuildOptions(OptionsCont &oc);
protected:
    /**
     * @brief computes the structures
     * the order of the computation steps is not variable!!!
     */
    void compute(OptionsCont &oc);

    /** saves the net (not the junction logics) */
    bool save(std::string path, OptionsCont &oc);

    /** clears all structures */
//    void clearAll();

    void inform(int step, const std::string &about);


    /** removes dummy edges from junctions */
    bool removeDummyEdges(int step);

    /** joins edges which connect the same nodes */
    bool joinEdges(int step);

    bool removeUnwishedNodes(int step, OptionsCont &oc);

    /** computes the turning direction for each edge */
    bool computeTurningDirections(int step);

    /** sorts the edges of a node */
    bool sortNodesEdges(int step);

    /** sets the node positions in a way that nodes are lying at zero */
    bool normaliseNodePositions(int step);

    /** computes edges 2 edges - relationships
        (step1: computation of approached edges) */
    bool computeEdge2Edges(int step);

    /** computes edges 2 edges - relationships
        (step2: computation of which lanes approach the edges) */
    bool computeLanes2Edges(int step);

    /** computes edges 2 edges - relationships
        (step3: division of lanes to approached edges) */
    bool computeLanes2Lanes(int step);

    /** rechecks whether all lanes have a following lane/edge */
    bool recheckLanes(int step);

    bool computeNodeShapes(int step);
    bool computeEdgeShapes(int step);


    /** appends the turnarounds */
    bool appendTurnarounds(int step, OptionsCont &oc);

    /** computes nodes' logics */
    bool computeLogic(int step, OptionsCont &oc);

    /** computes nodes' tl-logics */
    bool computeTLLogic(int step, OptionsCont &oc);

    bool reshiftRotateNet(int step, OptionsCont &oc);

    void checkPrint(OptionsCont &oc) ;

};


#endif
