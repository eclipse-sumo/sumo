/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in OpenStreetMap format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
     * @param[in, out] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont& oc, NBNetBuilder& nb);


protected:
    /** @brief An internal representation of an OSM-node
     */
    struct NIOSMNode {
        NIOSMNode(SUMOLong _id, double _lon, double _lat) :
            id(_id), lon(_lon), lat(_lat), ele(0), tlsControlled(false), node(0) {}

        /// @brief The node's id
        SUMOLong id;
        /// @brief The longitude the node is located at
        double lon;
        /// @brief The latitude the node is located at
        double lat;
        /// @brief The elevation of this node 
        double ele;
        /// @brief Whether this is a tls controlled junction
        bool tlsControlled;
        /// @brief the NBNode that was instantiated
        NBNode* node;
    };


    /** @brief An internal definition of a loaded edge
     */
    struct Edge {

        Edge(SUMOLong _id) :
            id(_id), myNoLanes(-1), myNoLanesForward(0), myMaxSpeed(MAXSPEED_UNGIVEN),
            myCurrentIsRoad(false) {}

        /// @brief The edge's id
        SUMOLong id;
        /// @brief The edge's street name
        std::string streetName;
        /// @brief number of lanes, or -1 if unknown
        int myNoLanes;
        /// @brief number of lanes in forward direction or 0 if unknown, negative if backwards lanes are meant
        int myNoLanesForward;
        /// @brief maximum speed in km/h, or MAXSPEED_UNGIVEN
        double myMaxSpeed;
        /// @brief The type, stored in "highway" key
        std::string myHighWayType;
        /// @brief Information whether this is an one-way road
        std::string  myIsOneWay;
        /// @brief The list of nodes this edge is made of
        std::vector<SUMOLong> myCurrentNodes;
        /// @brief Information whether this is a road
        bool myCurrentIsRoad;
    };


    NIImporter_OpenStreetMap();

    ~NIImporter_OpenStreetMap();

    void load(const OptionsCont& oc, NBNetBuilder& nb);

private:
    /** @brief Functor which compares two NIOSMNodes according
     * to their coordinates
     */
    class CompareNodes {
    public:
        bool operator()(const NIOSMNode* n1, const NIOSMNode* n2) const {
            return (n1->lat > n2->lat) || (n1->lat == n2->lat && n1->lon > n2->lon);
        }
    };


    /// @brief The separator within newly created compound type names
    static const std::string compoundTypeSeparator;

    class CompareEdges;

    /** @brief the map from OSM node ids to actual nodes
     * @note: NIOSMNodes may appear multiple times due to substition
     */
    std::map<SUMOLong, NIOSMNode*> myOSMNodes;

    /// @brief the set of unique nodes used in NodesHandler, used when freeing memory
    std::set<NIOSMNode*, CompareNodes> myUniqueNodes;


    /** @brief the map from OSM way ids to edge objects */
    std::map<SUMOLong, Edge*> myEdges;

    /// @brief The compounds types that do not contain known types
    std::set<std::string> myUnusableTypes;

    /// @brief The compound types that have already been mapped to other known types
    std::map<std::string, std::string> myKnownCompoundTypes;

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
    NBNode* insertNodeChecking(SUMOLong id, NBNodeCont& nc, NBTrafficLightLogicCont& tlsc);


    /** @brief Builds an NBEdge
     *
     * @param[in] e The definition of the edge
     * @param[in] index The index of the edge (in the case it is split along her nodes)
     * @param[in] from The origin node of the edge
     * @param[in] to The destination node of the edge
     * @param[in] passed The list of passed nodes (geometry information)
     * @param[in] osmNodes Container of node definitions for getting information about nodes from
     * @param[in, out] The NetBuilder instance
     * @return the new index if the edge is split
     * @exception ProcessError If the edge could not be added to the container
     */
    int insertEdge(Edge* e, int index, NBNode* from, NBNode* to,
                   const std::vector<SUMOLong>& passed, NBNetBuilder& nb);


