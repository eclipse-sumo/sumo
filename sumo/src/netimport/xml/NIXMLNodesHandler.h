/****************************************************************************/
/// @file    NIXMLNodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Used to load the XML-description of the nodes given in a
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
#ifndef NIXMLNodesHandler_h
#define NIXMLNodesHandler_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/xml/SUMOSAXHandler.h>
#include <utils/geom/Position2D.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBNode;
class NBNodeCont;
class NBTrafficLightLogicCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIXMLNodesHandler
 * A class that parses nodes from XML-files
 */
class NIXMLNodesHandler
            : public SUMOSAXHandler
{

public:
    /// standard constructor
    NIXMLNodesHandler(NBNodeCont &nc, NBTrafficLightLogicCont &tlc,
                      OptionsCont &options);

    /// Destructor
    ~NIXMLNodesHandler();

protected:
    /// The method called by the SAX-handler to parse start tags
    void myStartElement(int element, const std::string &name,
                        const Attributes &attrs);

    /// The method called by the SAX-handler to parse characters
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);

    /// The method called by the SAX-handler to parse end tags
    void myEndElement(int element, const std::string &name);

private:
    /** @brief Sets the position of the node
        Returns false when the information about the node's position was not valid */
    bool setPosition(const Attributes &attrs);

    /** @brief Builds the defined traffic light or adds a node to it */
    void processTrafficLightDefinitions(const Attributes &attrs,
                                        NBNode *currentNode);

private:
    /// A reference to the program's options
    OptionsCont &_options;

    /// The id of the currently parsed node
    std::string myID;

    /// The position of the currently parsed node
    Position2D myPosition;

    /// The (optional) type of the node currently parsed
    std::string myType;

    NBNodeCont &myNodeCont;

    NBTrafficLightLogicCont &myTLLogicCont;

private:
    /** invalid copy constructor */
    NIXMLNodesHandler(const NIXMLNodesHandler &s);

    /** invalid assignment operator */
    NIXMLNodesHandler &operator=(const NIXMLNodesHandler &s);

};


#endif

/****************************************************************************/

