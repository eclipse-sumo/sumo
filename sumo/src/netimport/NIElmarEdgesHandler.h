/****************************************************************************/
/// @file    NIElmarEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id:NIElmarEdgesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer of edges stored in split elmar format
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
#ifndef NIElmarEdgesHandler_h
#define NIElmarEdgesHandler_h


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
 * @class NIElmarEdgesHandler
 * @brief Importer of edges stored in split elmar format
 *
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain edge definitions
 * in Elmar's split format
 */
class NIElmarEdgesHandler : public LineHandler
{
public:
    /** @brief Constructor
     * @param[in] nc The node control to retrieve nodes from
     * @param[in, filled] ec The edge control to insert loaded edges into
     * @param[in] file The name of the parsed file
     */
    NIElmarEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                        const std::string &file) throw();

    /// @brief Destructor
    ~NIElmarEdgesHandler() throw();


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


private:
    /// @brief Invalidated copy constructor.
    NIElmarEdgesHandler(const NIElmarEdgesHandler&);

    /// @brief Invalidated assignment operator.
    NIElmarEdgesHandler& operator=(const NIElmarEdgesHandler&);

};


#endif

/****************************************************************************/

