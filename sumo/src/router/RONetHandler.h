/****************************************************************************/
/// @file    RONetHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The handler for SUMO-Networks
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
 * SAX2-Handler for SUMO-network loading. As this class is used for both
 *  the dua- and the jp-router, a reference to the edge builder is given.
 */
class RONetHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    RONetHandler(OptionsCont &oc, RONet &net, ROAbstractEdgeBuilder &eb);

    /// Destrcutor
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


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    virtual void myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError);
    //@}

protected:
    /// Parses an edge
    void parseEdge(const SUMOSAXAttributes &attrs);

    /// Parses a lane
    virtual void parseLane(const SUMOSAXAttributes &attrs);

    /// Parses a junction
    void parseJunction(const SUMOSAXAttributes &attrs);

    /// Parses edge connections
    void parseConnEdge(const SUMOSAXAttributes &attrs);

    /// Preallocates edges
    void preallocateEdges(const std::string &chars);

protected:
    /// the options (program settings)
    OptionsCont &myOptions;

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
    /// invalidated copy constructor
    RONetHandler(const RONetHandler &src);

    /// invalidated assignment operator
    RONetHandler &operator=(const RONetHandler &src);

};


#endif

/****************************************************************************/

