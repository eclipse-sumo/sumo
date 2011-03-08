/****************************************************************************/
/// @file    NBNetBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Instance responsible for building networks
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
#include <vector>
#include <set>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBNode.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNetBuilder
 * @brief Instance responsible for building networks
 *
 * The class' - and the netbuild module's - functionality is embedded within the
 *  buildLoaded() method. Here, the network representation is completed using compute(),
 *  first. Then, the output file is built and the network is saved into it using save().
 *
 * @addtogroup netbuild
 * @{
 *
 * -# Removing dummy edges
 *  \n Removes edges which end at the node they start at using NBNodeCont::removeDummyEdges().
 * -# Joining double connections
 *  \n Joins edges between same nodes using NBNodeCont::recheckEdges(). 
 * -# Finding isolated roads (optional)
 * -# Removing empty nodes and geometry nodes (optional)
 *  \n Removed nodes with no incoming/outgoing edges and nodes which can be transformed into 
 *   geometry point using NBNodeCont::removeUnwishedNodes().
 * -# Removing unwished edges (optional)
 *  \n If "keep-edges.postload" and "keep-edges" are set, the edges not with "keep-edges" are
 *   removed from the network using NBEdgeCont::removeUnwishedEdges().
 * -# Rechecking nodes after edge removal (optional)
 *  \n If any of the edge removing options was set ("keep-edges", "remove-edges", "keep-edges.postload",
 *   "keep-edges.by-vclass", "keep-edges.input-file"), the now orphaned nodes are removed using 
 *   NBNodeCont::removeUnwishedNodes().
 * -# Splitting geometry edges (optional)
 *  \n If "split-geometry" is set, edge geometries are converted to nodes using 
 *   NBEdgeCont::splitGeometry().
 * -# Normalising/transposing node positions
 *  \n If "disable-normalize-node-positions", "x-offset-to-apply", and "y-offset-to-apply" are not
 *   set, teh road graph's instances are moved to the origin.
 * -# Guessing and setting on-/off-ramps
 * -# Guessing and setting TLs
 * -# Computing turning directions
 * -# Sorting nodes' edges
 * -# Guessing and setting roundabouts
 * -# Computing Approached Edges
 * -# Computing Approaching Lanes
 * -# Dividing of Lanes on Approached Lanes
 * -# Appending Turnarounds (optional)
 * -# Rechecking of lane endings
 * -# Computing node shapes
 * -# Computing edge shapes
 * -# Computing tls logics
 * -# Computing node logics
 * -# Computing traffic light logics
 *
 *  @todo Removing unwished edges: Recheck whether this can be done during loading - whether this option/step is really needed.
 *  @todo Finding isolated roads: Describe 
 *  @bug Removing empty nodes and geometry nodes: Ok, empty nodes should be removed, uh? But this is only done if "remove-geometry" is set. 
 * @}
 */
class NBNetBuilder {
public:
    /// @brief Constructor
    NBNetBuilder() throw();

    /// @brief Destructor
    ~NBNetBuilder() throw();


    /** @brief Initialises the storage by applying given options
     *
     * Options, mainly steering the acceptance of edges, are parsed
     *  and the according internal variables are set.
     *
     * @param[in] oc The options container to read options from
     * @exception ProcessError If something fails (message is included)
     */
    void applyOptions(OptionsCont &oc) throw(ProcessError);


    /** @brief Builds a network using the loaded structures and saves it
     * @param[in] oc The options container to read options from
     */
    void buildLoaded(OptionsCont& oc);



    /// @name Retrieval of subcontainers
    /// @{

    /** @brief Returns the edge container
     * @return The edge container (reference)
     */
    NBEdgeCont &getEdgeCont() throw() {
        return myEdgeCont;
    }


    /** @brief Returns the node container
     * @return The node container (reference)
     */
    NBNodeCont &getNodeCont() throw() {
        return myNodeCont;
    }


    /** @brief Returns the type container
     * @return The type container (reference)
     */
    NBTypeCont &getTypeCont() throw() {
        return myTypeCont;
    }


    /** @brief Returns the traffic light logics container
     * @return The traffic light logics container (reference)
     */
    NBTrafficLightLogicCont &getTLLogicCont() throw() {
        return myTLLCont;
    }


    /** @brief Returns the junction logics container
     * @return The junction logics container (reference)
     */
    NBJunctionLogicCont &getJunctionLogicCont() throw() {
        return myJunctionLogicCont;
    }


    /** @brief Returns the districts container
     * @return The districts container (reference)
     */
    NBDistrictCont &getDistrictCont() throw() {
        return myDistrictCont;
    }
    /// @}



    /** @brief Adds net building options into the given options container
     *
     * @param[in] oc The options container to add net building options to
     */
    static void insertNetBuildOptions(OptionsCont &oc);


protected:
    /**
     * @brief Performs the network building steps
     *
     * @param[in] oc Container that contains options for building
     * @exception ProcessError (recheck)
     */
    void compute(OptionsCont &oc) throw(ProcessError);


    /** @brief Writes information about the currently processed step
     *
     * @param[in, out] step The current building step (incremented)
     * @param[in] Brief description about what is done in this step
     */
    void inform(int &step, const std::string &about) throw();



    /// @name Output methods
    /// @{

    /** @brief Saves the generated network
     *
     * Instances the network is made of are written by called methods from
     *  sub-containers.
     *
     * @param[in] device The device to write the network into
     * @param[in] oc Options needed to know what shall be saved (mainly whether internal links shall be saved)
     * @exception IOError (not yet implemented)
     * @see NBDistrictCont::writeXML
     * @see NBEdgeCont::writeXMLStep1
     * @see NBJunctionLogicCont::writeXML
     * @see NBTrafficLightLogicCont::writeXML
     * @see NBNodeCont::writeXML
     * @see NBNodeCont::writeXMLInternalNodes
     * @see NBEdgeCont::writeXMLStep2
     * @see NBNodeCont::writeXMLInternalSuccInfos
     */
    void save(OutputDevice &device, OptionsCont &oc) throw(IOError);
    /// @}



    /**
     * @class by_id_sorter
     * @brief Sorts nodes by their ids
     */
    class by_id_sorter {
    public:
        /// @brief constructor
        explicit by_id_sorter() { }

        int operator()(const NBNode *n1, const NBNode *n2) const {
            return n1->getID()<n2->getID();
        }

    };

protected:
    /// @brief The used container for nodes
    NBNodeCont myNodeCont;

    /// @brief The used container for street types
    NBTypeCont myTypeCont;

    /// @brief The used container for edges
    NBEdgeCont myEdgeCont;

    /// @brief The used container for traffic light logics
    NBTrafficLightLogicCont myTLLCont;

    /// @brief The used container for junction logics
    NBJunctionLogicCont myJunctionLogicCont;

    /// @brief The used container for districts
    NBDistrictCont myDistrictCont;

    /// @brief Edges marked as belonging to a roundabout (each set is a roundabout)
    std::vector<std::set<NBEdge*> > myRoundabouts;


private:
    /// @brief invalidated copy constructor
    NBNetBuilder(const NBNetBuilder &s);

    /// @brief invalidated assignment operator
    NBNetBuilder &operator=(const NBNetBuilder &s);

};


#endif

/****************************************************************************/

