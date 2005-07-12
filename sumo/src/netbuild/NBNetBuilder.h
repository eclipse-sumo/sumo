#ifndef NBNetBuilder_h
#define NBNetBuilder_h
//---------------------------------------------------------------------------//
//                        NBNetBuilder.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.14  2005/07/12 12:32:48  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "nodes/NBNodeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"

class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBNetBuilder {
public:
	/// Constructor
    NBNetBuilder();

	/// Destructor
    ~NBNetBuilder();

    void buildLoaded();

    static void insertNetBuildOptions(OptionsCont &oc);

    static void preCheckOptions(OptionsCont &oc);

    /**
     * @brief computes the structures
     * the order of the computation steps is not variable!!!
     */
    void compute(OptionsCont &oc);

    /** saves the net */
    bool save(std::string path, OptionsCont &oc);
    bool save(std::ostream &os, OptionsCont &oc);

    bool netBuild() const;

    NBEdgeCont &getEdgeCont();
    NBNodeCont &getNodeCont();
    NBTypeCont &getTypeCont();
    NBTrafficLightLogicCont &getTLLogicCont();
    NBJunctionLogicCont &getJunctionLogicCont();
    NBDistrictCont &getDistrictCont();


protected:

    void inform(int &step, const std::string &about);


    /** removes dummy edges from junctions */
    bool removeDummyEdges(int &step);

    /** joins edges which connect the same nodes */
    bool joinEdges(int &step);

    bool removeUnwishedNodes(int &step, OptionsCont &oc);
    bool removeUnwishedEdges(int &step, OptionsCont &oc);

    bool guessRamps(int &step, OptionsCont &oc);
    bool guessTLs(int &step, OptionsCont &oc);

    /** computes the turning direction for each edge */
    bool computeTurningDirections(int &step);

    /** sorts the edges of a node */
    bool sortNodesEdges(int &step);

    /** sets the node positions in a way that nodes are lying at zero */
    bool normaliseNodePositions(int &step);

    /** computes edges 2 edges - relationships
        (step1: computation of approached edges) */
    bool computeEdge2Edges(int &step);

    /** computes edges 2 edges - relationships
        (step2: computation of which lanes approach the edges) */
    bool computeLanes2Edges(int &step);

    /** computes edges 2 edges - relationships
        (step3: division of lanes to approached edges) */
    bool computeLanes2Lanes(int &step);

    /** rechecks whether all lanes have a following lane/edge */
    bool recheckLanes(int &step);

    void initJoinedEdgesInformation();

    bool computeNodeShapes(int &step);
    bool computeEdgeShapes(int &step);

    bool setTLControllingInformation(int &step);

    /** appends the turnarounds */
    bool appendTurnarounds(int &step, OptionsCont &oc);

    /** computes nodes' logics */
    bool computeLogic(int &step, OptionsCont &oc);

    /** computes nodes' tl-logics */
    bool computeTLLogic(int &step, OptionsCont &oc);

    bool reshiftRotateNet(int &step, OptionsCont &oc);

    void checkPrint(OptionsCont &oc) ;

    bool saveMap(std::string path);

    void savePlain(const std::string &filename);

protected:
    bool myHaveBuildNet;

    NBEdgeCont myEdgeCont;
    NBNodeCont myNodeCont;
    NBTypeCont myTypeCont;
    NBTrafficLightLogicCont myTLLCont;
    NBJunctionLogicCont myJunctionLogicCont;
    NBDistrictCont myDistrictCont;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
