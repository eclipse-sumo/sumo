/****************************************************************************/
/// @file    NIImporter_DlrNavteq.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id: NIImporter_DlrNavteq.h 7703 2009-09-10 13:45:09Z simsiem $
///
// Importer for networks stored in Elmar's format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
class Position2DVector;


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
     * If the option "dlr-navteq" is set, the file(s) stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "dlr-navteq" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont &oc, NBNetBuilder &nb);


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
        NodesHandler(NBNodeCont &nc, const std::string &file,
                     std::map<std::string, Position2DVector> &geoms) throw();


        /// @brief Destructor
        ~NodesHandler() throw();


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
        bool report(const std::string &result) throw(ProcessError);


    protected:
        // @brief The node container to store parsed nodes into
        NBNodeCont &myNodeCont;

        /// @brief A container for parsed geometries
        std::map<std::string, Position2DVector> &myGeoms;


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
         */
        EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                     const std::string &file, std::map<std::string, Position2DVector> &geoms) throw();


        /// @brief Destructor
        ~EdgesHandler() throw();


        /** @brief Parsing method
         *
         * Implementation of the LineHandler-interface called by a LineReader;
         * interprets the retrieved information and stores it into "myEdgeCont".
         * @param[in] result The read line
         * @return Whether the parsing shall continue
         * @exception ProcessError if something fails
         * @see LineHandler::report
         */
        bool report(const std::string &result) throw(ProcessError);


    protected:
        /// @brief The node container to get the referenced nodes from
        NBNodeCont &myNodeCont;

        /// @brief The edge container to store loaded edges into
        NBEdgeCont &myEdgeCont;

        /// @brief Previously read edge geometries
        std::map<std::string, Position2DVector> &myGeoms;

        /// @brief Whether node positions shall not be added to the edge's geometry
        bool myTryIgnoreNodePositions;


    private:
        /// @brief Invalidated copy constructor.
        EdgesHandler(const EdgesHandler&);

        /// @brief Invalidated assignment operator.
        EdgesHandler& operator=(const EdgesHandler&);

    };

};


#endif

/****************************************************************************/

