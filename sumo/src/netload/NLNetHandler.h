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
// Revision 1.24  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.23  2004/06/17 13:08:16  dkrajzew
// Polygon visualisation added
//
// Revision 1.22  2004/04/02 11:23:52  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles all simulation-wide output
//
// Revision 1.21  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.20  2004/01/13 14:28:46  dkrajzew
// added alternative detector description; debugging
//
// Revision 1.19  2004/01/12 15:12:05  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.18  2004/01/12 14:46:21  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.17  2003/12/05 10:26:11  dkrajzew
// handling of internal links when theyre not wished improved
//
// Revision 1.16  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.15  2003/11/24 10:18:32  dkrajzew
// handling of definitions for minimum and maximum phase duration added;
//  modified the gld-offsets computation
//
// Revision 1.14  2003/11/17 07:22:03  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.13  2003/09/24 09:57:13  dkrajzew
// bug on building induct loops of an actuated tls within the gui patched
//
// Revision 1.12  2003/09/22 12:30:23  dkrajzew
// actuated traffic lights are now derived from simple traffic lights
//
// Revision 1.11  2003/09/17 06:53:23  dkrajzew
// phase definitions extracted from traffic lights; MSActuatedPhaseDefinition
//  is now derived from MSPhaseDefinition
//
// Revision 1.10  2003/09/05 15:20:19  dkrajzew
// loading of internal links added
//
// Revision 1.9  2003/08/18 12:41:40  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.8  2003/07/30 09:25:17  dkrajzew
// loading of directions and priorities of links implemented
//
// Revision 1.7  2003/07/22 15:12:16  dkrajzew
// new usage of detectors applied
//
// Revision 1.6  2003/07/07 08:35:10  dkrajzew
// changes due to loading of geometry applied from the gui-version
//  (no major drawbacks in loading speed)
//
// Revision 1.5  2003/06/18 11:18:05  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/05/20 09:45:35  dkrajzew
// some make-up done (splitting large methods; patching comments)
//
// Revision 1.3  2003/03/20 16:35:44  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/03 15:06:33  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.1  2003/02/07 11:18:56  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <sax/HandlerBase.hpp>
#include <sax/SAXException.hpp>
#include <sax/AttributeList.hpp>
#include "NLNetBuilder.h"
#include "NLDiscreteEventBuilder.h"
#include <microsim/MSLink.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/MSSimpleTrafficLightLogic.h>
#include <microsim/MSActuatedTrafficLightLogic.h>
#include <microsim/MSBitSetLogic.h>


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NLContainer;
class NLDetectorBuilder;
class NLTriggerBuilder;


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
    NLNetHandler(const std::string &file, NLContainer &container,
        NLDetectorBuilder &detBuilder, NLTriggerBuilder &triggerBuilder,
        double stdDetectorPositions, double stdDetectorlength);

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
    virtual void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** called on the end of an element;
        this method */
    virtual void myEndElement(int element, const std::string &name);

    /** @brief adds a detector
        Determines the type of the detector first, and then calls the
         appropriate method */
    virtual void addDetector(const Attributes &attrs);

    /// Builds an e1-detector using the given specification
    virtual void addE1Detector(const Attributes &attrs);

    /// Builds an e2-detector using the given specification
    virtual void addE2Detector(const Attributes &attrs);

    /// Starts building of an e3-detector using the given specification
    void beginE3Detector(const Attributes &attrs);

    void addE3Entry(const Attributes &attrs);
    void addE3Exit(const Attributes &attrs);

    /// Builds of an e3-detector using collected values
    virtual void endE3Detector();

    void endDetector();


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

    /// the description about which in-junction lanes disallow other passing the junction
    MSBitsetLogic::Foes                     *m_pActiveFoes;

    /// the current phase definitions for a simple traffic light
    MSSimpleTrafficLightLogic::Phases   m_ActivePhases;

    /// the size of the request
    int             _requestSize;

    /// the number of lanes
    int             _laneNo;

    /// counter for the inserted items
    int             _requestItems;

    /// the current key
    std::string     m_Key;

    /// the number of the current traffic light logic
    int             _tlLogicNo;

    /// The current junction type
    std::string     m_Type;

    /// The offset within the junction
    size_t          m_Offset;

    /// the current polygon
    std::string     poly_name;

    double m_DetectorOffset;
    double myStdDetectorPositions;
    double myStdDetectorLengths;

    /// The absolute duration of a tls-control loop
    size_t myAbsDuration;

    std::string actuell_poly_name;


