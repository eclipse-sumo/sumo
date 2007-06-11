/****************************************************************************/
/// @file    NIXMLEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Realises the loading of the edges given in a XML-format
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
class NBNodeCont;
class NBEdgeCont;
class NBTypeCont;
class NBDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIXMLEdgesHandler
 * A class that parses edges from XML-files
 */
class NIXMLEdgesHandler : public SUMOSAXHandler
{
public:
    /// standard constructor
    NIXMLEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                      NBTypeCont &tc, NBDistrictCont &dc, OptionsCont &options);

    /// Destructor
    ~NIXMLEdgesHandler() throw();

protected:
    /// @name inherited from GenericSAXHandler
    //@{ 
    /// The method called by the SAX-handler to parse start tags
    void myStartElement(SumoXMLTag element, const std::string &name,
                        const Attributes &attrs) throw();

    /// The method called by the SAX-handler to parse characters
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars) throw();

    /// The method called by the SAX-handler to parse end tags
    void myEndElement(SumoXMLTag element, const std::string &name) throw();
    //@}

private:
    /// Parses the id from the given attributes
    void setID(const Attributes &attrs);

    /// Parses the name from the given attributes
    void setName(const Attributes &attrs);

    /// Sets the type from the given attributes
    void checkType(const Attributes &attrs);

    /// Sets the speed from the given type or the given attributes
    void setGivenSpeed(const Attributes &attrs);

    /// Sets the number of lanes from the given type or the given attributes
    void setGivenLanes(const Attributes &attrs);

    /// Sets the priority from the given type or the given attributes
    void setGivenPriority(const Attributes &attrs);

    /// Sets the length of the edge, computing it in prior if necessary
    void setLength(const Attributes &attrs);

    /// tries to parse the shape definition
    Position2DVector tryGetShape(const Attributes &attrs);

    void setGivenType(const Attributes &attrs);


    /// Tries to set information needed by the nodes
    bool setNodes(const Attributes &attrs);

    /** @brief tries to parse one of the node's positions
        Which position has to be parsed is defined by the given call variables */
    SUMOReal tryGetPosition(const Attributes &attrs, SumoXMLAttr attrID,
                            const std::string &attrName);

    /** @brief Build the enodes in respect to the given parameters
        Returns the information whether the nodes could be build correctly */
    bool insertNodesCheckingCoherence();

    /// Parses the optional information of how to spread the lanes
    NBEdge::LaneSpreadFunction getSpreadFunction(const Attributes &attrs);


private:
    /// A reference to the program's options
    OptionsCont &_options;

    /// The current edge's id
    std::string myCurrentID;

    /// The current edge's name
    std::string myCurrentName;

    /// The current edge's maximum speed
    SUMOReal myCurrentSpeed;

    /// The current edge's priority
    int myCurrentPriority;

    /// The current edge's number of lanes
    int myCurrentLaneNo;

    /// The current edge's function
    NBEdge::EdgeBasicFunction myCurrentEdgeFunction;

    /// The current edge's type
    std::string myCurrentType;

    /// The ids of the begin and the end node
    std::string myCurrentBegNodeID, myCurrentEndNodeID;

    /// The positions of the nodes
    SUMOReal myBegNodeXPos, myBegNodeYPos, myEndNodeXPos, myEndNodeYPos;

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

    struct Expansion
    {
        std::vector<int> lanes;
        SUMOReal pos;
        int nameid;
        Position2D gpos;
    };

    std::vector<Expansion> myExpansions;

    class expansions_sorter
    {
    public:
        explicit expansions_sorter()
        { }

        /// comparing operator
        int operator()(const Expansion &e1, const Expansion &e2) const
        {
            return e1.pos > e2.pos;
        }
    };

    class expansion_by_pos_finder
    {
    public:
        /** constructor */
        explicit expansion_by_pos_finder(SUMOReal pos)
                : myPosition(pos)
        { }

        /** the comparing function */
        bool operator()(const Expansion &e)
        {
            return e.pos==myPosition;
        }

    private:
        /// The position to search for
        SUMOReal myPosition;

    };


private:
    /** invalid copy constructor */
    NIXMLEdgesHandler(const NIXMLEdgesHandler &s);

    /** invalid assignment operator */
    NIXMLEdgesHandler &operator=(const NIXMLEdgesHandler &s);

};


#endif

/****************************************************************************/

