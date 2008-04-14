/****************************************************************************/
/// @file    NIImporter_SUMO.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id:NIImporter_SUMO.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for networks stored in SUMO format
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


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class myEdgeCont;
class myNodeCont;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_SUMO
 * @brief Importer for networks stored in SUMO format
 *
 */
class NIImporter_SUMO : public SUMOSAXHandler
{
public:
    /** @brief Loads network definition from the assigned option and stores it in the given network builder
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
     *
     * @param[in] nc The node control to fill
     */
    NIImporter_SUMO(NBNodeCont &nc);


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


    /** @brief Called when a closing tag occures
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
    //@}


private:
    /** @struct LaneAttrs
     * @brief Describes the values found in a lane's definition
     */
    struct LaneAttrs {
        /// @brief Whether this is the edge's depart lane
        bool depart;
        /// @brief The vehicle classes allowed on this lane
        std::string vclasses;
        /// @brief The maximum velocity allowed on this lane
        SUMOReal maxSpeed;
        /// @brief This lane's shape
        Position2DVector shape;
    };


    /** @struct EdgeAttrs
     * @brief Describes the values found in an edge's definition and this edge's lanes
     */
    struct EdgeAttrs {
        /// @brief This edge's id
        std::string id;
        /// @brief This edge's type
        std::string type;
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
    };


    /// @brief Loaded edge definitions
    std::map<std::string, EdgeAttrs*> myEdges;

    /// @brief The node container to fill
    NBNodeCont &myNodeCont;

    /// @brief The currently parsed edge's definition (to add loaded lanes to)
    EdgeAttrs *myCurrentEdge;

    /// @brief The currently parsed lanes's definition (to add the shape to)
    LaneAttrs *myCurrentLane;

};


#endif

/****************************************************************************/

