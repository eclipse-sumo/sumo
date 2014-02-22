/****************************************************************************/
/// @file    NIImporter_SUMO.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in SUMO format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIImporter_SUMO_h
#define NIImporter_SUMO_h


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
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBLoadedSUMOTLDef.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_SUMO
 * @brief Importer for networks stored in SUMO format
 *
 */
class NIImporter_SUMO : public SUMOSAXHandler {
public:
    /** @brief Loads content of the optionally given SUMO file
     *
     * If the option "sumo-net-file" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "sumo-net-file" is not set, this method simply returns.
     *
     * The loading is done by parsing the network definition as an XML file
     *  using the SAXinterface and handling the incoming data via this class'
     *  methods.
     *
     * @param[in,out] oc The options to use (option no-internal-links may be modified)
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(OptionsCont& oc, NBNetBuilder& nb);

    /// begins the reading of a traffic lights logic
    static NBLoadedSUMOTLDef* initTrafficLightLogic(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL);

    /// adds a phase to the traffic lights logic currently build
    static void addPhase(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL);

    /// Parses network location description and registers it with GeoConveHelper::setLoaded
    static GeoConvHelper* loadLocation(const SUMOSAXAttributes& attrs);

protected:
    /** @brief Constructor
     * @param[in] nc The network builder to fill
     */
    NIImporter_SUMO(NBNetBuilder& nb);


    /// @brief Destructor
    ~NIImporter_SUMO();



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
    /// @brief load the network
    void _loadNetwork(OptionsCont& oc);

    /// @name Object instance parsing methods
    //@{

    /** @brief Parses an edge and stores the values in "myCurrentEdge"
     * @param[in] attrs The attributes to get the edge's values from
     */
    void addEdge(const SUMOSAXAttributes& attrs);


    /** @brief Parses a lane and stores the values in "myCurrentLane"
     * @param[in] attrs The attributes to get the lane's values from
     */
    void addLane(const SUMOSAXAttributes& attrs);


    /** @brief Parses a junction and saves it in the node control
     * @param[in] attrs The attributes to get the junction's values from
     */
    void addJunction(const SUMOSAXAttributes& attrs);


    /** @brief Parses a connection and saves it
     *    into the lane's definition stored in "myCurrentLane"
     * @param[in] attrs The attributes to get the connection from
     */
    void addConnection(const SUMOSAXAttributes& attrs);

    /** @brief Parses a prohibition and saves it
     * @param[in] attrs The attributes to get the connection from
     */
    void addProhibition(const SUMOSAXAttributes& attrs);

    //@}



private:
    /**
     * @struct Connection
     * @brief A connection description.
     */
    struct Connection {
        /// @brief The id of the target edge
        std::string toEdgeID;
        /// @brief The index of the target lane
        unsigned int toLaneIdx;
        /// @brief The id of the traffic light that controls this connection
        std::string tlID;
        /// @brief The index of this connection within the controlling traffic light
        unsigned int tlLinkNo;
        /// @brief Information about being definitely free to drive (on-ramps)
        bool mayDefinitelyPass;
    };


    /** @struct LaneAttrs
     * @brief Describes the values found in a lane's definition
     */
    struct LaneAttrs {
        /// @brief The maximum velocity allowed on this lane
        SUMOReal maxSpeed;
        /// @brief This lane's shape (needed to reconstruct edge shape for legacy networks)
        PositionVector shape;
        /// @brief This lane's connections
        std::vector<Connection> connections;
        /// @brief This lane's allowed vehicle classes
        std::string allow;
        /// @brief This lane's disallowed vehicle classes
        std::string disallow;
        /// @brief The width of this lane
        SUMOReal width;
        /// @brief This lane's offset from the intersection
        SUMOReal offset;
    };


    /** @struct EdgeAttrs
     * @brief Describes the values found in an edge's definition and this edge's lanes
     */
    struct EdgeAttrs {
        /// @brief This edge's id
        std::string id;
        /// @brief This edge's street name
        std::string streetName;
        /// @brief This edge's type
        std::string type;
        /// @brief This edge's function
        SumoXMLEdgeFunc func;
        /// @brief The node this edge starts at
        std::string fromNode;
        /// @brief The node this edge ends at
        std::string toNode;
        /// @brief This edges's shape
        PositionVector shape;
        /// @brief The length of the edge if set explicitly
        SUMOReal length;
        /// @brief This edge's priority
        int priority;
        /// @brief The maximum velocity allowed on this edge (!!!)
        SUMOReal maxSpeed;
        /// @brief This edge's lanes
        std::vector<LaneAttrs*> lanes;
        /// @brief The built edge
        NBEdge* builtEdge;
        /// @brief The lane spread function
        LaneSpreadFunction lsf;
    };


    /** @struct Prohibition
     * @brief Describes the values found in a prohibition
     */
    struct Prohibition {
        std::string prohibitorFrom;
        std::string prohibitorTo;
        std::string prohibitedFrom;
        std::string prohibitedTo;
    };


    /// @brief Loaded edge definitions
    std::map<std::string, EdgeAttrs*> myEdges;

    /// @brief Loaded prohibitions
    std::vector<Prohibition> myProhibitions;

    /// @brief The network builder to fill
    NBNetBuilder& myNetBuilder;

    /// @brief The node container to fill
    NBNodeCont& myNodeCont;

    /// @brief The node container to fill
    NBTrafficLightLogicCont& myTLLCont;

    /// @brief The currently parsed edge's definition (to add loaded lanes to)
    EdgeAttrs* myCurrentEdge;

    /// @brief The currently parsed lanes's definition (to add the shape to)
    LaneAttrs* myCurrentLane;

    /// @brief The currently parsed traffic light
    NBLoadedSUMOTLDef* myCurrentTL;

    /// @brief The coordinate transformation which was used to build the loaded network.
    GeoConvHelper* myLocation;

    /// @brief whether we suspect a net that was built with xml.keep-shape
    bool mySuspectKeepShape;

    /// @brief whether the loaded network contains internal lanes
    bool myHaveSeenInternalEdge;

    /** @brief Parses lane index from lane ID an retrieve lane from EdgeAttrs
     * @param[in] edge The EdgeAttrs* which should contain the lane
     * @param[in] lane_id The ID of the lane
     */
    LaneAttrs* getLaneAttrsFromID(EdgeAttrs* edge, std::string lane_id);

    /** @brief parses edge-id and index from lane-id
     * @param[in] lane_id The lane-id
     * @param[out] edge_id ID of this lane's edge
     * @param[out] index Index of this lane
     */
    static void interpretLaneID(const std::string& lane_id, std::string& edge_id, unsigned int& index);

    /** @brief reconstructs the edge shape from the node positions and the given lane shapes
     * since we do not know the original LaneSpreadFunction this is only an
     * approximation
     * @param[in] lanes The list of lane attributes
     */
    static PositionVector reconstructEdgeShape(const EdgeAttrs* edge, const Position& from, const Position& to);

    /// @brief read position from the given attributes, attribute errors to id
    static Position readPosition(const SUMOSAXAttributes& attrs, const std::string& id, bool& ok);

    /** @brief parses connection string of a prohibition (very old school)
     * @param[in] attr The connection attribute
     * @param[out] from ID of the source edge
     * @param[out] to ID of the destination edge
     * @param[out] ok Whether parsing completed successfully
     */
    void parseProhibitionConnection(const std::string& attr, std::string& from, std::string& to, bool& ok);
};


#endif

/****************************************************************************/