protected:
    static const SUMOReal MAXSPEED_UNGIVEN;
    static const SUMOLong INVALID_ID;

    /**
     * @class NodesHandler
     * @brief A class which extracts OSM-nodes from a parsed OSM-file
     */
    friend class NodesHandler;
    class NodesHandler : public SUMOSAXHandler {
    public:
        /** @brief Contructor
         * @param[in, out] toFill The nodes container to fill
         * @param[in, out] uniqueNodes The nodes container for ensuring uniqueness
         * @param[in] options The options to use
         */
        NodesHandler(std::map<SUMOLong, NIOSMNode*>& toFill,
                     std::set<NIOSMNode*, CompareNodes>& uniqueNodes);


        /// @brief Destructor
        ~NodesHandler();


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
        void myStartElement(int element, const SUMOSAXAttributes& attrs);


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(int element);
        //@}


    private:

        /// @brief The nodes container to fill
        std::map<SUMOLong, NIOSMNode*>& myToFill;

        /// @brief ID of the currently parsed node, for reporting mainly
        SUMOLong myLastNodeID;

        /// @brief Hierarchy helper for parsing a node's tags
        bool myIsInValidNodeTag;

        /// @brief The current hierarchy level
        int myHierarchyLevel;

        /// @brief the set of unique nodes (used for duplicate detection/substitution)
        std::set<NIOSMNode*, CompareNodes>& myUniqueNodes;


    private:
        /** @brief invalidated copy constructor */
        NodesHandler(const NodesHandler& s);

        /** @brief invalidated assignment operator */
        NodesHandler& operator=(const NodesHandler& s);

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
         * @param[in, out] toFill The edges container to fill with read edges
         */
        EdgesHandler(const std::map<SUMOLong, NIOSMNode*>& osmNodes,
                     std::map<SUMOLong, Edge*>& toFill);


        /// @brief Destructor
        ~EdgesHandler();


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
        void myStartElement(int element, const SUMOSAXAttributes& attrs);


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(int element);
        //@}


    private:
        /// @brief The previously parsed nodes
        const std::map<SUMOLong, NIOSMNode*>& myOSMNodes;

        /// @brief A map of built edges
        std::map<SUMOLong, Edge*>& myEdgeMap;

        /// @brief The currently built edge
        Edge* myCurrentEdge;

        /// @brief The element stack
        std::vector<int> myParentElements;

        /// @brief A map of non-numeric speed descriptions to their numeric values
        std::map<std::string, SUMOReal> mySpeedMap;

    private:
        /** @brief invalidated copy constructor */
        EdgesHandler(const EdgesHandler& s);

        /** @brief invalidated assignment operator */
        EdgesHandler& operator=(const EdgesHandler& s);

    };

    /**
     * @class RelationHandler
     * @brief A class which extracts relevant relation information from a parsed OSM-file
     *   - turn restrictions
     */
    class RelationHandler : public SUMOSAXHandler {
    public:
        /** @brief Constructor
         *
         * @param[in] osmNodes The previously parsed OSM-nodes
         * @param[in] osmEdges The previously parse OSM-edges
         */
        RelationHandler(const std::map<SUMOLong, NIOSMNode*>& osmNodes,
                        const std::map<SUMOLong, Edge*>& osmEdges);


        /// @brief Destructor
        ~RelationHandler();


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
        void myStartElement(int element, const SUMOSAXAttributes& attrs);


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(int element);
        //@}


    private:
        /// @brief The previously parsed nodes
        const std::map<SUMOLong, NIOSMNode*>& myOSMNodes;

        /// @brief The previously parsed edges
        const std::map<SUMOLong, Edge*>& myOSMEdges;

        /// @brief The currently parsed relation
        SUMOLong myCurrentRelation;

        /// @brief The element stack
        std::vector<int> myParentElements;

        /// @brief whether the currently parsed relation is a restriction
        bool myIsRestriction;

        /// @brief the origination way for the current restriction
        SUMOLong myFromWay;

        /// @brief the destination way for the current restriction
        SUMOLong myToWay;

        /// @brief the via node/way for the current restriction
        SUMOLong myViaNode;
        SUMOLong myViaWay;


        /** @enum RestrictionType
         * @brief whether the only allowed or the only forbidden connection is defined
         */
        enum RestrictionType {
            /// @brief The only valid connection is declared
            RESTRICTION_ONLY,
            /// @brief The only invalid connection is declared
            RESTRICTION_NO,
            /// @brief The relation tag was missing
            RESTRICTION_UNKNOWN
        };
        RestrictionType myRestrictionType;

        /// @brief reset members to their defaults for parsing a new relation
        void resetValues();

        /// @brief check whether a referenced way has a corresponding edge
        bool checkEdgeRef(SUMOLong ref) const;

        /// @brief try to apply the parsed restriction and return whether successful
        bool applyRestriction() const;

        /// @brief try to find the way segment among candidates
        NBEdge* findEdgeRef(SUMOLong wayRef, const std::vector<NBEdge*>& candidates) const;

    private:
        /** @brief invalidated copy constructor */
        RelationHandler(const RelationHandler& s);

        /** @brief invalidated assignment operator */
        RelationHandler& operator=(const RelationHandler& s);

    };

};


#endif

/****************************************************************************/

