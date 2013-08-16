/****************************************************************************/
/// @file    NIImporter_OpenDrive.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in openDrive format
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
#ifndef NIImporter_OpenDrive_h
#define NIImporter_OpenDrive_h


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
#include <utils/xml/GenericSAXHandler.h>
#include <utils/geom/PositionVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;
class NBNode;
class NBNodeCont;


#define UNSET_CONNECTION 100000

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_OpenDrive
 * @brief Importer for networks stored in openDrive format
 *
 */
class NIImporter_OpenDrive : public GenericSAXHandler {
public:
    /** @brief Loads content of the optionally given SUMO file
     *
     * If the option "opendrive-files" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "opendrive-files" is not set, this method simply returns.
     *
     * The loading is done by parsing the network definition as an XML file
     *  using the SAXinterface and handling the incoming data via this class'
     *  methods.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont& oc, NBNetBuilder& nb);


protected:

    /**
     * @enum OpenDriveXMLTag
     * @brief Numbers representing openDrive-XML - element names
     * @see GenericSAXHandler
     */
    enum OpenDriveXMLTag {
        OPENDRIVE_TAG_NOTHING,
        OPENDRIVE_TAG_HEADER,
        OPENDRIVE_TAG_ROAD,
        OPENDRIVE_TAG_PREDECESSOR,
        OPENDRIVE_TAG_SUCCESSOR,
        /// @todo OPENDRIVE_TAG_NEIGHBOR,
        /// @todo OPENDRIVE_TAG_TYPE,
        OPENDRIVE_TAG_GEOMETRY,
        OPENDRIVE_TAG_LINE,
        OPENDRIVE_TAG_SPIRAL,
        OPENDRIVE_TAG_ARC,
        OPENDRIVE_TAG_POLY3,
        OPENDRIVE_TAG_LANESECTION,
        OPENDRIVE_TAG_LEFT,
        OPENDRIVE_TAG_CENTER,
        OPENDRIVE_TAG_RIGHT,
        OPENDRIVE_TAG_LANE,
        OPENDRIVE_TAG_SIGNAL,
        OPENDRIVE_TAG_JUNCTION,
        OPENDRIVE_TAG_CONNECTION,
        OPENDRIVE_TAG_LANELINK,
        OPENDRIVE_TAG_WIDTH,
        OPENDRIVE_TAG_SPEED
    };


    /**
     * @enum OpenDriveXMLAttr
     * @brief Numbers representing openDrive-XML - attributes
     * @see GenericSAXHandler
     */
    enum OpenDriveXMLAttr {
        OPENDRIVE_ATTR_NOTHING,
        OPENDRIVE_ATTR_REVMAJOR,
        OPENDRIVE_ATTR_REVMINOR,
        OPENDRIVE_ATTR_ID,
        OPENDRIVE_ATTR_LENGTH,
        OPENDRIVE_ATTR_JUNCTION,
        OPENDRIVE_ATTR_ELEMENTTYPE,
        OPENDRIVE_ATTR_ELEMENTID,
        OPENDRIVE_ATTR_CONTACTPOINT,
        OPENDRIVE_ATTR_S,
        OPENDRIVE_ATTR_X,
        OPENDRIVE_ATTR_Y,
        OPENDRIVE_ATTR_HDG,
        OPENDRIVE_ATTR_CURVSTART,
        OPENDRIVE_ATTR_CURVEND,
        OPENDRIVE_ATTR_CURVATURE,
        OPENDRIVE_ATTR_A,
        OPENDRIVE_ATTR_B,
        OPENDRIVE_ATTR_C,
        OPENDRIVE_ATTR_D,
        OPENDRIVE_ATTR_TYPE,
        OPENDRIVE_ATTR_LEVEL,
        OPENDRIVE_ATTR_ORIENTATION,
        OPENDRIVE_ATTR_DYNAMIC,
        OPENDRIVE_ATTR_INCOMINGROAD,
        OPENDRIVE_ATTR_CONNECTINGROAD,
        OPENDRIVE_ATTR_FROM,
        OPENDRIVE_ATTR_TO,
        OPENDRIVE_ATTR_MAX
    };


    /** @brief OpenDrive link type enumeration
     */
    enum LinkType {
        OPENDRIVE_LT_SUCCESSOR,
        OPENDRIVE_LT_PREDECESSOR
    };


    /** @brief OpenDrive element type enumeration
     */
    enum ElementType {
        OPENDRIVE_ET_UNKNOWN,
        OPENDRIVE_ET_ROAD,
        OPENDRIVE_ET_JUNCTION
    };


    /** @brief OpenDrive contact type enumeration
     */
    enum ContactPoint {
        OPENDRIVE_CP_UNKNOWN,
        OPENDRIVE_CP_START,
        OPENDRIVE_CP_END
    };

