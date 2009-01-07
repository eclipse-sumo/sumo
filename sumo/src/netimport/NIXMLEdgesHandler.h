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
class MsgHandler;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIArcView_Loader
 * @brief Importer for network edges stored in XML
 *
 * This SAX-handler parses edge information and stores it in the given
 *  container.
 */
class NIXMLEdgesHandler : public SUMOSAXHandler
{
public:
    /** @brief Constructor
     */
    NIXMLEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                      NBTypeCont &tc, NBDistrictCont &dc, OptionsCont &options);

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

    /// tries to parse the shape definition
    Position2DVector tryGetShape(const SUMOSAXAttributes &attrs);

    /// Tries to set information needed by the nodes
    bool setNodes(const SUMOSAXAttributes &attrs);

    /** @brief tries to parse one of the node's positions
        Which position has to be parsed is defined by the given call variables */
    SUMOReal tryGetPosition(const SUMOSAXAttributes &attrs, SumoXMLAttr attrID,
                            const std::string &attrName);

    NBNode * insertNodeChecking(const Position2D &pos,
                                const std::string &name, const std::string &dir);


private:
    /// A reference to the program's options
    OptionsCont &myOptions;

    /// The current edge's id
    std::string myCurrentID;

    /// The current edge's maximum speed
    SUMOReal myCurrentSpeed;

    /// The current edge's priority
    int myCurrentPriority;

    /// The current edge's number of lanes
    int myCurrentLaneNo;

    /// The current edge's type
    std::string myCurrentType;

    /// The nodes
    NBNode *myFromNode, *myToNode;

    /// The current edge's length
    SUMOReal myLength;

    /// The shape of the edge
    Position2DVector myShape;

    /// Information about how to spread the lanes
    NBEdge::LaneSpreadFunction myLanesSpread;

    NBNodeCont &myNodeCont;

    NBEdgeCont &myEdgeCont;

    NBTypeCont &myTypeCont;

    NBDistrictCont &myDistrictCont;

    struct Expansion {
        std::vector<int> lanes;
        SUMOReal pos;
        int nameid;
        Position2D gpos;
    };

    std::vector<Expansion> myExpansions;

    NBEdge *myCurrentEdge;

    class expansions_sorter
    {
    public:
        explicit expansions_sorter() { }

        /// comparing operator
        int operator()(const Expansion &e1, const Expansion &e2) const {
            return e1.pos > e2.pos;
        }
    };

    class expansion_by_pos_finder
    {
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


private:
    /** @brief invalid copy constructor */
    NIXMLEdgesHandler(const NIXMLEdgesHandler &s);

    /** @brief invalid assignment operator */
    NIXMLEdgesHandler &operator=(const NIXMLEdgesHandler &s);

};


#endif

/****************************************************************************/

