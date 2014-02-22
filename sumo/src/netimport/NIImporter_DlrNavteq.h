/****************************************************************************/
/// @file    NIImporter_DlrNavteq.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in Elmar's format
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
#ifndef NIImporter_DlrNavteq_h
#define NIImporter_DlrNavteq_h


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
#include <utils/common/UtilExceptions.h>
#include <utils/importio/LineHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdgeCont;
class NBNetBuilder;
class NBNodeCont;
class NBTrafficLightLogicCont;
class NBTypeCont;
class OptionsCont;
class PositionVector;
class StringTokenizer;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_DlrNavteq
 * @brief Importer for networks stored in Elmar's format
 *
 */
class NIImporter_DlrNavteq {
public:
    /** @brief Loads content of the optionally given dlr-navteq (aka Elmar-fomat) folder
     *
     * If the option "dlr-navteq-prefix" is set, the file(s) stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "dlr-navteq-prefix" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont& oc, NBNetBuilder& nb);

    /// @brief scaling factor for geo coordinates (DLRNavteq format uses this to increase floating point precisions)
    static const int GEO_SCALE;

protected:
    /**
     * @class NodesHandler
     * @brief Importer of nodes stored in unsplit elmar format
     *
     * Being a LineHandler, this class retrieves each line from a LineReader
     * and parses these information assuming they contain node definitions
     * in DLRNavteq's unsplit format.
     */
    class NodesHandler : public LineHandler {
    public:
        /** @brief Constructor
         * @param[in, filled] nc The node control to insert loaded nodes into
         * @param[in] file The name of the parsed file
         * @param[in, geoms] geoms Storage for read edge geometries
         */
        NodesHandler(NBNodeCont& nc, const std::string& file,
                     std::map<std::string, PositionVector>& geoms);


        /// @brief Destructor
        ~NodesHandler();


        /** @brief Parsing method
         *
         * Implementation of the LineHandler-interface called by a LineReader;
         *  interprets the retrieved information and stores it into "myNodeCont".
         * Additionally, edge geometries are parsed and stored into "myGeoms".
         *
         * @param[in] result The read line
         * @return Whether the parsing shall continue
         * @exception ProcessError if something fails
         * @see LineHandler::report
         */
        bool report(const std::string& result);


    protected:
        // @brief The node container to store parsed nodes into
        NBNodeCont& myNodeCont;

        /// @brief A container for parsed geometries
        std::map<std::string, PositionVector>& myGeoms;


    private:
        /// @brief Invalidated copy constructor.
        NodesHandler(const NodesHandler&);

        /// @brief Invalidated assignment operator.
        NodesHandler& operator=(const NodesHandler&);

    };


    /**
     * @class EdgesHandler
     * @brief Importer of edges stored in unsplit elmar format
     *
     * Being a LineHandler, this class retrieves each line from a LineReader
     * and parses these information assuming they contain edge definitions
     * in DLRNavteq's unsplit format.
     */
    class EdgesHandler : public LineHandler {

    public:
        /** @brief Constructor
         * @param[in] nc The node control to retrieve nodes from
         * @param[in, filled] ec The edge control to insert loaded edges into
         * @param[in] file The name of the parsed file
         * @param[in] geoms The previously read edge geometries
         * @param[in] streetNames The previously read street names
         */
        EdgesHandler(NBNodeCont& nc, NBEdgeCont& ec,
                     const std::string& file,
                     std::map<std::string, PositionVector>& geoms,
                     std::map<std::string, std::string>& streetNames);


        /// @brief Destructor
        ~EdgesHandler();


        /** @brief Parsing method
             *
             * Implementation of the LineHandler-interface called by a LineReader;
             * interprets the retrieved information and stores it into "myEdgeCont".
             * @param[in] result The read line
             * @return Whether the parsing shall continue
             * @exception ProcessError if something fails
             * @see LineHandler::report
         */
        bool report(const std::string& result);


    protected:
        /// @brief The node container to get the referenced nodes from
        NBNodeCont& myNodeCont;

        /// @brief The edge container to store loaded edges into
        NBEdgeCont& myEdgeCont;

        /// @brief Previously read edge geometries (manipulated during use)
        std::map<std::string, PositionVector>& myGeoms;

        /// @brief Previously read streat names (non-const because operate[] is more convenient)
        std::map<std::string, std::string>& myStreetNames;

        /// @brief Whether node positions shall not be added to the edge's geometry
        bool myTryIgnoreNodePositions;

        /// @brief version number of current file
        SUMOReal myVersion;

