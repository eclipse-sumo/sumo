/****************************************************************************/
/// @file    NIXMLNodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIXMLNodesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for network nodes stored in XML
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
 * @class NIArcView_Loader
 * @brief Importer for network nodes stored in XML
 */
class NIXMLNodesHandler
            : public SUMOSAXHandler
{

public:
    /// standard constructor
    NIXMLNodesHandler(NBNodeCont &nc, NBTrafficLightLogicCont &tlc,
                      OptionsCont &options);

    /// Destructor
    ~NIXMLNodesHandler() throw();

protected:
    /// @name inherited from GenericSAXHandler
    //@{
    /// The method called by the SAX-handler to parse start tags
    void myStartElement(SumoXMLTag element,
                        const Attributes &attrs) throw(ProcessError);
    //@}

private:
    /** @brief Sets the position of the node
        Returns false when the information about the node's position was not valid */
    bool setPosition(const Attributes &attrs);

    /** @brief Builds the defined traffic light or adds a node to it */
    void processTrafficLightDefinitions(const Attributes &attrs,
                                        NBNode *currentNode);

private:
    /// A reference to the program's options
    OptionsCont &myOptions;

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

