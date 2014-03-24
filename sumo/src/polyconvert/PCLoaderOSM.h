/****************************************************************************/
/// @file    PCLoaderOSM.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Wed, 19.11.2008
/// @version $Id$
///
// A reader of pois and polygons stored in OSM-format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PCLoaderOSM_h
#define PCLoaderOSM_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "PCPolyContainer.h"
#include "PCTypeMap.h"
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCLoaderOSM
 * @brief A reader of pois and polygons stored in OSM-format
 *
 * Reads pois stored as XML definition as given by the OpenStreetMap-API.
 */
class PCLoaderOSM : public SUMOSAXHandler {
public:
    /** @brief Loads pois/polygons assumed to be stored as OSM-XML
     *
     * If the option "osm-files" is set within the given options container,
     *  an instance of PCLoaderOSM is built and used as a handler for the
     *  files given in this option.
     *
     * @param[in] oc The options container to get further options from
     * @param[in] toFill The poly/pois container to add loaded polys/pois to
     * @param[in] tm The type map to use for setting values of loaded polys/pois
     * @exception ProcessError if something fails
     */
    static void loadIfSet(OptionsCont& oc, PCPolyContainer& toFill,
                          PCTypeMap& tm);


protected:
    /** @brief An internal representation of an OSM-node
     */
    struct PCOSMNode {
        /// @brief The node's id
        SUMOLong id;
        /// @brief The longitude the node is located at
        SUMOReal lon;
        /// @brief The latitude the node is located at
        SUMOReal lat;
        /// @brief Additional attributes
        std::map<std::string, std::string> myAttributes;
    };


    /** @brief An internal definition of a loaded edge
     */
    struct PCOSMEdge {
        /// @brief The edge's id
        std::string id;
        /// @brief The edge's name (if any)
        std::string name;
        /// @brief Information whether this area is closed
        bool myIsClosed;
        /// @brief The list of nodes this edge is made of
        std::vector<SUMOLong> myCurrentNodes;
        /// @brief Additional attributes
        std::map<std::string, std::string> myAttributes;
    };

protected:
    /// @brief try add the polygon and return the next index on success
    static int addPolygon(const PCOSMEdge* edge, const PositionVector& vec, const PCTypeMap::TypeDef& def,
                          const std::string& fullType, int index, bool useName, PCPolyContainer& toFill, bool ignorePruning, bool withAttributes);

    /// @brief try add the POI and return the next index on success
    static int addPOI(const PCOSMNode* node, const Position& pos, const PCTypeMap::TypeDef& def,
                      const std::string& fullType, int index, PCPolyContainer& toFill, bool ignorePruning, bool withAttributes);


protected:
    static const std::set<std::string> MyKeysToInclude;

private:
    static std::set<std::string> initMyKeysToInclude();


protected:
    /**
     * @class NodesHandler
     * @brief A class which extracts OSM-nodes from a parsed OSM-file
     */
    class NodesHandler : public SUMOSAXHandler {
    public:
        /** @brief Contructor
         * @param[in] toFill The nodes container to fill
         * @param[in] withAttributes Whether all attributes shall be stored
         * @param[in] errorHandler The handler to report errors to (WarningHandler for ignoring errors)
         */
        NodesHandler(std::map<SUMOLong, PCOSMNode*>& toFill, bool withAttributes,
                     MsgHandler& errorHandler);


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
        /// @brief Whether all attributes shall be stored
        bool myWithAttributes;

        /// @brief The handler to report errors to (will be the WarningsHandler if --ignore-errors was set)
        MsgHandler& myErrorHandler;

        /// @brief The nodes container to fill
        std::map<SUMOLong, PCOSMNode*>& myToFill;

        /// @brief Current path in order to know to what occuring values belong
        std::vector<int> myParentElements;

        /// @brief The id of the last parsed node
        SUMOLong myLastNodeID;

    private:
        /// @brief Invalidated copy constructor
        NodesHandler(const NodesHandler& s);

        /// @brief Invalidated assignment operator
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
         * @param[in] toFill The edges container to fill with read edges
         * @param[in] withAttributes Whether all attributes shall be stored
         * @param[in] errorHandler The handler to report errors to (WarningHandler for ignoring errors)
         */
        EdgesHandler(const std::map<SUMOLong, PCOSMNode*>& osmNodes,
                     std::map<std::string, PCOSMEdge*>& toFill, bool withAttributes,
                     MsgHandler& errorHandler);


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
        /// @brief Whether all attributes shall be stored
        bool myWithAttributes;

        /// @brief The handler to report errors to (will be the WarningsHandler if --ignore-errors was set)
        MsgHandler& myErrorHandler;

        /// @brief The previously parsed nodes
        const std::map<SUMOLong, PCOSMNode*>& myOSMNodes;

        /// @brief A map of built edges
        std::map<std::string, PCOSMEdge*>& myEdgeMap;

        /// @brief The currently built edge
        PCOSMEdge* myCurrentEdge;

        /// @brief Current path in order to know to what occuring values belong
        std::vector<int> myParentElements;

        /// @brief whether the last edge (way) should be kept because it had a key from the inclusion list
        bool myKeep;

    private:
        /// @brief Invalidated copy constructor
        EdgesHandler(const EdgesHandler& s);

        /// @brief Invalidated assignment operator
        EdgesHandler& operator=(const EdgesHandler& s);

    };

};


#endif

/****************************************************************************/

