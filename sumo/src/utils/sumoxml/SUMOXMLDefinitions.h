#ifndef SUMOXMLDefinitions_h
#define SUMOXMLDefinitions_h
//---------------------------------------------------------------------------//
//                        SUMOXMLDefinitions.h -
//  Definitions of SUMO-tags
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
// Revision 1.8  2003/05/21 15:15:43  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.7  2003/05/20 09:52:39  dkrajzew
// MSTrafficLane-transformations were no longer needed
//
// Revision 1.6  2003/03/20 16:41:12  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.5  2003/03/03 15:27:01  dkrajzew
// period attribute for router added
//
// Revision 1.4  2003/02/07 10:53:23  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>


/* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * Numbers representing SUMO-XML - tags (element names)
 */
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
    /** the number of lanes */
    SUMO_TAG_LANENUMBER,
    /** description of a single junction logic part */
    SUMO_TAG_LOGICITEM,
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
    SUMO_TAG_TRIPDEF,
    /** the number of the logic (traffic light logic) */
    SUMO_TAG_LOGICNO,
    /** a trigger definition */
    SUMO_TAG_TRIGGER,
    /** trigger: a step description */
    SUMO_TAG_STEP
};


/**
 * Numbers representing SUMO-XML - attributes
 */
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
    /* lane attributes */
    SUMO_ATTR_CHANGEURGE,
    /* node attributes */
    SUMO_ATTR_X,
    SUMO_ATTR_Y,
    /* sumo-junction attributes */
    SUMO_ATTR_KEY,
    SUMO_ATTR_REQUEST,
    SUMO_ATTR_RESPONSE,
    /// the weight of a district's source or sink
    SUMO_ATTR_WEIGHT,
    /* vehicle attributes */
    SUMO_ATTR_DEPART,
    SUMO_ATTR_ROUTE,
    SUMO_ATTR_MAXSPEED,
    SUMO_ATTR_ACCEL,
    SUMO_ATTR_DECEL,
    SUMO_ATTR_SIGMA,
    SUMO_ATTR_REPNUMBER,
    /* route alternatives attributes */
    SUMO_ATTR_LAST,
    SUMO_ATTR_COST,
    SUMO_ATTR_PROP,
    /* trip definition attributes */
    SUMO_ATTR_POS,
    SUMO_ATTR_LANE,
    SUMO_ATTR_FROM,
    SUMO_ATTR_TO,
    SUMO_ATTR_PERIOD,
    /* source definitions */
    SUMO_ATTR_FUNC,
    SUMO_ATTR_POSITION,
    SUMO_ATTR_SPLINTERVAL,
    SUMO_ATTR_STYLE,
    SUMO_ATTR_FILE,
    SUMO_ATTR_JUNCTION,
    SUMO_ATTR_YIELD,
    SUMO_ATTR_NO,
    SUMO_ATTR_PHASE,
    SUMO_ATTR_BRAKE,
	SUMO_ATTR_YELLOW,
    SUMO_ATTR_DURATION,
    /** trigger: definition of an object type to be triggered */
    SUMO_ATTR_OBJECTTYPE,
    /** trigger: definition of an attribute to trigger */
    SUMO_ATTR_ATTR,
    /** trigger: name of the object to manipulate */
    SUMO_ATTR_OBJECTID,
    /** trigger: the time of the step */
    SUMO_ATTR_TIME,
    /** routes: information whether not only one vehicle uses the route */
    SUMO_ATTR_MULTIR,
    /** weights: one possible value */
    SUMO_ATTR_VALUE,
    /** weights: time range begin */
    SUMO_ATTR_BEGIN,
    /** weights: time range end */
    SUMO_ATTR_END
};


/* =========================================================================
 * declarations
 * ======================================================================= */
/// The number of SUMO-XML element names
extern size_t noSumoTags;
/// The names of SUMO-XML elements
extern GenericSAX2Handler::Tag sumotags[36];

/// The number of SUMO-XML attributes
extern size_t noSumoAttrs;
/// The names of SUMO-XML attributes
extern AttributesHandler::Attr sumoattrs[55];


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "SUMOXMLDefinitions.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

