#ifndef NLNetHandler_h
#define NLNetHandler_h
//---------------------------------------------------------------------------//
//                        NLNetHandler.h -
//  The XML-Handler for network loading (parses the elements)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2003/02/07 11:18:56  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <sax/HandlerBase.hpp>
#include "NLNetBuilder.h"
#include <microsim/MSRouteHandler.h>
#include <microsim/MSSimpleTrafficLightLogic.h>
#include <microsim/MSBitSetLogic.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;
class SAXParseException;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLNetHandler
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 * be loaded from network descriptions.
 */
class NLNetHandler : public MSRouteHandler {
public:
    /// standard constructor
    NLNetHandler(bool verbose, bool warn, const std::string &file,
        NLContainer &container);

    /// Destructor
    virtual ~NLNetHandler();

    /// returns a message about the processing
    std::string getMessage() const;

    /// sets the data type filter
    void setWanted(LoadFilter filter);

protected:
    /** called on the occurence of the beginning of a tag;
        this method */
    virtual void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** called when simple characters occure; this method */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** called on the end of an element;
        this method */
    void myEndElement(int element, const std::string &name);

protected:
    /** adds the message about the occured error to the error handler
	after building it */
    void setError(const std::string &type,
        const SAXParseException& exception);

    /** returns the information whether instances belonging to the
        given class of data shall be extracted during this parsing */
    bool wanted(LoadFilter filter) const;

protected:
    /// the container (storage) for build data
    NLContainer                             &myContainer;

    /// the definition of what to load
    LoadFilter                              _filter;

    /** storage for the id of the currently build lane */
    std::string                             m_LaneId;

    /// the right-of-way-logic of the currently chosen bitset-logic
    MSBitsetLogic::Logic                    *m_pActiveLogic;

    /// the transformation matrix of the bitset-logic currently chosen
    MSBitsetLogic::Link2LaneTrafo           *m_pActiveTrafo;

    /// the current phase definitions
    MSSimpleTrafficLightLogic<64>::Phases   m_ActivePhases;

    /// the size of the request
    int             _requestSize;

    /// the size of the response
    int             _responseSize;

    /// the number of lanes
    int             _laneNo;

    /// counter for the inserted items
    int             _trafoItems, _requestItems;

    /// the current key
    std::string     m_Key;

    /// the number of the current traffic light logic
    int             _tlLogicNo;

private:
    /// sets the number of edges the network contains
    void setEdgeNumber(const Attributes &attrs);

    /// begins the processing of an edge
    void chooseEdge(const Attributes &attrs);

    /// adds a lane to the previously opened edge
    void addLane(const Attributes &attrs);

    /// opens the list of next edges for processing
    void openAllowedEdge(const Attributes &attrs);

    /// adds a junction key
    void addJunctionKey(const Attributes &attrs);

    /// initialises a junction logic
    void initJunctionLogic();

    /// adds a logic item to the current logic
    void addLogicItem(const Attributes &attrs);

    /// adds a transformation item to the current logic
    void addTrafoItem(const Attributes &attrs);


    /// begins the reading of a traffic lights logic
    void initTrafficLightLogic();

    /// adds a phase to the traffic lights logic currently build
    void addPhase(const Attributes &attrs);


    /// opens a junction for processing
    virtual void openJunction(const Attributes &attrs);

    /// adds a detector
    virtual void addDetector(const Attributes &attrs);

    /// adds a source
    virtual void addSource(const Attributes &attrs);

    /// adds a trigger
    void addTrigger(const Attributes &attrs);

    /// opens the container of succeding lanes for processing
    void openSucc(const Attributes &attrs);

    /// adds a succeding lane
    void addSuccLane(const Attributes &attrs);


    /// adds the incoming lanes
    void addInLanes(const std::string &chars);


    /// allocates edges from the initial list
    void allocateEdges(const std::string &chars);

    /// sets the number of nodes (allocates storage when needed)
    void setNodeNumber(const std::string &chars);


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

    /// sets the number of the current logic
    void setTLLogicNo(const std::string &chars);

    /// adds a logic item
    void addLogicItem(int request, const std::string &response);

    /// adds a trafo item
    void addTrafoItem(const std::string &links, int lane);

    /// ends the loading of a junction logic
    void closeJunctionLogic();

    /// ends the loading of a traffic lights logic
    void closeTrafficLightLogic();

    /// ends the processing of a junction
    virtual void closeJunction();

    /// closes the processing of a lane
    void closeSuccLane();

private:
    /** invalid copy constructor */
    NLNetHandler(const NLNetHandler &s);

    /** invalid assignment operator */
    NLNetHandler &operator=(const NLNetHandler &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLNetHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

