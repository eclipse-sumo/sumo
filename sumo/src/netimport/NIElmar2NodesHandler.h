/****************************************************************************/
/// @file    NIElmar2NodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id$
///
// Importer of nodes stored in unsplit elmar format
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
#ifndef NIElmar2NodesHandler_h
#define NIElmar2NodesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/importio/LineHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIElmar2NodesHandler
 * @brief Importer of nodes stored in unsplit elmar format
 *
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Elmar's unsplit format.
 */
class NIElmar2NodesHandler : public LineHandler {
public:
    /** @brief Constructor
     * @param[in, filled] nc The node control to insert loaded nodes into
     * @param[in] file The name of the parsed file
     * @param[in, geoms] geoms Storage for read edge geometries
     */
    NIElmar2NodesHandler(NBNodeCont &nc, const std::string &file,
                         std::map<std::string, Position2DVector> &geoms) throw();


    /// @brief Destructor
    ~NIElmar2NodesHandler() throw();


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
    NIElmar2NodesHandler(const NIElmar2NodesHandler&);

    /// @brief Invalidated assignment operator.
    NIElmar2NodesHandler& operator=(const NIElmar2NodesHandler&);

};


#endif

/****************************************************************************/

