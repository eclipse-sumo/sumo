/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in OpenStreetMap format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIImporter_OpenStreetMap_h
#define NIImporter_OpenStreetMap_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBEdgeCont;
class NBNetBuilder;
class NBNode;
class NBNodeCont;
class NBTrafficLightLogicCont;
class NBTypeCont;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_OpenStreetMap
 * @brief Importer for networks stored in OpenStreetMap format
 *
 */
class NIImporter_OpenStreetMap {
public:
    /** @brief Loads content of the optionally given OSM file
     *
     * If the option "osm-files" is set, the file(s) stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "osm-files" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont &oc, NBNetBuilder &nb);


protected:
    /** @brief An internal representation of an OSM-node
     */
    struct NIOSMNode {
        /// @brief The node's id
        int id;
        /// @brief The longitude the node is located at
        double lon;
        /// @brief The latitude the node is located at
        double lat;
        /// @brief Whether this is a tls controlled junction
        bool tlsControlled;
    };


    /** @brief An internal definition of a loaded edge
     */
    struct Edge {
        /// @brief The edge's id
        std::string id;
        /// @brief number of lanes, or -1 if unknown
        int myNoLanes;
        /// @brief maximum speed in km/h, or -1 if unknown
        double myMaxSpeed;
        /// @brief The type, stored in "highway" key
        std::string myHighWayType;
        /// @brief Information whether this is an one-way road
        std::string  myIsOneWay;
        /// @brief The list of nodes this edge is made of
        std::vector<int> myCurrentNodes;
        /// @brief Information whether this is a road
        bool myCurrentIsRoad;
    };


private:

    /// @brief The separator within newly created compound type names
    static const std::string compoundTypeSeparator;

    class CompareNodes;
    class SubstituteNode;
    class SimilarEdge;

    /** @brief Builds an NBNode
     *
     * If a node with the given id is already known, nothing is done.
     *  Otherwise, the position and other information of the node is retrieved from the
     *  given node map, the node is built and added to the given node container.
     * If the node is controlled by a tls, the according tls is built and added
     *  to the tls container.
     * @param[in] id The id of the node to build
     * @param[in] osmNodes Map of node ids to information about these
     * @param[in, out] nc The node container to add the built node to
     * @param[in, out] tlsc The traffic lights logic container to add the built tls to
     * @return The built/found node
     * @exception ProcessError If the tls could not be added to the container
     */
    static NBNode *insertNodeChecking(int id, const std::map<int, NIOSMNode*> &osmNodes,
                                      NBNodeCont &nc, NBTrafficLightLogicCont &tlsc) throw(ProcessError);


    /** @brief Builds an NBEdge
     *
     * @param[in] e The definition of the edge
     * @param[in] index The index of the edge (in the case it is split along her nodes)
     * @param[in] from The origin node of the edge
     * @param[in] to The destination node of the edge
     * @param[in] passed The list of passed nodes (geometry information)
     * @param[in] osmNodes Container of node definitions for getting information about nodes from
     * @param[in] nc The container for built nodes
     * @param[in, out] The edge container to add the built edge to
     * @param[in] tc The type container to get information about the edge from
     * @exception ProcessError If the edge could not be added to the container
     */
    static void insertEdge(Edge *e, int index, NBNode *from, NBNode *to,
                           const std::vector<int> &passed, const std::map<int, NIOSMNode*> &osmNodes,
                           NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc) throw(ProcessError);



    /**
     * @class NodesHandler
     * @brief A class which extracts OSM-nodes from a parsed OSM-file
     */
    class NodesHandler : public SUMOSAXHandler {
    public:
        /** @brief Contructor
         * @param[in] toFill The nodes container to fill
         * @param[in] options The options to use
         */
        NodesHandler(std::map<int, NIOSMNode*> &toFill) throw();


        /// @brief Destructor
        ~NodesHandler() throw();


    protected:
        /// @name inherited from GenericSAXHandler
        //@{

        /** @brief Called on the opening of a tag;
         *
         * @param[in] element ID of the currently opened element
         * @param[in] attrs Attributes within the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myStartElement
         */
        void myStartElement(SumoXMLTag element, const SUMOSAXAttributes &attrs) throw(ProcessError);


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(SumoXMLTag element) throw(ProcessError);
        //@}


    private:
        /// @brief The nodes container to fill
        std::map<int, NIOSMNode*> &myToFill;

        /// @brief ID of the currently parsed node, for reporting mainly
        int myLastNodeID;

        /// @brief Hierarchy helper for parsing a node's tags
        bool myIsInValidNodeTag;

        /// @brief The current hierarchy level
        int myHierarchyLevel;


    private:
        /** @brief invalidated copy constructor */
        NodesHandler(const NodesHandler &s);

        /** @brief invalidated assignment operator */
        NodesHandler &operator=(const NodesHandler &s);

    };



    /**
     * @class EdgesHandler
     * @brief A class which extracts OSM-edges from a parsed OSM-file
     */
    class EdgesHandler : public SUMOSAXHandler {
    public:
        /** @brief Constructor
         *
         * @param[in] osmNodes The previously parsed (osm-)nodes
         * @param[in] toFill The edges container to fill with read edges
         */
        EdgesHandler(const std::map<int, NIOSMNode*> &osmNodes,
                     std::map<std::string, Edge*> &toFill) throw();


        /// @brief Destructor
        ~EdgesHandler() throw();


    protected:
        /// @name inherited from GenericSAXHandler
        //@{

        /** @brief Called on the opening of a tag;
         *
         * @param[in] element ID of the currently opened element
         * @param[in] attrs Attributes within the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myStartElement
         */
        void myStartElement(SumoXMLTag element, const SUMOSAXAttributes &attrs) throw(ProcessError);


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(SumoXMLTag element) throw(ProcessError);
        //@}


    private:
        /// @brief The previously parsed nodes
        const std::map<int, NIOSMNode*> &myOSMNodes;

        /// @brief A map of built edges
        std::map<std::string, Edge*> &myEdgeMap;

        /// @brief The currently built edge
        Edge *myCurrentEdge;

        /// @brief The element stack
        std::vector<SumoXMLTag> myParentElements;

        /// @brief A map of non-numeric speed descriptions to their numeric values
        std::map<std::string, SUMOReal> mySpeedMap;


    private:
        /** @brief invalidated copy constructor */
        EdgesHandler(const EdgesHandler &s);

        /** @brief invalidated assignment operator */
        EdgesHandler &operator=(const EdgesHandler &s);

    };


};


#endif

/****************************************************************************/

