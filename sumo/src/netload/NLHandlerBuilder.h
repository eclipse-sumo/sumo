#ifndef NLHandlerBuilder_h
#define NLHandlerBuilder_h
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax/HandlerBase.hpp>
//#include "NLSAXHandler.h"
#include "NLNetBuilder.h"
#include <utils/sumoxml/SUMOSAXHandler.h>
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
 * NLHandlerBuilder
 */
class NLHandlerBuilder : public SUMOSAXHandler {
protected:
    /// the container (storage) for build data
    NLContainer                &myContainer;
    /// the definition of what to load
    LoadFilter                 _filter;
    /** storage for the id of the currently build lane */
    std::string                     m_LaneId;
    /// the right-of-way-logic of the currently chosen bitset-logic
    MSBitsetLogic::Logic            *m_pActiveLogic;
    /// the transformation matrix of the bitset-logic currently chosen
    MSBitsetLogic::Link2LaneTrafo   *m_pActiveTrafo;
    /// the current phase definitions
    MSSimpleTrafficLightLogic<64>::Phases m_ActivePhases;
    /// the current 
    /// the size of the request
    int     _requestSize;
    /// the size of the response
    int     _responseSize;
    /// the number of lanes
    int     _laneNo;
    /// counter for the inserted items
    int     _trafoItems, _requestItems;
    /// the current key
    std::string                     m_Key;
    /// the number of the current traffic light logic
    int _tlLogicNo;
public:
    /// standard constructor
    NLHandlerBuilder(NLContainer &container);
    /// standard destructor
    virtual ~NLHandlerBuilder();
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
private:
    void setEdgeNumber(const Attributes &attrs);
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
    void initJunctionLogic();
    /// adds a logic item to the current logic
    void addLogicItem(const Attributes &attrs);
    /// adds a transformation item to the current logic
    void addTrafoItem(const Attributes &attrs);

    void initTrafficLightLogic();
    void addPhase(const Attributes &attrs);

    /// opens a junction for processing
    virtual void openJunction(const Attributes &attrs);
    /// adds a vehicle
    virtual void addVehicle(const Attributes &attrs);
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
    void allocateEdges(const std::string &chars);
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
    /** returns the build logic */
    void closeJunctionLogic();
    void closeTrafficLightLogic();
    /// ends the processing of a junction
    virtual void closeJunction();
    /// closes the processing of a lane
    void closeSuccLane();

private:
    /** invalid copy constructor */
    NLHandlerBuilder(const NLHandlerBuilder &s);
    /** invalid assignment operator */
    NLHandlerBuilder &operator=(const NLHandlerBuilder &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