    /** @brief OpenDrive geometry type enumeration
     */
    enum GeometryType {
        OPENDRIVE_GT_UNKNOWN,
        OPENDRIVE_GT_LINE,
        OPENDRIVE_GT_SPIRAL,
        OPENDRIVE_GT_ARC,
        OPENDRIVE_GT_POLY3
    };



    /**
     * @struct OpenDriveLink
     * @brief Representation of an OpenDrive link
     */
    struct OpenDriveLink {
        /** @brief Constructor
         * @param[in] linkTypeArg The link type
         * @param[in] elementIDArg The element id
         */
        OpenDriveLink(LinkType linkTypeArg, const std::string& elementIDArg)
            : linkType(linkTypeArg), elementID(elementIDArg),
              elementType(OPENDRIVE_ET_UNKNOWN), contactPoint(OPENDRIVE_CP_UNKNOWN) { }

        LinkType linkType;
        std::string elementID;
        ElementType elementType;
        ContactPoint contactPoint;
    };


    /**
     * @struct OpenDriveGeometry
     * @brief Representation of an OpenDrive geometry part
     */
    struct OpenDriveGeometry {
        /** @brief Constructor
         * @param[in] lengthArg The length of this geometry part
         * @param[in] sArg The offset from the start, counted from the begin
         * @param[in] xArg x-position at this part's begin
         * @param[in] yArg y-position at this part's begin
         * @param[in] hdgArg heading at this part's begin
         */
        OpenDriveGeometry(SUMOReal lengthArg, SUMOReal sArg, SUMOReal xArg, SUMOReal yArg, SUMOReal hdgArg)
            : length(lengthArg), s(sArg), x(xArg), y(yArg), hdg(hdgArg),
              type(OPENDRIVE_GT_UNKNOWN) { }

        SUMOReal length;
        SUMOReal s;
        SUMOReal x;
        SUMOReal y;
        SUMOReal hdg;
        GeometryType type;
        std::vector<SUMOReal> params;
    };


    /**
     * @struct OpenDriveLane
     * @brief Representation of a lane
     */
    struct OpenDriveLane {
        /** @brief Constructor
         * @param[in] idArg The OpenDrive id of the lane
         * @param[in] levelArg The level
         * @param[in] typeArg type of the lane
         */
        OpenDriveLane(int idArg, const std::string& levelArg, const std::string& typeArg)
            : id(idArg), level(levelArg), type(typeArg), successor(UNSET_CONNECTION), predecessor(UNSET_CONNECTION),
              speed(0), width(0) { }

        int id;
        std::string level;
        std::string type;
        int successor;
        int predecessor;
        SUMOReal speed;
        SUMOReal width; ///< @todo: this is the maximum width only
    };



    /**
     * @struct OpenDriveLaneSection
     * @brief Representation of a lane section
     */
    struct OpenDriveLaneSection {
        /** @brief Constructor
         * @param[in] sArg The offset from the start, counted from the begin
         */
        OpenDriveLaneSection(SUMOReal sArg);


        /** @brief Returns the number of lanes for the given direction
         * @return The named direction's lane number
         */
        unsigned int getLaneNumber(OpenDriveXMLTag dir) const;


        /** @brief Build the mapping from OpenDrive to SUMO lanes
         *
         * Not all lanes are converted to SUMO-lanes; the mapping includes only those
         * which are included in the SUMO network.
         */
        void buildLaneMapping();


        std::map<int, int> getInnerConnections(OpenDriveXMLTag dir, const OpenDriveLaneSection& prev);


        /// @brief The starting offset of this lane section
        SUMOReal s;
        /// @brief A mapping from OpenDrive to SUMO-index (the first is signed, the second unsigned)
        std::map<int, int> laneMap;
        /// @brief The lanes, sorted by their direction
        std::map<OpenDriveXMLTag, std::vector<OpenDriveLane> > lanesByDir;
        /// @brief The id (generic, without the optionally leading '-') of the edge generated for this section
        std::string sumoID;
    };



    /**
     * @struct OpenDriveSignal
     * @brief Representation of a signal
     */
    struct OpenDriveSignal {
        /** @brief Constructor
         * @param[in] idArg The OpenDrive id of the signal
         * @param[in] typeArg The type of the signal
         * @param[in] orientationArg The direction the signal belongs to
         * @param[in] dynamicArg Whether the signal is dynamic
         * @param[in] sArg The offset from the start, counted from the begin
         */
        OpenDriveSignal(int idArg, const std::string typeArg, int orientationArg, bool dynamicArg, SUMOReal sArg)
            : id(idArg), type(typeArg), orientation(orientationArg), dynamic(dynamicArg), s(sArg) { }

        int id;
        std::string type;
        int orientation;
        bool dynamic;
        SUMOReal s;
    };


