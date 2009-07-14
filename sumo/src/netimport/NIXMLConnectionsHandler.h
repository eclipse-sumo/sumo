/****************************************************************************/
/// @file    NIXMLConnectionsHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for edge connections stored in XML
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
#ifndef NIXMLConnectionsHandler_h
#define NIXMLConnectionsHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBConnection.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIXMLConnectionsHandler
 * @brief Importer for edge connections stored in XML
 *
 * This importer parses connections, and prohibitions, and is able
 *  to reset connections between edges.
 */
class NIXMLConnectionsHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     * @param[in] ec The edge container which includes the edges to change connections of
     */
    NIXMLConnectionsHandler(NBEdgeCont &ec) throw();


    /// @brief Destructor
    ~NIXMLConnectionsHandler() throw();


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
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}

private:
    /** @brief Returns the connection described by def
     *
     * def should have the following format <FROM_EDGE_ID>[_<FROM_LANE_NO>]-><TO_EDGE_ID>[_<TO_LANE_NO>].
     *
     * @param[in] defRole "prohibitor" or "prohibited" - used for error reporting
     * @param[in] def The definition of the connection
     * @return The parsed connection
     */
    NBConnection parseConnection(const std::string &defRole, const std::string &def) throw();


    /** @brief Parses a connection when it describes a lane-2-lane relationship
     * @param[in] attrs The attributes to get further information about the connection from
     * @param[in] from The edge at which the connection starts (the on incoming into a node)
     * @param[in] to The edge at which the connection ends (the on outgoing from a node)
     */
    void parseLaneBound(const SUMOSAXAttributes &attrs,NBEdge *from,
                        NBEdge *to) throw();


private:
    /// @brief Whether an information about an occured, deprecated attributes has bee printed
    bool myHaveReportedAboutFunctionDeprecation;

    /// @brief The edge container to fill
    NBEdgeCont &myEdgeCont;


private:
    /// @brief invalidated copy constructor
    NIXMLConnectionsHandler(const NIXMLConnectionsHandler &s);

    /// @brief invalidated assignment operator
    NIXMLConnectionsHandler &operator=(const NIXMLConnectionsHandler &s);


};


#endif

/****************************************************************************/

