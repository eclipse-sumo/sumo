#ifndef NBXMLDefinitions_h
#define NBXMLDefinitions_h

#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>

enum TagEnum
{
    /** invalid tag */
    SUMO_TAG_NOTHING,
    /** begin/end of the simulation description */
    SUMO_TAG_SIMULATION,
    /** begin/end of the description of an edge */
    SUMO_TAG_EDGE,
    /** begin/end of the description of a single lane */
    SUMO_TAG_LANE,
    /** begin/end of the list of lane descriptions */
    SUMO_TAG_LANES,
    /** description of a connected edge */
    SUMO_TAG_CEDGE,
    /** begin/end of the description of a junction */
    SUMO_TAG_JUNCTION,
    /** begin/end of the list of incoming lanes (into a junction) */
    SUMO_TAG_INLANES,
    /** a detector */
    SUMO_TAG_DETECTOR,
    /** description of a vehicle */
    SUMO_TAG_VEHICLE,
    /** description of a vehicle type */
    SUMO_TAG_VTYPE,
    /** begin/end of the description of a route */
    SUMO_TAG_ROUTE,
    /** begin/end of the description of succeding lanes */
    SUMO_TAG_SUCC,
    /** description of a single succeding lane */
    SUMO_TAG_SUCCLANE,
    /** begin/end of the key of a junction */
    SUMO_TAG_KEY,
    /** begin/end of the list of junction logic parts */
    SUMO_TAG_JUNCTIONLOGIC,
    /** begin of a right-of-way logic */
    SUMO_TAG_ROWLOGIC,
    /** the request size (of a junction logic) */
    SUMO_TAG_REQUESTSIZE,
    /** the response size (of a junction logic) */
    SUMO_TAG_RESPONSESIZE,
    /** the number of lanes */
    SUMO_TAG_LANENUMBER,
    /** description of a single junction logic part */
    SUMO_TAG_LOGICITEM,
    /** a transformation item */
    SUMO_TAG_TRAFOITEM,
    /** a source */
    SUMO_TAG_SOURCE,
    /** a district */
    SUMO_TAG_DISTRICT,
    /** a source within a district (connection road) */
    SUMO_TAG_DSOURCE,
    /** a sink within a district (connection road) */
    SUMO_TAG_DSINK,
    /** the list of edge names for preallocation */
    SUMO_TAG_EDGES,
    /** the number of nodes within the net */
    SUMO_TAG_NODECOUNT,
    /** a traffic light logic */
    SUMO_TAG_TLLOGIC,
    /** the time offset from the first switch */
    SUMO_TAG_OFFSET,
    /** the initial step of the traffic light */
    SUMO_TAG_INITSTEP,
    /** the number of phases of the traffic light */
    SUMO_TAG_PHASENO,
    /** a single phase description */
    SUMO_TAG_PHASE,
    /** the position of an edge */
    SUMO_TAG_EDGEPOS,
    /** the route alternatives */
    SUMO_TAG_ROUTEALT,
    /** a single trip definition (used by router) */
    SUMO_TAG_TRIPDEF
};

enum AttrEnum
{
    /* common attributes */
    SUMO_ATTR_ID,
    SUMO_ATTR_NAME,
    SUMO_ATTR_TYPE,
    SUMO_ATTR_PRIORITY,
    SUMO_ATTR_NOLANES,
    SUMO_ATTR_SPEED,
    /* edge attributes */
    SUMO_ATTR_LENGTH,
    SUMO_ATTR_FROMNODE,
    SUMO_ATTR_TONODE,
    SUMO_ATTR_XFROM,
    SUMO_ATTR_YFROM,
    SUMO_ATTR_XTO,
    SUMO_ATTR_YTO,
    /* node attributes */
    SUMO_ATTR_X,
    SUMO_ATTR_Y,
    /* sumo-junction attributes */
    SUMO_ATTR_KEY,
    /// the weight of a district's source or sink
    SUMO_ATTR_WEIGHT,
    /* vehicle attributes */
    SUMO_ATTR_DEPART,
    SUMO_ATTR_ROUTE,
    SUMO_ATTR_MAXSPEED,
    SUMO_ATTR_ACCEL,
    SUMO_ATTR_DECEL,
    SUMO_ATTR_SIGMA,
    /* route alternatives attributes */
    SUMO_ATTR_LAST,
    SUMO_ATTR_COST,
    SUMO_ATTR_PROP,
    /* trip definition attributes */
    SUMO_ATTR_POS,
    SUMO_ATTR_LANE,
    SUMO_ATTR_FROM,
    SUMO_ATTR_TO
};

extern size_t noSumoTags;
extern GenericSAX2Handler::Tag sumotags[35];

extern size_t noSumoAttrs;
extern AttributesHandler::Attr sumoattrs[30];

#endif
