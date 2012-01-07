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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
        // !!! OPENDRIVE_TAG_NEIGHBOR,
        // !!! OPENDRIVE_TAG_TYPE,
        OPENDRIVE_TAG_GEOMETRY,
        OPENDRIVE_TAG_LINE,
        OPENDRIVE_TAG_SPIRAL,
        OPENDRIVE_TAG_ARC,
        OPENDRIVE_TAG_POLY3,
        OPENDRIVE_TAG_LANESECTION,
        OPENDRIVE_TAG_LEFT,
        OPENDRIVE_TAG_CENTER,
        OPENDRIVE_TAG_RIGHT,
        OPENDRIVE_TAG_LANE
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
        OPENDRIVE_ATTR_LEVEL
    };

    enum LinkType {
        OPENDRIVE_LT_SUCCESSOR,
        OPENDRIVE_LT_PREDECESSOR
    };

    enum ElementType {
        OPENDRIVE_ET_UNKNOWN,
        OPENDRIVE_ET_ROAD,
        OPENDRIVE_ET_JUNCTION
    };

    enum ContactPoint {
        OPENDRIVE_CP_UNKNOWN,
        OPENDRIVE_CP_START,
        OPENDRIVE_CP_END
    };

    enum GeometryType {
        OPENDRIVE_GT_UNKNOWN,
        OPENDRIVE_GT_LINE,
        OPENDRIVE_GT_SPIRAL,
        OPENDRIVE_GT_ARC,
        OPENDRIVE_GT_POLY3
    };

    /**
     * @struct OpenDriveLink
     * @brief Representation of an openDrive connection
     */
    struct OpenDriveLink {
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
     * @brief Representation of an openDrive geometry part
     */
    struct OpenDriveGeometry {
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
        OpenDriveLane(int idArg, const std::string& levelArg, const std::string& typeArg)
            : id(idArg), level(levelArg), type(typeArg), successor(UNSET_CONNECTION), predecessor(UNSET_CONNECTION) { }

        int id;
        std::string level;
        std::string type;
        int successor;
        int predecessor;
    };


    /**
     * @struct OpenDriveLaneSection
     * @brief Representation of a lane section
     */
    struct OpenDriveLaneSection {
        OpenDriveLaneSection(SUMOReal sArg)
            : s(sArg) {
            lanesByDir[OPENDRIVE_TAG_LEFT] = std::vector<OpenDriveLane>();
            lanesByDir[OPENDRIVE_TAG_RIGHT] = std::vector<OpenDriveLane>();
            lanesByDir[OPENDRIVE_TAG_CENTER] = std::vector<OpenDriveLane>();
        }

        unsigned int getLaneNumber(OpenDriveXMLTag dir) const {
            unsigned int laneNum = 0;
            const std::vector<OpenDriveLane> &dirLanes = lanesByDir.find(dir)->second;
            for (std::vector<OpenDriveLane>::const_iterator i = dirLanes.begin(); i != dirLanes.end(); ++i) {
                if ((*i).type == "driving") {
                    ++laneNum;
                }
            }
            return laneNum;
        }

        std::map<int, int> buildLaneMapping(OpenDriveXMLTag dir) {
            std::map<int, int> ret;
            unsigned int sumoLane = 0;
            const std::vector<OpenDriveLane> &dirLanes = lanesByDir.find(dir)->second;
            if (dir == OPENDRIVE_TAG_RIGHT) {
                for (std::vector<OpenDriveLane>::const_reverse_iterator i = dirLanes.rbegin(); i != dirLanes.rend(); ++i) {
                    if ((*i).type == "driving") {
                        ret[(*i).id] = sumoLane++;
                    }
                }
            } else {
                for (std::vector<OpenDriveLane>::const_iterator i = dirLanes.begin(); i != dirLanes.end(); ++i) {
                    if ((*i).type == "driving") {
                        ret[(*i).id] = sumoLane++;
                    }
                }
            }
            return ret;
        }

        SUMOReal s;
        std::map<OpenDriveXMLTag, std::vector<OpenDriveLane> > lanesByDir;
    };


    /**
     * @struct OpenDriveEdge
     * @brief Representation of an openDrive "link"
     */
    struct OpenDriveEdge {
        OpenDriveEdge(const std::string& idArg, const std::string& junctionArg, SUMOReal lengthArg)
            : id(idArg), junction(junctionArg), length(lengthArg),
              from(0), to(0) { }

        unsigned int getMaxLaneNumber(OpenDriveXMLTag dir) const {
            unsigned int maxLaneNum = 0;
            for (std::vector<OpenDriveLaneSection>::const_iterator i = laneSections.begin(); i != laneSections.end(); ++i) {
                maxLaneNum = MAX2(maxLaneNum, (*i).getLaneNumber(dir));
            }
            return maxLaneNum;
        }

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
        std::map<int, int> beginLaneMap;
        std::map<int, int> endLaneMap;
        PositionVector geom;
        std::vector<OpenDriveLaneSection> laneSections;
    };


    struct Connection {
        Connection(NBEdge* fromArg, const std::string& viaArg, NBEdge* toArg)
            : from(fromArg), to(toArg), via(viaArg) { }
        NBEdge* from;
        NBEdge* to;
        std::string via;
        std::vector<std::pair<int, int> > lanes;
    };

protected:
    /** @brief Constructor
     * @param[in] nc The node control to fill
     */
    NIImporter_OpenDrive(std::vector<OpenDriveEdge> &innerEdges, std::vector<OpenDriveEdge> &outerEdges);


    /// @brief Destructor
    ~NIImporter_OpenDrive() ;



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
                        const SUMOSAXAttributes& attrs) ;


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    void myCharacters(int element,
                      const std::string& chars) ;


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element) ;
    //@}



