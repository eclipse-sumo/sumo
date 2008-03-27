/****************************************************************************/
/// @file    NIOSMEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIOSMEdgesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
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
#ifndef NIOSMEdgesHandler_h
#define NIOSMEdgesHandler_h


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
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netimport/NIOSMNodesHandler.h>


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
 * @class NIOSMEdgesHandler
 * A class that parses edges from XML-files
 */
class NIOSMEdgesHandler : public SUMOSAXHandler
{
public:
    /// standard constructor
    NIOSMEdgesHandler(const std::map<int, NIOSMNode*> &osmNodes,
                      NBNodeCont &nc, NBEdgeCont &ec,
                      NBTypeCont &tc, NBDistrictCont &dc, OptionsCont &options);

    /// Destructor
    ~NIOSMEdgesHandler() throw();

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

    /// The method called by the SAX-handler to parse characters
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
    struct Edge {
        std::string id;
        std::string myHighWayType;
        std::string  myIsOneWay;
        std::vector<int> myCurrentNodes;
        bool myCurrentIsRoad;
    };


private:
    NBNode *insertNodeChecking(int id) ;
    void insertEdge(NIOSMEdgesHandler::Edge *e, int index, NBNode *from, NBNode *to,
                    const std::vector<int> &passed) ;

private:
    /// A reference to the program's options
    OptionsCont &myOptions;

    //
    const std::map<int, NIOSMNode*> &myOSMNodes;




    /// The current edge's maximum speed
    //SUMOReal myCurrentSpeed;

    /// The current edge's priority
    //int myCurrentPriority;

    /// The current edge's number of lanes
    //int myCurrentLaneNo;

    /// The current edge's function
    //NBEdge::EdgeBasicFunction myCurrentEdgeFunction;

    /// The current edge's type
    //std::string myCurrentType;

    /// The nodes
    //NBNode *myFromNode, *myToNode;

    /// The current edge's length
    //SUMOReal myLength;

    /// The shape of the edge
    Position2DVector myShape;

    /// Information about how to spread the lanes
    //NBEdge::LaneSpreadFunction myLanesSpread;

    NBNodeCont &myNodeCont;

    NBEdgeCont &myEdgeCont;

    NBTypeCont &myTypeCont;

    NBDistrictCont &myDistrictCont;

    std::vector<SumoXMLTag> myParentElements;

    std::map<std::string, Edge*> myEdgeMap;
    Edge*myCurrentEdge;

private:
    /** invalid copy constructor */
    NIOSMEdgesHandler(const NIOSMEdgesHandler &s);

    /** invalid assignment operator */
    NIOSMEdgesHandler &operator=(const NIOSMEdgesHandler &s);

};


#endif

/****************************************************************************/

