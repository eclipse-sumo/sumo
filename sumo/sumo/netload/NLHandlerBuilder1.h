#ifndef NLHandlerBuilder1_h
#define NLHandlerBuilder1_h
/***************************************************************************
                          NLHandlerBuilder1.h
			  The third-step - handler building structures
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.9  2002/06/11 14:39:24  dkrajzew
// windows eol removed
//
// Revision 1.8  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.7  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.6  2002/04/25 14:37:57  dkrajzew
// Declarations of size variables changed from signed to unsigned due to warnings
//
// Revision 1.5  2002/04/25 09:26:05  dkrajzew
// New names for the acceleration and the deceleration parameter applied
//
// Revision 1.4  2002/04/17 16:16:13  dkrajzew
// False include patched
//
// Revision 1.3  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:05:36  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:41  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:06  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax/HandlerBase.hpp>
#include "NLSAXHandler.h"
#include "NLNetBuilder.h"
#include "../microsim/MSBitSetLogic.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLHandlerBuilder1
 * This is the handler performing the third step of the parsing of the
 * XML-file used.
 * It is used for:
 * - parsing the data of previously allocated instances of the MSEdge-class
 * - parsing and allocating MSVehicleType-instances
 * - parsing and allocating MSNet::Route-instances
 */
class NLHandlerBuilder1 : public NLSAXHandler {
private:
    /// numerical ids for the attributes
    enum AttributeEnum { ATTR_ID, ATTR_DEPART, ATTR_MAXSPEED, ATTR_LENGTH,
        ATTR_CHANGEURGE, ATTR_ACCEL, ATTR_DECEL, ATTR_SIGMA, ATTR_KEY,
        ATTR_REQUEST, ATTR_RESPONSE, ATTR_TO, ATTR_FROM };
    /// the right-of-way-logic of the currently chosen bitset-logic
    MSBitsetLogic::Logic   *m_pActiveLogic;
    /// the transformation matrix of the bitset-logic currently chosen
    MSBitsetLogic::Link2LaneTrafo   *m_pActiveTrafo;
    /// the size of the request
    int     _requestSize;
    /// the size of the response
    int     _responseSize;
    /// the number of lanes
    int     _laneNo;
    /// inserted items
    int _trafoItems, _requestItems;
    /// the current key
    std::string    m_Key;
public:
    /// standard constructor
    NLHandlerBuilder1(NLContainer &container, LoadFilter filter);
    /// standard destructor
    ~NLHandlerBuilder1();
    /// returns a message about the processing
    std::string getMessage() const;

    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag;
        this method
        a) opens edges for the addition of their elements
        b) adds lanes to the edges
        c) opens descriptions of connections between edges
        d) adds vehicle-types or
        e) adds routes in dependence of the occured tag */
    void myStartElement(int element, const std::string &name, const Attributes &attrs);
    /** called on the end of an element;
        this method
        a) builds single edges by closing their description
        b) closes the addition of lanes to an edge
        c) closes the addition of connections to a previously chosen following
           edge or
        d) closes the generation of a route in dependence of the occured tag*/
    void myEndElement(int element, const std::string &name);
    /** called when simple characters occure; this method
        a) adds lanes connecting the previously chosen current edge with
           the previously chosen following edge
        b) adds edges to a route in dependence of the occured tag */
    void myCharacters(int element, const std::string &name, const std::string &chars);
private:
    /// begins the processing of an edge
    void chooseEdge(const Attributes &attrs);
    /// adds a lane to the previously opened edge
    void addLane(const Attributes &attrs);
    /// opens the list of next edges for processing
    void openAllowedEdge(const Attributes &attrs);
    /// adds a vehicle type
    void addVehicleType(const Attributes &attrs);
    // opens a route
    void openRoute(const Attributes &attrs);
    /// adds a junction key
    void addJunctionKey(const Attributes &attrs);
    /// initialises a junction logic
    void initLogic();
    /// adds a logic item to the current logic
    void addLogicItem(const Attributes &attrs);
    /// adds a transformation item to the current logic
    void addTrafoItem(const Attributes &attrs);
    /// adds the list of allowed following edges
    void addAllowedEdges(const std::string &chars);
    /// adds the route elements
    void addRouteElements(const std::string &name, const std::string &chars);
    /// sets the request size of the current junction logic
    void setRequestSize(const std::string &chars);
    /// sets the response size of the current junction logic
    void setResponseSize(const std::string &chars);
    /// sets the lane number of the current junction logic
    void setLaneNumber(const std::string &chars);
    /// sets the key of the current junction logic
    void setKey(const std::string &chars);
    /// adds a logic item
    void addLogicItem(int request, const std::string &response);
    /// adds a trafo item
    void addTrafoItem(const std::string &links, int lane);
    /** returns the build logic */
    void closeLogic();

private:
    /** invalid copy constructor */
    NLHandlerBuilder1(const NLHandlerBuilder1 &s);
    /** invalid assignment operator */
    NLHandlerBuilder1 &operator=(const NLHandlerBuilder1 &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerBuilder1.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
