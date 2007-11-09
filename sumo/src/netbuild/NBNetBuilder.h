/****************************************************************************/
/// @file    NBNetBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	The instance responsible for building networks
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
#ifndef NBNetBuilder_h
#define NBNetBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"

class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNetBuilder
 */
class NBNetBuilder
{
public:
    /// Constructor
    NBNetBuilder();

    /// Destructor
    ~NBNetBuilder();

    void buildLoaded();

    static void insertNetBuildOptions(OptionsCont &oc);

    void preCheckOptions(OptionsCont &oc);

    /**
     * @brief computes the structures
     * the order of the computation steps is not variable!!!
     */
    void compute(OptionsCont &oc);

    /** saves the net */
    bool save(OutputDevice &device, OptionsCont &oc);

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
    void removeDummyEdges(int &step);

    /** joins edges which connect the same nodes */
    void joinEdges(int &step);

    void removeUnwishedNodes(int &step, OptionsCont &oc);
    void removeUnwishedEdges(int &step, OptionsCont &oc);

    void guessRamps(int &step, OptionsCont &oc);
    void guessTLs(int &step, OptionsCont &oc);

    void splitGeometry(int &step, OptionsCont &oc);

    /** computes the turning direction for each edge */
    void computeTurningDirections(int &step);

    /** sorts the edges of a node */
    void sortNodesEdges(int &step);

    /** sets the node positions in a way that nodes are lying at zero */
    void normaliseNodePositions(int &step);

    /** computes edges 2 edges - relationships
        (step1: computation of approached edges) */
    void computeEdge2Edges(int &step);

    /** computes edges 2 edges - relationships
        (step2: computation of which lanes approach the edges) */
    void computeLanes2Edges(int &step);

    /** computes edges 2 edges - relationships
        (step3: division of lanes to approached edges) */
    void computeLanes2Lanes(int &step);

    /** rechecks whether all lanes have a following lane/edge */
    void recheckLanes(int &step);

    void initJoinedEdgesInformation();

    void computeNodeShapes(int &step);
    void computeEdgeShapes(int &step);

    void setTLControllingInformation(int &step);

    /** appends the turnarounds */
    void appendTurnarounds(int &step, OptionsCont &oc);

    /** computes nodes' logics */
    void computeLogic(int &step, OptionsCont &oc);

    /** computes nodes' tl-logics */
    void computeTLLogic(int &step, OptionsCont &oc);

    void reshiftRotateNet(int &step, OptionsCont &oc);

    void checkPrint(OptionsCont &oc) ;

    bool saveMap(const std::string &path);

protected:
    bool myHaveBuildNet;

    NBEdgeCont myEdgeCont;
    NBNodeCont myNodeCont;
    NBTypeCont myTypeCont;
    NBTrafficLightLogicCont myTLLCont;
    NBJunctionLogicCont myJunctionLogicCont;
    NBDistrictCont myDistrictCont;

};


#endif

/****************************************************************************/

