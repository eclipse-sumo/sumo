/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @author  Gregor Laemmel
/// @date    Mon, 14.04.2008
///
// Importer for networks stored in OpenStreetMap format
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Parameterised.h>
#include <netbuild/NBPTPlatform.h>


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

    /** @enum CycleWayType
     * @brief details on the kind of cycleway along this road
     */
    enum WayType {
        WAY_NONE = 0,
        WAY_FORWARD = 1,
        WAY_BACKWARD = 2,
        WAY_BOTH = WAY_FORWARD | WAY_BACKWARD,
        WAY_UNKNOWN = 4
    };

    /** @brief An internal representation of an OSM-node
     */
    struct NIOSMNode : public Parameterised {
        NIOSMNode(long long int _id, double _lon, double _lat)
            :
            id(_id), lon(_lon), lat(_lat), ele(0.),
            tlsControlled(false),
            pedestrianCrossing(false),
            railwayCrossing(false),
            railwaySignal(false),
            railwayBufferStop(false),
            ptStopPosition(false), ptStopLength(0), name(""),
            permissions(SVC_IGNORING),
            positionMeters(std::numeric_limits<double>::max()),
            myRailDirection(WAY_UNKNOWN),
            node(nullptr) { }

        /// @brief The node's id
        const long long int id;
        /// @brief The longitude the node is located at
        const double lon;
        /// @brief The latitude the node is located at
        const double lat;
        /// @brief The elevation of this node
        double ele;
        /// @brief Whether this is a tls controlled junction
        bool tlsControlled;
        /// @brief Whether this is a pedestrian crossing
        bool pedestrianCrossing;
        /// @brief Whether this is a railway crossing
        bool railwayCrossing;
        /// @brief Whether this is a railway (main) signal
        bool railwaySignal;
        /// @brief Whether this is a railway buffer stop
        bool railwayBufferStop;
        /// @brief Whether this is a public transport stop position
        bool ptStopPosition;
        /// @brief The length of the pt stop
        double ptStopLength;
        /// @brief The name of the node
        std::string name;
        /// @brief type of pt stop
        SVCPermissions permissions;
        /// @brief kilometrage/mileage
        std::string position;
        /// @brief position converted to m (using highest precision available)
        double positionMeters;
        /// @brief Information about the direction(s) of railway usage
        WayType myRailDirection;
        /// @brief the NBNode that was instantiated
        NBNode* node;

    private:
        /// invalidated assignment operator
        NIOSMNode& operator=(const NIOSMNode& s) = delete;


    };

public:
    /// @brief translate osm transport designations into sumo vehicle class
    static SUMOVehicleClass interpretTransportType(const std::string& type, NIOSMNode* toSet = nullptr);

