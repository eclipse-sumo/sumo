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
    virtual ~RONetHandler();

protected:
    /// @name inherited from GenericSAXHandler
    //@{ 
    /** the user-impemlented handler method for an opening tag */
    virtual void myStartElement(SumoXMLTag element, const std::string &name,
                                const Attributes &attrs) throw();

    /** the user-implemented handler method for characters */
    virtual void myCharacters(SumoXMLTag element, const std::string &name,
                              const std::string &chars) throw();

    /** the user-implemented handler method for a closing tag */
    virtual void myEndElement(SumoXMLTag element, const std::string &name) throw();
    //@}

protected:
    /// Parses an edge
    void parseEdge(const Attributes &attrs);

    /// Parses a lane
    virtual void parseLane(const Attributes &attrs);

    /// Parses a junction
    void parseJunction(const Attributes &attrs);

    /// Parses edge connections
    void parseConnEdge(const Attributes &attrs);

    /// Preallocates edges
    void preallocateEdges(const std::string &chars);

protected:
    /// the options (program settings)
    OptionsCont &_options;

    /// the net to store the information into
    RONet &_net;

    /// the name of the edge/node that is currently processed
    std::string _currentName;

    /// the currently build edge
    ROEdge *_currentEdge;

    /** @brief An indicator whether the next edge shall be read
        Internal edges are not read by now */
    bool _process;

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

