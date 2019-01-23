/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIXMLPTHandler.h
/// @author  Jakob Erdmann
/// @date    Sat, 28 Jul 2018
/// @version $Id$
///
// Importer for static public transport information
/****************************************************************************/
#ifndef NIXMLPTHandler_h
#define NIXMLPTHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/PositionVector.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBNode;
class NBEdge;
class NBNodeCont;
class NBTypeCont;
class NBDistrictCont;
class NBTrafficLightLogicCont;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIXMLPTHandler
 * @brief Importer for network edges stored in XML
 *
 * This SAX-handler parses edge information and stores it in the given
 *  container.
 * @todo revalidate node retrieval
 * @todo One day, one should rethink the order of parsing. Now, the handler
 *  is able to load edges, using information from the types, first, and extending
 *  them by given information. In addition, if the read edge is already known,
 *  it's values are also used. Then, defining vehicles allowed per lane, and
 *  additional edge split definitions add some further complexity. This all
 *  works somehow for most of our use cases, but it's definitely not as consistent
 *  that everything what seems to be possible would also work appropriately.
 */
class NIXMLPTHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     * @param[in] nc The nodes container (for retrieval of referenced nodes)
     * @param[in] ec The edges container (for insertion of build edges)
     * @param[in] tc The types container (for retrieval of type defaults)
     * @param[in] dc The districts container (needed if an edge must be split)
     * @param[in] options The options to use while building edges
     */
    NIXMLPTHandler(NBEdgeCont& ec, NBPTStopCont& sc, NBPTLineCont& lc);


    /// @brief Destructor
    ~NIXMLPTHandler();

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
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element);
    //@}


private:
    /** @brief Tries to parse the shape definition
     *
     * Returns the edge's geometry (may be empty if no one was defined).
     * Writes an error message if an error occurred.
     * @param[in] attrs The attributes to read the shape from
     * @return The edge's shape
     */
    PositionVector tryGetShape(const SUMOSAXAttributes& attrs);


    /** @brief Tries to parse the spread type
     */
    LaneSpreadFunction tryGetLaneSpread(const SUMOSAXAttributes& attrs);


    /** @brief Sets from/to node information of the currently parsed edge
     *
     * If the nodes could be retrieved/built, they are set in myFromNode/myToNode,
     *  respectively, and true is returned. If not, false is returned.
     * @param[in] attrs The SAX-attributes to parse the nodes from
     * @return Whether valid nodes exist
     */
    bool setNodes(const SUMOSAXAttributes& attrs);


private:

    /// @brief The edges container (for retrieving referenced stop edge)
    NBEdgeCont& myEdgeCont;

    /// @brief The stop container (for loading of stops)
    NBPTStopCont& myStopCont;

    /// @brief The line container (for loading of lines)
    NBPTLineCont& myLineCont;

    /// @brief The currently processed stop
    NBPTStop* myCurrentStop;

    /// @brief The currently processed line
    NBPTLine* myCurrentLine;

    /// @brief the completion level of the current line
    double myCurrentCompletion;

    /// @brief element to receive parameters
    std::vector<Parameterised*> myLastParameterised;

private:

    /** @brief Parses an public transport stop
     * @param[in] attrs The attributes to get the stops's values from
     */
    void addPTStop(const SUMOSAXAttributes& attrs);

    /** @brief Parses an public transport stop reference within a line element
     * @param[in] attrs The attributes to get the stops's values from
     */
    void addPTLineStop(const SUMOSAXAttributes& attrs);

    /** @brief Parses an stop access definition
     * @param[in] attrs The attributes to get the access's values from
     */
    void addAccess(const SUMOSAXAttributes& attrs);

    /** @brief Parses a public transport line
     * @param[in] attrs The attributes to get the lines's values from
     */
    void addPTLine(const SUMOSAXAttributes& attrs);


private:
    /** @brief invalid copy constructor */
    NIXMLPTHandler(const NIXMLPTHandler& s);

    /** @brief invalid assignment operator */
    NIXMLPTHandler& operator=(const NIXMLPTHandler& s);

};


#endif

/****************************************************************************/

