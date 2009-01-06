/****************************************************************************/
/// @file    NIElmar2EdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id:NIElmar2EdgesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer of edges stored in unsplit elmar format
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
#ifndef NIElmar2EdgesHandler_h
#define NIElmar2EdgesHandler_h


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
 * @class NIElmar2EdgesHandler
 * @brief Importer of edges stored in unsplit elmar format
 *
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain edge definitions
 * in Elmar's unsplit format.
 */
class NIElmar2EdgesHandler : public LineHandler
{
public:
    /** @brief Constructor
     * @param[in] nc The node control to retrieve nodes from
     * @param[in, filled] ec The edge control to insert loaded edges into
     * @param[in] file The name of the parsed file
     * @param[in] geoms The previously read edge geometries
     * @param[in] tryIgnoreNodePositions Whether node positions shall not be added to the geometry
     */
    NIElmar2EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                         const std::string &file, std::map<std::string,
                         Position2DVector> &geoms, bool tryIgnoreNodePositions) throw();


    /// @brief Destructor
    ~NIElmar2EdgesHandler() throw();


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
    NIElmar2EdgesHandler(const NIElmar2EdgesHandler&);

    /// @brief Invalidated assignment operator.
    NIElmar2EdgesHandler& operator=(const NIElmar2EdgesHandler&);

};


#endif

/****************************************************************************/

