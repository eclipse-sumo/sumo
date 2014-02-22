/****************************************************************************/
/// @file    NIXMLEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network edges stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIXMLEdgesHandler_h
#define NIXMLEdgesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/PositionVector.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBNode;
class NBEdge;
class NBNodeCont;
class NBEdgeCont;
class NBTypeCont;
class NBDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIXMLEdgesHandler
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
class NIXMLEdgesHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     * @param[in] nc The nodes container (for retrieval of referenced nodes)
     * @param[in] ec The edges container (for insertion of build edges)
     * @param[in] tc The types container (for retrieval of type defaults)
     * @param[in] dc The districts container (needed if an edge must be split)
     * @param[in] options The options to use while building edges
     */
    NIXMLEdgesHandler(NBNodeCont& nc, NBEdgeCont& ec,
                      NBTypeCont& tc, NBDistrictCont& dc,
                      NBTrafficLightLogicCont& tlc,
                      OptionsCont& options);


    /// @brief Destructor
    ~NIXMLEdgesHandler();


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
     * Writes an error message if an error occured.
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
    /// @brief A reference to the program's options
    OptionsCont& myOptions;


    /// @name Currently parsed edge's values
    /// @{

    /// @brief The current edge's id
    std::string myCurrentID;

    /// @brief The current edge's maximum speed
    SUMOReal myCurrentSpeed;

    /// @brief The current edge's priority
    int myCurrentPriority;

    /// @brief The current edge's number of lanes
    int myCurrentLaneNo;

    /// @brief The current edge's lane width
    SUMOReal myCurrentWidth;

    /// @brief The current edge's offset till the destination node
    SUMOReal myCurrentOffset;

    /// @brief The current edge's street name
    std::string myCurrentStreetName;

    /// @brief The current edge's type
    std::string myCurrentType;

    /// @brief The nodes the edge starts and ends at
    NBNode* myFromNode, *myToNode;

    /// @brief The current edge's length
    SUMOReal myLength;

    /// @brief The shape of the edge
    PositionVector myShape;

    /// @brief Information about how to spread the lanes
    LaneSpreadFunction myLanesSpread;

    /// @brief Information about lane permissions
    SVCPermissions myPermissions;

    /// @brief Whether the edge shape shall be kept at reinitilization
    bool myReinitKeepEdgeShape;

    /// @}


    /// @brief Whether this edge definition is an update of a previously inserted edge
    bool myIsUpdate;


    /// @name Used instance containers (access to nodes, edges, types, etc.)
    /// @{

    /// @brief The nodes container (for retrieval of referenced nodes)
    NBNodeCont& myNodeCont;

    /// @brief The edges container (for insertion of build edges)
    NBEdgeCont& myEdgeCont;

    /// @brief The types container (for retrieval of type defaults)
    NBTypeCont& myTypeCont;

    /// @brief The districts container (needed if an edge must be split)
    NBDistrictCont& myDistrictCont;

    /** @brief The traffic lights container to add built tls to (when
     * invalidating tls because of splits) */
    NBTrafficLightLogicCont& myTLLogicCont;
    /// @}


    /// @brief The currently processed edge
    NBEdge* myCurrentEdge;

    /** @struct Split
     * @brief A structure which describes changes of lane number along the road
     */
    struct Split {
        /// @brief The lanes until this change
        std::vector<int> lanes;
        /// @brief The position of this change
        SUMOReal pos;
        /// @brief A numerical id
        int nameid;
        /// @brief A 2D-position (for the node to insert at this place)
        Position gpos;
    };

    /// @brief The list of this edge's splits
    std::vector<Split> mySplits;


    /** @class split_sorter
     * @brief Sorts splits by their position (increasing)
     */
    class split_sorter {
    public:
        /// @brief Constructor
        explicit split_sorter() { }

        /// @brief Comparing operator
        int operator()(const Split& e1, const Split& e2) const {
            return e1.pos < e2.pos;
        }
    };


    /** @class split_by_pos_finder
     * @brief Finds a split at the given position
     */
    class split_by_pos_finder {
    public:
        /// @brief Constructor
        explicit split_by_pos_finder(SUMOReal pos)
            : myPosition(pos) { }

        /// @brief Comparing operator
        bool operator()(const Split& e) {
            return e.pos == myPosition;
        }

    private:
        /// @brief The position to search for
        SUMOReal myPosition;

    };


    /// @brief Information whether at least one edge's attributes were overwritten
    bool myHaveReportedAboutOverwriting;

    bool myHaveWarnedAboutDeprecatedLaneId;

    /// @brief Whether the edge shape shall be kept generally
    const bool myKeepEdgeShape;


private:

    /** @brief Parses an edge and stores the values in "myCurrentEdge"
     * @param[in] attrs The attributes to get the edge's values from
     */
    void addEdge(const SUMOSAXAttributes& attrs);

    /** @brief parses delete tag and deletes the specified edge
     * @param[in] attrs The attributes to get the edge's id from
     */
    void deleteEdge(const SUMOSAXAttributes& attrs);

    /** @brief Parses a lane and modifies myCurrentEdge according to the given
     * attribures
     * @param[in] attrs The attributes to get the lanes's values from
     */
    void addLane(const SUMOSAXAttributes& attrs);

    /** @brief Parses a split and stores it in mySplits. Splits are executed Upon reading the end
     * tag of an edge
     * @param[in] attrs The attributes to get the splits's values from
     */
    void addSplit(const SUMOSAXAttributes& attrs);

private:
    /** @brief invalid copy constructor */
    NIXMLEdgesHandler(const NIXMLEdgesHandler& s);

    /** @brief invalid assignment operator */
    NIXMLEdgesHandler& operator=(const NIXMLEdgesHandler& s);

};


#endif

/****************************************************************************/