private:
    /// sets the number of edges the network contains
    void setEdgeNumber(const Attributes &attrs);

    /// begins the processing of an edge
    void chooseEdge(const Attributes &attrs);

    /// adds a lane to the previously opened edge
    void addLane(const Attributes &attrs);

    /// adds a polygon
    void addPoly(const Attributes &attrs);

    /// add the position to the Polygon
    void addPolyPosition(const std::string &chars);

    /// opens the list of next edges for processing
    void openAllowedEdge(const Attributes &attrs);

    /// adds a junction key
    void addJunctionKey(const Attributes &attrs);

    /// initialises a junction logic
    void initJunctionLogic();

    /// adds a logic item to the current logic
    void addLogicItem(const Attributes &attrs);

    /// begins the reading of a traffic lights logic
    void initTrafficLightLogic(const Attributes &attrs);

    /// adds a phase to the traffic lights logic currently build
    void addPhase(const Attributes &attrs);


    /// opens a junction for processing
    virtual void openJunction(const Attributes &attrs);

    /// adds a source
    virtual void addSource(const Attributes &attrs);

    /// adds a trigger
    void addTrigger(const Attributes &attrs);

    /// opens the container of succeding lanes for processing
    void openSucc(const Attributes &attrs);

    /// adds a succeding lane
    void addSuccLane(const Attributes &attrs);


    /// adds the incoming lanes
    void addIncomingLanes(const std::string &chars);

    /// adds the incoming Polygon's Positions
    void NLNetHandler::addIncomingPolyPosititon(const std::string &chars);

    /// adds the internal lanes
    void addInternalLanes(const std::string &chars);


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

    /// sets the lane number of the current junction logic
    void setLaneNumber(const std::string &chars);

    /// sets the key of the current junction logic
    void setKey(const std::string &chars);

    /// Sets the offset a tl-logic shall be fired the first time after
    void setOffset(const std::string &chars);

    /// sets the number of the current logic
    void setTLLogicNo(const std::string &chars);

    /// adds a logic item
    void addLogicItem(int request, const std::string &response,
        const std::string &foes);

    /// ends the loading of a junction logic
    void closeJunctionLogic();

    /// ends the loading of a traffic lights logic
    virtual void closeTrafficLightLogic();

    /// ends the processing of a junction
    virtual void closeJunction();

    /// closes the processing of a lane
    void closeSuccLane();

    /// Parses the given character into an enumeration typed link direction
    MSLink::LinkDirection parseLinkDir(char dir);

    /// Parses the given character into an enumeration typed link state
    MSLink::LinkState parseLinkState(char state);

    /// Compute the initial step of a tls-logic from the given offset
    size_t computeInitTLSStep() const;

    /// Compute the time offset the tls shall for the first time
    size_t computeInitTLSEventOffset() const;


private:
    /// A builder for object actions
    NLDiscreteEventBuilder myActionBuilder;

    /// Information whether the currently parsed edge is internal and not wished, here
    bool myCurrentIsInternalToSkip;

    /// The detector builder to use
    NLDetectorBuilder &myDetectorBuilder;

    /// The type of the last detector
    std::string myDetectorType;

    /// The trigger builder to use
    NLTriggerBuilder &myTriggerBuilder;

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