        /// @brief the version number of the edge file being parsed
        std::vector<int> myColumns;

        /// @brief the file being parsed
        const std::string myFile;

        static const int MISSING_COLUMN;

        enum ColumnName {
            LINK_ID = 0,
            NODE_ID_FROM,
            NODE_ID_TO,
            BETWEEN_NODE_ID,
            LENGTH,
            VEHICLE_TYPE,
            FORM_OF_WAY,
            BRUNNEL_TYPE,
            FUNCTIONAL_ROAD_CLASS,
            SPEED_CATEGORY,
            NUMBER_OF_LANES,
            SPEED_LIMIT,
            SPEED_RESTRICTION,
            NAME_ID1_REGIONAL,
            NAME_ID2_LOCAL,
            HOUSENUMBERS_RIGHT,
            HOUSENUMBERS_LEFT,
            ZIP_CODE,
            AREA_ID,
            SUBAREA_ID,
            THROUGH_TRAFFIC,
            SPECIAL_RESTRICTIONS,
            EXTENDED_NUMBER_OF_LANES,
            ISRAMP,
            CONNECTION
        };

        std::string getColumn(const StringTokenizer& st, ColumnName name, const std::string fallback = "");

    private:
        /// @brief build the street name for the given ids
        std::string getStreetNameFromIDs(const std::string& regionalID, const std::string& localID) const;


    private:
        /// @brief Invalidated copy constructor.
        EdgesHandler(const EdgesHandler&);

        /// @brief Invalidated assignment operator.
        EdgesHandler& operator=(const EdgesHandler&);

    };


    /**
     * @class TrafficlightsHandler
     * @brief Importer of traffic lights stored in DLRNavteq's (aka elmar) format
     *
     * Being a LineHandler, this class retrieves each line from a LineReader
     * and parses these information assuming they contain traffic light definitions
     * in DLRNavteq's format.
     */
    class TrafficlightsHandler : public LineHandler {
    public:
        /** @brief Constructor
         * @param[in] nc The node control to retrieve nodes from
         * @param[in, filled] tlc The traffic lights container to fill
         * @param[in] file The name of the parsed file
         */
        TrafficlightsHandler(NBNodeCont& nc, NBTrafficLightLogicCont& tlc,
                             NBEdgeCont& ne, const std::string& file);


        /// @brief Destructor
        ~TrafficlightsHandler();


        /** @brief Parsing method
         *
         * Implementation of the LineHandler-interface called by a LineReader;
         * interprets the retrieved information and alters the nodes.
         * @param[in] result The read line
         * @return Whether the parsing shall continue
         * @exception ProcessError if something fails
         * @see LineHandler::report
         */
        bool report(const std::string& result);


    protected:
        /// @brief The node container to get the referenced nodes from
        NBNodeCont& myNodeCont;

        /// @brief The traffic lights container to add built tls to
        NBTrafficLightLogicCont& myTLLogicCont;

        /// @brief The edge container to get the referenced edges from
        NBEdgeCont& myEdgeCont;


    private:
        /// @brief Invalidated copy constructor.
        TrafficlightsHandler(const TrafficlightsHandler&);

        /// @brief Invalidated assignment operator.
        TrafficlightsHandler& operator=(const TrafficlightsHandler&);

    };


    /**
     * @class NamesHandler
     * @brief Importer of street names in DLRNavteq's (aka elmar) format
     *
     * Being a LineHandler, this class retrieves each line from a LineReader
     * and parses these information assuming they contain name definitions
     * in DLRNavteq's format.
     */
    class NamesHandler : public LineHandler {
    public:
        /** @brief Constructor
         * @param[in] file The name of the parsed file
         * @param[filled] streetNames output container for read names
         */
        NamesHandler(const std::string& file, std::map<std::string, std::string>& streetNames);


        /// @brief Destructor
        ~NamesHandler();


        /** @brief Parsing method
         *
         * Implementation of the LineHandler-interface called by a LineReader;
         * interprets the retrieved information and stores the streetNames
         * @param[in] result The read line
         * @return Whether the parsing shall continue
         * @exception ProcessError if something fails
         * @see LineHandler::report
         */
        bool report(const std::string& result);


    protected:
        /// @brief The container for storing read names
        std::map<std::string, std::string>& myStreetNames;


    private:
        /// @brief Invalidated copy constructor.
        NamesHandler(const NamesHandler&);

        /// @brief Invalidated assignment operator.
        NamesHandler& operator=(const NamesHandler&);

    };

};


#endif

/****************************************************************************/

