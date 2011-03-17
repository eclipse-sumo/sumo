/****************************************************************************/
/// @file    NIImporter_SUMO.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in SUMO format
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
     * If the option "sumo-net" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "sumo-net" is not set, this method simply returns.
     *
     * The loading is done by parsing the network definition as an XML file
     *  using the SAXinterface and handling the incoming data via this class'
     *  methods.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont &oc, NBNetBuilder &nb);


protected:
    /** @brief Constructor
     * @param[in] nc The node control to fill
     * @param[in] nc The trafficLight control to fill
     */
    NIImporter_SUMO(NBNodeCont &nc, NBTrafficLightLogicCont &tllc);


    /// @brief Destructor
    ~NIImporter_SUMO() throw();



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
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    void myCharacters(SumoXMLTag element,
                      const std::string &chars) throw(ProcessError);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}


private:
    /// @name Object instance parsing methods
    //@{

    /** @brief Parses an edge and stores the values in "myCurrentEdge"
     * @param[in] attrs The attributes to get the edge's values from
     */
    void addEdge(const SUMOSAXAttributes &attrs);


    /** @brief Parses a lane and stores the values in "myCurrentLane"
     * @param[in] attrs The attributes to get the lane's values from
     */
    void addLane(const SUMOSAXAttributes &attrs);


    /** @brief Parses a junction and saves it in the node control
     * @param[in] attrs The attributes to get the junction's values from
     */
    void addJunction(const SUMOSAXAttributes &attrs);


    /** @brief Parses a succedge-definition and saves it by assigning "myCurrentEdge" and "myCurrentLane" to the read values
     * @param[in] attrs The attributes to get the succedge-definition from
     * @todo Recheck: should the junction be read? What is this attribute for?
     */
    void addSuccEdge(const SUMOSAXAttributes &attrs);


    /** @brief Parses a succlane-definition and saves it into the lane's definition stored in "myCurrentLane"
     * @param[in] attrs The attributes to get the succlane-definition from
     */
    void addSuccLane(const SUMOSAXAttributes &attrs);

    /// begins the reading of a traffic lights logic
    void initTrafficLightLogic(const SUMOSAXAttributes &attrs);

    /// adds a phase to the traffic lights logic currently build
    void addPhase(const SUMOSAXAttributes &attrs);
    //@}


private:
    /**
     * @struct Connection
     * @brief A connection description. 
     */
    struct Connection {
        /// @brief The connected lane
        std::string lane;
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
        /// @brief Whether this is the edge's depart lane
        bool depart;
        /// @brief The maximum velocity allowed on this lane
        SUMOReal maxSpeed;
        /// @brief This lane's shape
        Position2DVector shape;
        /// @brief This lane's connections
        std::vector<Connection> connections;
    };


    /** @struct EdgeAttrs
     * @brief Describes the values found in an edge's definition and this edge's lanes
     */
    struct EdgeAttrs {
        /// @brief This edge's id
        std::string id;
        /// @brief This edge's type
        std::string type;
        /// @brief This edge's function
        std::string func;
        /// @brief The node this edge starts at
        std::string fromNode;
        /// @brief The node this edge ends at
        std::string toNode;
        /// @brief This edge's priority
        int priority;
        /// @brief The maximum velocity allowed on this edge (!!!)
        SUMOReal maxSpeed;
        /// @brief This edge's lanes
        std::vector<LaneAttrs*> lanes;
        /// @brief The built edge
        NBEdge *builtEdge;
    };


    /// @brief Loaded edge definitions
    std::map<std::string, EdgeAttrs*> myEdges;

    /// @brief The node container to fill
    NBNodeCont &myNodeCont;

    /// @brief The node container to fill
    NBTrafficLightLogicCont &myTLLCont;

    /// @brief The currently parsed edge's definition (to add loaded lanes to)
    EdgeAttrs *myCurrentEdge;

    /// @brief The currently parsed lanes's definition (to add the shape to)
    LaneAttrs *myCurrentLane;

    /// @brief The currently parsed traffic light
    NBLoadedSUMOTLDef *myCurrentTL;

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
    static void interpretLaneID(const std::string &lane_id, std::string &edge_id, size_t &index); 
};


#endif

/****************************************************************************/

