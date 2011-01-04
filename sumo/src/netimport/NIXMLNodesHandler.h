/****************************************************************************/
/// @file    NIXMLNodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network nodes stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
 * @class NIXMLNodesHandler
 * @brief Importer for network nodes stored in XML
 *
 * This SAX-handler parses node information and stores it in the given node
 *  container. Additionally, the given tls-container may be filled with
 *  additional information.
 */
class NIXMLNodesHandler : public SUMOSAXHandler {

public:
    /** @brief Constructor
     *
     * @param[in, filled] nc The node container to fill
     * @param[in, filled] tlc The traffic lights container to fill
     * @param[in] options The options to use
     * @todo Options are only given to determine whether "flip-y" is set; maybe this should be done by giving a bool
     * @todo Why are options not const?
     */
    NIXMLNodesHandler(NBNodeCont &nc, NBTrafficLightLogicCont &tlc,
                      OptionsCont &options);


    /// @brief Destructor
    ~NIXMLNodesHandler() throw();


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag; Parses node information
     *
     * Tries to parse a node. If the node can be parsed, it is stored within
     *  "myNodeCont". Otherwise an error is generated. Then, if givenm
     *  the tls information is parsed and inserted into "myTLLogicCont".
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails (not used herein)
     * @recheck-policy No exception in order to allow further processing
     * @todo ProcessErrors are thrown when parsing traffic lights!?
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}


private:
    /** @brief Builds the defined traffic light or adds a node to it
     *
     * @param[in] attrs Attributes within the currently opened node
     * @param[in] currentNode The built node to add the tls information to
     */
    void processTrafficLightDefinitions(const SUMOSAXAttributes &attrs,
                                        NBNode *currentNode);


private:
    /// @brief A reference to the program's options
    OptionsCont &myOptions;

    /// @brief The id of the currently parsed node
    std::string myID;

    /// @brief The position of the currently parsed node
    Position2D myPosition;

    /// @brief The node container to add built nodes to
    NBNodeCont &myNodeCont;

    /// @brief The traffic lights container to add built tls to
    NBTrafficLightLogicCont &myTLLogicCont;


private:
    /** @brief invalid copy constructor */
    NIXMLNodesHandler(const NIXMLNodesHandler &s);

    /** @brief invalid assignment operator */
    NIXMLNodesHandler &operator=(const NIXMLNodesHandler &s);

};


#endif

/****************************************************************************/