private:
    void addLink(LinkType lt, const std::string& elementType, const std::string& elementID,
                 const std::string& contactPoint) ;

    void addGeometryShape(GeometryType type, const std::vector<SUMOReal> &vals) ;

    OpenDriveEdge myCurrentEdge;

    std::vector<OpenDriveEdge> &myInnerEdges;
    std::vector<OpenDriveEdge> &myOuterEdges;
    std::vector<int> myElementStack;
    OpenDriveXMLTag myCurrentLaneDirection;


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
    static NBNode* getOrBuildNode(const std::string& id, Position& pos, NBNodeCont& nc) ;


    static std::vector<Position> geomFromLine(const OpenDriveEdge& e, const OpenDriveGeometry& g) ;
    static std::vector<Position> geomFromSpiral(const OpenDriveEdge& e, const OpenDriveGeometry& g) ;
    static std::vector<Position> geomFromArc(const OpenDriveEdge& e, const OpenDriveGeometry& g) ;
    static std::vector<Position> geomFromPoly(const OpenDriveEdge& e, const OpenDriveGeometry& g) ;
    static Position calculateStraightEndPoint(double hdg, double length, const Position& start) ;
    static void calculateCurveCenter(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_radius, SUMOReal ad_hdg) ;
    static void calcPointOnCurve(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_centerX, SUMOReal ad_centerY,
                                 SUMOReal ad_r, SUMOReal ad_length) ;
    static NBEdge* getOutgoingDirectionalEdge(const NBEdgeCont& ec, const NBNodeCont& nc,
            const std::string& edgeID, const std::string& nodeID) ;
    static NBEdge* getIncomingDirectionalEdge(const NBEdgeCont& ec, const NBNodeCont& nc,
            const std::string& edgeID, const std::string& nodeID) ;

    static void computeShapes(std::vector<OpenDriveEdge> &edges) ;
    static void setNodeSecure(NBNodeCont& nc, OpenDriveEdge& e,
                              const std::string& nodeID, NIImporter_OpenDrive::LinkType lt) ;

    static void addE2EConnectionsSecure(const NBEdgeCont& ec, const NBNode* const node,
                                        const OpenDriveEdge& from, const OpenDriveEdge& to,
                                        std::vector<NIImporter_OpenDrive::Connection> &connections);
    static void addViaConnectionSecure(const NBEdgeCont& ec, const NBNode* const node, const OpenDriveEdge& e,
                                       LinkType lt, const std::string& via,
                                       std::vector<NIImporter_OpenDrive::Connection> &connections);

    static void setLaneConnections(NIImporter_OpenDrive::Connection& c,
                                   const OpenDriveEdge& from, bool fromAtBegin, OpenDriveXMLTag fromLaneDir,
                                   const OpenDriveEdge& to, bool toAtEnd, OpenDriveXMLTag toLaneDir);

    static void setLaneConnections(NIImporter_OpenDrive::Connection& c,
                                   const OpenDriveEdge& from, bool fromAtBegin, OpenDriveXMLTag fromLaneDir,
                                   const OpenDriveEdge& via, bool viaIsReversed, OpenDriveXMLTag viaLaneDir,
                                   const OpenDriveEdge& to, bool fromAtEnd, OpenDriveXMLTag toLaneDir);


    class edge_by_id_finder {
    public:
        explicit edge_by_id_finder(const std::string& id)
            : myEdgeID(id) { }

        bool operator()(const OpenDriveEdge& e) {
            return e.id == myEdgeID;
        }

    private:
        const std::string& myEdgeID;

    };



    /// The names of openDrive-XML elements (for passing to GenericSAXHandler)
    static StringBijection<int>::Entry openDriveTags[];

    /// The names of openDrive-XML attributes (for passing to GenericSAXHandler)
    static StringBijection<int>::Entry openDriveAttrs[];



};


#endif

/****************************************************************************/