    /**
     * @struct Connection
     * @brief A connection between two roads
     */
    struct Connection {
        std::string fromEdge;
        std::string toEdge;
        int fromLane;
        int toLane;
        ContactPoint fromCP;
        ContactPoint toCP;
        bool all;
        std::string origID;
        int origLane;
    };


    /**
     * @struct OpenDriveEdge
     * @brief Representation of an openDrive "link"
     */
    struct OpenDriveEdge {
        OpenDriveEdge(const std::string& idArg, const std::string& junctionArg, SUMOReal lengthArg)
            : id(idArg), junction(junctionArg), length(lengthArg),
              from(0), to(0) {
            isInner = junction != "" && junction != "-1";
        }

        unsigned int getMaxLaneNumber(OpenDriveXMLTag dir) const;
        int getPriority(OpenDriveXMLTag dir) const;

        /// @brief The id of the edge
        std::string id;
        /// @brief The id of the junction the edge belongs to
        std::string junction;
        /// @brief The length of the edge
        SUMOReal length;
        std::vector<OpenDriveLink> links;
        std::vector<OpenDriveGeometry> geometries;
        NBNode* from;
        NBNode* to;
        PositionVector geom;
        std::vector<OpenDriveLaneSection> laneSections;
        std::vector<OpenDriveSignal> signals;
        std::set<Connection> connections;
        bool isInner;
    };



protected:
    /** @brief Constructor
     * @param[in] nc The node control to fill
     */
    NIImporter_OpenDrive(std::map<std::string, OpenDriveEdge*>& edges);


    /// @brief Destructor
    ~NIImporter_OpenDrive();



    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * In dependence to the obtained type, an appropriate parsing
     *  method is called ("addEdge" if an edge encounters, f.e.).
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element);
    //@}



private:
    void addLink(LinkType lt, const std::string& elementType, const std::string& elementID,
                 const std::string& contactPoint);
    void addGeometryShape(GeometryType type, const std::vector<SUMOReal>& vals);
    static void setEdgeLinks2(OpenDriveEdge& e, const std::map<std::string, OpenDriveEdge*>& edges);
    static void buildConnectionsToOuter(const Connection& c, const std::map<std::string, OpenDriveEdge*>& innerEdges, std::vector<Connection>& into);
    friend bool operator<(const Connection& c1, const Connection& c2);
    static std::string revertID(const std::string& id);
    OpenDriveEdge myCurrentEdge;

    std::map<std::string, OpenDriveEdge*>& myEdges;
    std::vector<int> myElementStack;
    OpenDriveXMLTag myCurrentLaneDirection;
    std::string myCurrentJunctionID;
    std::string myCurrentIncomingRoad;
    std::string myCurrentConnectingRoad;
    ContactPoint myCurrentContactPoint;
    bool myConnectionWasEmpty;

    static std::set<std::string> myLaneTypes2Import;
    static bool myImportAllTypes;
    static bool myImportWidths;


protected:
    /** @brief Builds a node or returns the already built
     *
     * If the node is already known, it is returned. Otherwise, the
     *  node is built. If the newly built node can not be added to
     *  the container, a ProcessError is thrown.
     *  Otherwise this node is returned.
     *
     * @param[in] id The id of the node to build/get
     * @param[in, changed] pos The position of the node to build/get
     * @param[filled] nc The node container to retrieve/add the node to
     * @return The retrieved/built node
     * @exception ProcessError If the node could not be built/retrieved
     */
    static NBNode* getOrBuildNode(const std::string& id, const Position& pos, NBNodeCont& nc);


    static std::vector<Position> geomFromLine(const OpenDriveEdge& e, const OpenDriveGeometry& g);
    static std::vector<Position> geomFromSpiral(const OpenDriveEdge& e, const OpenDriveGeometry& g);
    static std::vector<Position> geomFromArc(const OpenDriveEdge& e, const OpenDriveGeometry& g);
    static std::vector<Position> geomFromPoly(const OpenDriveEdge& e, const OpenDriveGeometry& g);
    static Position calculateStraightEndPoint(double hdg, double length, const Position& start);
    static void calculateCurveCenter(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_radius, SUMOReal ad_hdg);
    static void calcPointOnCurve(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_centerX, SUMOReal ad_centerY,
                                 SUMOReal ad_r, SUMOReal ad_length);

    static void computeShapes(std::map<std::string, OpenDriveEdge*>& edges);
    static void setNodeSecure(NBNodeCont& nc, OpenDriveEdge& e,
                              const std::string& nodeID, NIImporter_OpenDrive::LinkType lt);




    /// The names of openDrive-XML elements (for passing to GenericSAXHandler)
    static StringBijection<int>::Entry openDriveTags[];

    /// The names of openDrive-XML attributes (for passing to GenericSAXHandler)
    static StringBijection<int>::Entry openDriveAttrs[];



};


#endif

/****************************************************************************/

