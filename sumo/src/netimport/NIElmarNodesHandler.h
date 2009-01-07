/****************************************************************************/
/// @file    NIElmarNodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id$
///
// Importer of nodes stored in split elmar format
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
#ifndef NIElmarNodesHandler_h
#define NIElmarNodesHandler_h


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
 * @class NIElmarNodesHandler
 * @brief Importer of nodes stored in split elmar format
 *
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Elmar's split format
 */
class NIElmarNodesHandler : public LineHandler
{
public:
    /** @brief Constructor
     * @param[in, filled] nc The node control to insert loaded nodes into
     * @param[in] file The name of the parsed file
     */
    NIElmarNodesHandler(NBNodeCont &nc, const std::string &file) throw();


    /// @brief Destructor
    ~NIElmarNodesHandler() throw();


    /** @brief Parsing method
     *
     * Implementation of the LineHandler-interface called by a LineReader;
     * interprets the retrieved information and stores it into "myNodeCont".
     * @param[in] result The read line
     * @return Whether the parsing shall continue
     * @exception ProcessError if something fails
     * @see LineHandler::report
     */
    bool report(const std::string &result) throw(ProcessError);


protected:
    /// @brief The node container to fill
    NBNodeCont &myNodeCont;


};


#endif

/****************************************************************************/

