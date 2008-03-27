/****************************************************************************/
/// @file    NIXMLConnectionsHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIXMLConnectionsHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for edge connections stored in XML
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
 * @class NIArcView_Loader
 * @brief Importer for edge connections stored in XML
 */
class NIXMLConnectionsHandler : public SUMOSAXHandler
{
public:
    /// standard constructor
    NIXMLConnectionsHandler(NBEdgeCont &ec);

    /// destructor
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
    // parses a connection when it describes a edge-2-edge relationship
    void parseEdgeBound(const SUMOSAXAttributes &attrs, NBEdge *from,
                        NBEdge *to);

    // parses a connection when it describes a lane-2-lane relationship
    void parseLaneBound(const SUMOSAXAttributes &attrs,NBEdge *from,
                        NBEdge *to);

    /** @brief returns the connection described by def
     *
     * def should have the following format <FROM_EDGE_ID>[_<FROM_LANE_NO>]-><TO_EDGE_ID>[_<TO_LANE_NO>]
     */
    NBConnection parseConnection(const std::string &defRole, const std::string &def);

    /** @brief Returns the node over which the connection defined by def goes
     *
     * def should have the following format <FROM_EDGE_ID>[_<FROM_LANE_NO>]-><TO_EDGE_ID>[_<TO_LANE_NO>]
     * As node the node the from-edge is incoming to is chosen
     */
    NBNode *getNode(const std::string &def);

private:
    NBEdgeCont &myEdgeCont;

private:
    /** invalid copy constructor */
    NIXMLConnectionsHandler(const NIXMLConnectionsHandler &s);

    /** invalid assignment operator */
    NIXMLConnectionsHandler &operator=(const NIXMLConnectionsHandler &s);

};


#endif

/****************************************************************************/

