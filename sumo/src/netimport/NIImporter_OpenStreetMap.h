/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id:NIImporter_OpenStreetMap.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for networks stored in OpenStreetMap format
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
 * @class NIImporter_OpenStreetMap
 * @brief Importer for networks stored in OpenStreetMap format
 *
 */
class NIImporter_OpenStreetMap
{
public:
    /** @brief Loads network definition from the assigned option and stores it in the given network builder
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
        SUMOReal lon;
        /// @brief The latitude the node is located at
        SUMOReal lat;
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
    static NBNode *insertNodeChecking(int id, const std::map<int, NIOSMNode*> &osmNodes, NBNodeCont &nc, NBTrafficLightLogicCont &tlsc) ;
    static void insertEdge(Edge *e, int index, NBNode *from, NBNode *to,
                           const std::vector<int> &passed, const std::map<int, NIOSMNode*> &osmNodes, 
                           NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc,
                           bool tryIgnoreNodePositions);
    static void addTypeSecure(NBTypeCont &tc, const std::string &mClass, const std::string &sClass,
                              int noLanes, SUMOReal maxSpeed, int prio, SUMOVehicleClass vClasses=SVC_UNKNOWN, bool oneWayIsDefault=false);





    /**
     * @class NodesHandler
     * @brief A class which extracts OSM-nodes from a parsed OSM-file
     */
class NodesHandler : public SUMOSAXHandler
    {
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


        /** @brief Called when a closing tag occures
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

        std::vector<SumoXMLTag> myParentElements;

        int myLastNodeID;

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
class EdgesHandler : public SUMOSAXHandler
    {
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


        /** @brief Called when a closing tag occures
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

        std::vector<SumoXMLTag> myParentElements;


    private:
        /** @brief invalidated copy constructor */
        EdgesHandler(const EdgesHandler &s);

        /** @brief invalidated assignment operator */
        EdgesHandler &operator=(const EdgesHandler &s);

    };


};


#endif

/****************************************************************************/

