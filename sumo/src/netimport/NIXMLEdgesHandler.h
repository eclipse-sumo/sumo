/****************************************************************************/
/// @file    NIXMLEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network edges stored in XML
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

#include <utils/geom/Position2DVector.h>
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
    NIXMLEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                      NBTypeCont &tc, NBDistrictCont &dc, OptionsCont &options) throw();


    /// @brief Destructor
    ~NIXMLEdgesHandler() throw();


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


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    void myCharacters(SumoXMLTag element,
                      const std::string &chars) throw(ProcessError);


    /** @brief Called when a closing tag occures
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}

private:
    /** @brief Tries to parse the shape definition
     *
     * Returns the edge's geometry (may be empty if no one was defined).
     * Writes an error message if an error occured.
     * @param[in] attrs The attributes to read the shape from
     * @return The edge's shape
     */
    Position2DVector tryGetShape(const SUMOSAXAttributes &attrs) throw();

    /// Tries to set information needed by the nodes
    bool setNodes(const SUMOSAXAttributes &attrs);

    /** @brief tries to parse one of the node's positions
        Which position has to be parsed is defined by the given call variables */
    SUMOReal tryGetPosition(const SUMOSAXAttributes &attrs, SumoXMLAttr attrID,
                            const std::string &attrName);

    NBNode * insertNodeChecking(const Position2D &pos,
                                const std::string &name, const std::string &dir);


private:
    /// @brief A reference to the program's options
    OptionsCont &myOptions;

    /// @brief The current edge's id
    std::string myCurrentID;

    /// @brief The current edge's maximum speed
    SUMOReal myCurrentSpeed;

    /// @brief The current edge's priority
    int myCurrentPriority;

    /// @brief The current edge's number of lanes
    int myCurrentLaneNo;

    /// @brief The current edge's type
    std::string myCurrentType;

    /// @brief The nodes the edge starts and ends at
    NBNode *myFromNode, *myToNode;

    /// @brief The current edge's length
    SUMOReal myLength;

    /// @brief The shape of the edge
    Position2DVector myShape;

    /// @brief Information about how to spread the lanes
    NBEdge::LaneSpreadFunction myLanesSpread;

    /// @brief The nodes container (for retrieval of referenced nodes)
    NBNodeCont &myNodeCont;

    /// @brief The edges container (for insertion of build edges)
    NBEdgeCont &myEdgeCont;

    /// @brief The types container (for retrieval of type defaults)
    NBTypeCont &myTypeCont;

    /// @brief The districts container (needed if an edge must be split)
    NBDistrictCont &myDistrictCont;

    /// @brief Whether this edge definition is an update of a previously inserted edge
    bool myIsUpdate;

    /// @brief The currently processed edge
    NBEdge *myCurrentEdge;


    /** @struct Expansion
     * @brief A structure which describes changes of lane number along the road
     */
    struct Expansion {
        /// @brief The lanes until this change
        std::vector<int> lanes;
        /// @brief The position of this change
        SUMOReal pos;
        /// @brief A numerical id
        int nameid;
        /// @brief A 2D-position (for the node to insert at this place)
        Position2D gpos;
    };

    /// @brief The list of this edge's expansions
    std::vector<Expansion> myExpansions;


    class expansions_sorter {
    public:
        explicit expansions_sorter() { }

        /// comparing operator
        int operator()(const Expansion &e1, const Expansion &e2) const {
            return e1.pos > e2.pos;
        }
    };

    class expansion_by_pos_finder {
    public:
        /** constructor */
        explicit expansion_by_pos_finder(SUMOReal pos)
                : myPosition(pos) { }

        /** the comparing function */
        bool operator()(const Expansion &e) {
            return e.pos==myPosition;
        }

    private:
        /// The position to search for
        SUMOReal myPosition;

    };

    /// @brief Information whether one edge with a function-attribute occured and was reported
    bool myHaveReportedAboutFunctionDeprecation;

    /// @brief Information whether at least one edge's attributes were overwritten
    bool myHaveReportedAboutOverwriting;


private:
    /** @brief invalid copy constructor */
    NIXMLEdgesHandler(const NIXMLEdgesHandler &s);

    /** @brief invalid assignment operator */
    NIXMLEdgesHandler &operator=(const NIXMLEdgesHandler &s);

};


#endif

/****************************************************************************/

