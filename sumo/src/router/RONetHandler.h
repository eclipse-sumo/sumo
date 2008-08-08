/****************************************************************************/
/// @file    RONetHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The handler that parses a SUMO-network for its usage in a router
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
#ifndef RONetHandler_h
#define RONetHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class OptionsCont;
class ROEdge;
class ROAbstractEdgeBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONetHandler
 * @brief The handler that parses a SUMO-network for its usage in a router
 *
 * SAX2-Handler for SUMO-network loading. As this class is used for both
 *  the dua- and the jp-router, a reference to the edge builder is given.
 */
class RONetHandler : public SUMOSAXHandler
{
public:
    /** @brief Constructor
     *
     * @param[in] net The network instance to fill
     * @param[in] eb The abstract edge builder to use
     */
    RONetHandler(RONet &net, ROAbstractEdgeBuilder &eb);


    /// @brief Destructor
    virtual ~RONetHandler() throw();


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
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}

protected:
    /// Parses an edge
    void parseEdge(const SUMOSAXAttributes &attrs);

    /// Parses a lane
    virtual void parseLane(const SUMOSAXAttributes &attrs);

    /// Parses a junction
    void parseJunction(const SUMOSAXAttributes &attrs);

    void parseConnectingEdge(const SUMOSAXAttributes &attrs);
    void parseConnectedEdge(const SUMOSAXAttributes &attrs);


protected:
    /// the net to store the information into
    RONet &myNet;

    /// the name of the edge/node that is currently processed
    std::string myCurrentName;

    /// the currently build edge
    ROEdge *myCurrentEdge;

    /** @brief An indicator whether the next edge shall be read
        Internal edges are not read by now */
    bool myProcess;

    std::vector<SUMOVehicleClass> myAllowedClasses;
    std::vector<SUMOVehicleClass> myNotAllowedClasses;

    /// The object used to build of edges of the desired type
    ROAbstractEdgeBuilder &myEdgeBuilder;

private:
    /// @brief Invalidated copy constructor
    RONetHandler(const RONetHandler &src);

    /// @brief Invalidated assignment operator
    RONetHandler &operator=(const RONetHandler &src);

};


#endif

/****************************************************************************/