protected:


    enum ParkingType {
        PARKING_NONE = 0,
        PARKING_LEFT = 1,
        PARKING_RIGHT = 2,
        PARKING_BOTH = WAY_FORWARD | WAY_BACKWARD,
        PARKING_UNKNOWN = 4,
        PARKING_FORBIDDEN = 8,
        PARKING_PERPENDICULAR = 16,
        PARKING_DIAGONAL = 32
    };

    enum ChangeType {
        CHANGE_YES = 0,
        CHANGE_NO_LEFT = 1,
        CHANGE_NO_RIGHT = 2,
        CHANGE_NO = 3
    };

    /** @brief An internal definition of a loaded edge
     */
    class Edge : public Parameterised {
    public:
        explicit Edge(long long int _id) :
            id(_id), myNoLanes(-1), myNoLanesForward(0),
            myMaxSpeed(MAXSPEED_UNGIVEN),
            myMaxSpeedBackward(MAXSPEED_UNGIVEN),
            myExtraAllowed(0),
            myExtraDisallowed(0),
            myCyclewayType(WAY_UNKNOWN), // building of extra lane depends on bikelaneWidth of loaded typemap
            myBuswayType(WAY_NONE), // buslanes are always built when declared
            mySidewalkType(WAY_UNKNOWN), // building of extra lanes depends on sidewalkWidth of loaded typemap
            myRailDirection(WAY_UNKNOWN), // store direction(s) of railway usage
            myParkingType(PARKING_NONE), // parking areas exported optionally
            myChangeForward(CHANGE_YES),
            myChangeBackward(CHANGE_YES),
            myLayer(0), // layer is non-zero only in conflict areas
            myCurrentIsRoad(false),
            myAmInRoundabout(false),
            myWidth(-1)
        { }

        virtual ~Edge() {}

        /// @brief The edge's id
        const long long int id;
        /// @brief The edge's street name
        std::string streetName;
        /// @brief The edge's track name
        std::string ref;
        /// @brief number of lanes, or -1 if unknown
        int myNoLanes;
        /// @brief number of lanes in forward direction or 0 if unknown, negative if backwards lanes are meant
        int myNoLanesForward;
        /// @brief maximum speed in km/h, or MAXSPEED_UNGIVEN
        double myMaxSpeed;
        /// @brief maximum speed in km/h, or MAXSPEED_UNGIVEN
        double myMaxSpeedBackward;
        /// @brief Extra permissions added from tags instead of highway type
        SVCPermissions myExtraAllowed;
        /// @brief Extra permissions prohibited from tags instead of highway type
        SVCPermissions myExtraDisallowed;
        /// @brief The type, stored in "highway" key
        std::string myHighWayType;
        /// @brief Information whether this is an one-way road
        std::string myIsOneWay;
        /// @brief Information about the kind of cycleway along this road
        WayType myCyclewayType;
        /// @brief Information about the kind of busway along this road
        WayType myBuswayType;
        /// @brief Information about the kind of sidwalk along this road
        WayType mySidewalkType;
        /// @brief Information about the direction(s) of railway usage
        WayType myRailDirection;
        /// @brief Information about road-side parking
        int myParkingType;
        /// @brief Information about change prohibitions (forward direction
        int myChangeForward;
        /// @brief Information about change prohibitions (backward direction
        int myChangeBackward;
        /// @brief (optional) information about whether the forward lanes are designated to some SVCs
        std::vector<bool> myDesignatedLaneForward;
        /// @brief (optional) information about whether the backward lanes are designated to some SVCs
        std::vector<bool> myDesignatedLaneBackward;
        /// @brief (optional) information about additional allowed SVCs on forward lane(s)
        std::vector<SVCPermissions> myAllowedLaneForward;
        /// @brief (optional) information about additional allowed SVCs on backward lane(s)
        std::vector<SVCPermissions> myAllowedLaneBackward;
        /// @brief (optional) information about additional disallowed SVCs on forward lane(s)
        std::vector<SVCPermissions> myDisallowedLaneForward;
        /// @brief (optional) information about additional disallowed SVCs on backward lane(s)
        std::vector<SVCPermissions> myDisallowedLaneBackward;
        /// @brief Information about the relative z-ordering of ways
        int myLayer;
        /// @brief The list of nodes this edge is made of
        std::vector<long long int> myCurrentNodes;
        /// @brief Information whether this is a road
        bool myCurrentIsRoad;
        /// @brief Information whether this road is part of a roundabout
        bool myAmInRoundabout;
        /// @brief Additionally tagged information
        std::map<std::string, std::string> myExtraTags;
        /// @brief turning direction (arrows printed on the road)
        std::vector<int> myTurnSignsForward;
        std::vector<int> myTurnSignsBackward;
        /// @brief Information on lane width
        std::vector<double> myWidthLanesForward;
        std::vector<double> myWidthLanesBackward;
        double myWidth;

    private:
        /// invalidated assignment operator
        Edge& operator=(const Edge& s) = delete;
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
    std::map<long long int, NIOSMNode*> myOSMNodes;

    /// @brief the set of unique nodes used in NodesHandler, used when freeing memory
    std::set<NIOSMNode*, CompareNodes> myUniqueNodes;


    /** @brief the map from OSM way ids to edge objects */
    std::map<long long int, Edge*> myEdges;

    /** @brief the map from OSM way ids to platform shapes */
    std::map<long long int, Edge*> myPlatformShapes;

    /// @brief The compounds types that do not contain known types
    std::set<std::string> myUnusableTypes;

    /// @brief The compound types that have already been mapped to other known types
    std::map<std::string, std::string> myKnownCompoundTypes;

    /// @brief import lane specific access restrictions
    bool myImportLaneAccess;

    /// @brief import sidewalks
    bool myImportSidewalks;

    /// @brief import bike path specific permissions and directions
    bool myImportBikeAccess;

    /// @brief import crossings
    bool myImportCrossings;

    /// @brief import turning signals (turn:lanes) to guide connection building
    bool myImportTurnSigns;

    /// @brief whether additional way and node attributes shall be imported
    static bool myAllAttributes;

    /// @brief extra attributes to import
    static std::set<std::string> myExtraAttributes;

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
    NBNode* insertNodeChecking(long long int id, NBNodeCont& nc, NBTrafficLightLogicCont& tlsc);


    /** @brief Builds an NBEdge
     *
     * @param[in] e The definition of the edge
     * @param[in] index The index of the edge (in the case it is split along her nodes)
     * @param[in] from The origin node of the edge
     * @param[in] to The destination node of the edge
     * @param[in] passed The list of passed nodes (geometry information)
     * @param[in] osmNodes Container of node definitions for getting information about nodes from
     * @param[in, out] The NetBuilder instance
     * @param[in] first The first node of the way
     * @param[in] last The last node of the way
     * @return the new index if the edge is split
     * @exception ProcessError If the edge could not be added to the container
     */
    int insertEdge(Edge* e, int index, NBNode* from, NBNode* to,
                   const std::vector<long long int>& passed, NBNetBuilder& nb,
                   const NBNode* first, const NBNode* last);

    /// @brief reconstruct elevation from layer info
    void reconstructLayerElevation(double layerElevation, NBNetBuilder& nb);

    /// @brief collect neighboring nodes with their road distance and maximum between-speed. Search does not continue beyond knownElevation-nodes
    std::map<NBNode*, std::pair<double, double> >
    getNeighboringNodes(NBNode* node, double maxDist, const std::set<NBNode*>& knownElevation);

    /// @brief check whether the type is known or consists of known type compounds. return empty string otherwise
    std::string usableType(const std::string& type, const std::string& id, NBTypeCont& tc);

    /// @brief extend kilometrage data for all nodes along railway
    void extendRailwayDistances(Edge* e, NBTypeCont& tc);

    /// @brief read distance value from node and return value in m
    static double interpretDistance(NIOSMNode* node);

protected:
    static const double MAXSPEED_UNGIVEN;
    static const long long int INVALID_ID;

    static void applyChangeProhibition(NBEdge* e, int changeProhibition);
    /// Applies lane use information from `nie` to `e`. Uses the member values
    /// `myLaneAllowedForward`, `myLaneDisallowedForward` and `myLaneDesignatedForward`
    /// or the respective backward values to determine the ultimate lane uses.
    /// When a value of `e->myLaneDesignatedForward/Backward` is `true`, all permissions for the corresponding
    /// lane will be deleted before adding permissions from `e->myLaneAllowedForward/Backward`.
    /// SVCs from `e->myLaneAllowedForward/Backward` will be added to the existing permissions (for each lane).
    /// SVCs from `e->myLaneDisallowedForward/Backward` will be subtracted from the existing permissions.
    /// @brief Applies lane use information from `nie` to `e`.
    /// @param e The NBEdge that the new information will be written to.
    /// @param nie Ths Edge that the information comes from.
    void applyLaneUse(NBEdge* e, NIImporter_OpenStreetMap::Edge* nie, const bool forward);

    static void mergeTurnSigns(std::vector<int>& signs, std::vector<int> signs2);
    void applyTurnSigns(NBEdge* e, const std::vector<int>& turnSigns);

    /**
     * @class NodesHandler
     * @brief A class which extracts OSM-nodes from a parsed OSM-file
     */
    class NodesHandler : public SUMOSAXHandler {
    public:
        /** @brief Constructor
         * @param[in, out] toFill The nodes container to fill
         * @param[in, out] uniqueNodes The nodes container for ensuring uniqueness
         * @param[in] options The options to use
         */
        NodesHandler(std::map<long long int, NIOSMNode*>& toFill, std::set<NIOSMNode*,
                     CompareNodes>& uniqueNodes,
                     const OptionsCont& cont);


        /// @brief Destructor
        ~NodesHandler() override;

        int getDuplicateNodes() const {
            return myDuplicateNodes;
        }

        void resetHierarchy() {
            myHierarchyLevel = 0;
        }

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
        void myStartElement(int element, const SUMOSAXAttributes& attrs) override;


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(int element) override;
        //@}


    private:
        /// @brief The nodes container to fill
        std::map<long long int, NIOSMNode*>& myToFill;

        /// @brief id of the currently parsed node
        std::string myLastNodeID;

        /// @brief the currently parsed node
        NIOSMNode* myCurrentNode;

        /// @brief The current hierarchy level
        int myHierarchyLevel;

        /// @brief the set of unique nodes (used for duplicate detection/substitution)
        std::set<NIOSMNode*, CompareNodes>& myUniqueNodes;

        /// @brief whether elevation data should be imported
        const bool myImportElevation;

        /// @brief custom requirements for rail signal tagging
        StringVector myRailSignalRules;

        /// @brief number of diplicate nodes
        int myDuplicateNodes;

        /// @brief the options
        const OptionsCont& myOptionsCont;

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
        EdgesHandler(const std::map<long long int, NIOSMNode*>& osmNodes,
                     std::map<long long int, Edge*>& toFill, std::map<long long int, Edge*>& platformShapes);


        /// @brief Destructor
        ~EdgesHandler() override;


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
        void myStartElement(int element, const SUMOSAXAttributes& attrs) override;


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(int element) override;
        //@}

        double interpretSpeed(const std::string& key, std::string value);

        int interpretChangeType(const std::string& value) const;

        void interpretLaneUse(const std::string& value, SUMOVehicleClass svc, const bool forward) const;


    private:
        /// @brief The previously parsed nodes
        const std::map<long long int, NIOSMNode*>& myOSMNodes;

        /// @brief A map of built edges
        std::map<long long int, Edge*>& myEdgeMap;

        /// @brief A map of built edges
        std::map<long long int, Edge*>& myPlatformShapesMap;

        /// @brief The currently built edge
        Edge* myCurrentEdge = nullptr;

        /// @brief A map of non-numeric speed descriptions to their numeric values
        std::map<std::string, double> mySpeedMap;

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
        RelationHandler(const std::map<long long int, NIOSMNode*>& osmNodes,
                        const std::map<long long int, Edge*>& osmEdges, NBPTStopCont* nbptStopCont,
                        const std::map<long long int, Edge*>& platfromShapes, NBPTLineCont* nbptLineCont,
                        const OptionsCont& oc);


        /// @brief Destructor
        ~RelationHandler() override;

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
        void myStartElement(int element, const SUMOSAXAttributes& attrs) override;


        /** @brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        void myEndElement(int element) override;
        //@}


    private:
        /// @brief The previously parsed nodes
        const std::map<long long int, NIOSMNode*>& myOSMNodes;

        /// @brief The previously parsed edges
        const std::map<long long int, Edge*>& myOSMEdges;

        /// @brief The previously parsed platform shapes
        const std::map<long long int, Edge*>& myPlatformShapes;

        /// @brief The previously filled pt stop container
        NBPTStopCont* myNBPTStopCont;

        /// @brief PT Line container to be filled
        NBPTLineCont* myNBPTLineCont;

        /// @brief The currently parsed relation
        long long int myCurrentRelation;

        /// @brief whether the currently parsed relation is a restriction
        bool myIsRestriction;

        /// @brief exceptions to the restriction currenlty being parsed
        SVCPermissions myRestrictionException;

        /// @brief the origination way for the current restriction
        long long int myFromWay;

        /// @brief the destination way for the current restriction
        long long int myToWay;

        /// @brief the via node/way for the current restriction
        long long int myViaNode;
        long long int myViaWay;


        /// @brief the options cont
        const OptionsCont& myOptionsCont;

        /** @enum RestrictionType
         * @brief whether the only allowed or the only forbidden connection is defined
         */
        enum class RestrictionType {
            /// @brief The only valid connection is declared
            ONLY,
            /// @brief The only invalid connection is declared
            NO,
            /// @brief The relation tag was missing
            UNKNOWN
        };
        RestrictionType myRestrictionType;

        /// @brief reset members to their defaults for parsing a new relation
        void resetValues();

        /// @brief check whether a referenced way has a corresponding edge
        bool checkEdgeRef(long long int ref) const;

        /// @brief try to apply the parsed restriction and return whether successful
        bool applyRestriction() const;

        /// @brief try to find the way segment among candidates
        NBEdge* findEdgeRef(long long int wayRef, const std::vector<NBEdge*>& candidates) const;

    private:
        /** @brief invalidated copy constructor */
        RelationHandler(const RelationHandler& s);

        /** @brief invalidated assignment operator */
        RelationHandler& operator=(const RelationHandler& s);

        /// @brief bus stop references
        std::vector<long long int> myStops;

        /// @brief myStops which are actually platforms (in case there is no stop_position)
        std::set<long long int> myPlatformStops;


        struct NIIPTPlatform {
            long long int ref;
            bool isWay;
        };

        /// @brief bus stop platforms
        std::vector<NIIPTPlatform> myPlatforms;

        /// @brief ways in pt line references
        std::vector<long long int> myWays;

        /// @brief indicates whether current relation is a pt stop area
        bool myIsStopArea;

        /// @brief indicates whether current relation is a route
        bool myIsRoute;

        /// @brief indicates whether current relation is a pt route
        std::string myPTRouteType;

        /// @brief official route color
        RGBColor myRouteColor;

        /// @brief name of the relation
        std::string myName;

        /// @brief ref of the pt line
        std::string myRef;

        /// @brief service interval of the pt line in minutes
        int myInterval;

        /// @brief night service information of the pt line
        std::string myNightService;

        /** @brief the map from stop area member to stop_area id */
        std::map<long long int, long long int > myStopAreas;

    };

};
