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
// Revision 1.24  2004/01/26 07:15:54  dkrajzew
// new tags added for detector and trip amounts handling
//
// Revision 1.23  2004/01/13 14:29:19  dkrajzew
// added alternative detector description
//
// Revision 1.22  2004/01/12 15:14:09  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.21  2004/01/12 14:40:16  dkrajzew
// added detector attributes
//
// Revision 1.20  2003/12/04 13:15:43  dkrajzew
// handling of internal links added
//
// Revision 1.19  2003/12/04 13:14:08  dkrajzew
// gfx-module added temporary to sumo
//
// Revision 1.18  2003/11/24 10:19:13  dkrajzew
// handling of definitions for minimum and maximum phase duration added
//
// Revision 1.17  2003/11/17 07:23:07  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.16  2003/09/05 15:28:07  dkrajzew
// tags for internal link handling added
//
// Revision 1.15  2003/08/18 12:49:30  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.14  2003/07/30 09:28:40  dkrajzew
// link direction and priority attribute names added
//
// Revision 1.13  2003/07/16 15:39:39  dkrajzew
// color attribute added
//
// Revision 1.12  2003/07/08 12:11:08  dkrajzew
// comma at end of list - warning patched
//
// Revision 1.11  2003/07/07 08:50:29  dkrajzew
// added tags for shapes and lane position description
//
// Revision 1.10  2003/06/19 11:02:48  dkrajzew
// usage of false tag-enums patched
//
// Revision 1.9  2003/06/05 14:30:53  dkrajzew
// new attributes needed for loading new traffic lights added
//
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
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


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
    /** a detector */
    SUMO_TAG_DETECTOR,
    /** an e1-detector */
    SUMO_TAG_E1DETECTOR,
    /** an e2-detector */
    SUMO_TAG_E2DETECTOR,
    /** an e3-detector */
    SUMO_TAG_E3DETECTOR,
    /** an e3-entry point*/
    SUMO_TAG_DET_ENTRY,
    /** an e3-exit point */
    SUMO_TAG_DET_EXIT,
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
    SUMO_TAG_STEP,
    /** an aggreagated-output interval */
    SUMO_TAG_INTERVAL,
    /// The tag enclosing an object's shape
    SUMO_TAG_SHAPE,
    /// The definition of a periodic event
    SUMO_TAG_TIMEDEVENT,
    /** begin/end of the list of incoming lanes (into a junction) */
    SUMO_TAG_INCOMING_LANES,
    /** begin/end of the list of internal lanes (of a junction) */
    SUMO_TAG_INTERNAL_LANES,
    /// Incoming edge specification (jp-router)
    SUMO_TAG_FROMEDGE,
    /// Outgoing edge specification (jp-router)
    SUMO_TAG_TOEDGE,
    /// Sink(s) specification
    SUMO_TAG_SINK
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
    SUMO_ATTR_END,
    /** link: the traffic light id responsible for this link */
    SUMO_ATTR_TLID,
    /** link: the index of the link within the traffic light */
    SUMO_ATTR_TLLINKNO,
    /** edge: the shape in xml-definition */
    SUMO_ATTR_SHAPE ,
    /// The information about how to spread the lanes from the given position
    SUMO_ATTR_SPREADFUNC,
    /// A color information
    SUMO_ATTR_COLOR,
    /// The abstract direction of a link
    SUMO_ATTR_DIR,
    /// The state of a link
    SUMO_ATTR_STATE,

    SUMO_ATTR_DEST,
    SUMO_ATTR_SOURCE,
    SUMO_ATTR_VIA,
    SUMO_ATTR_INTERNALEND,
    // Attributes for actuated traffic lights:
        /// minimum duration of a phase
    SUMO_ATTR_MINDURATION,
        /// maximum duration of a phase
    SUMO_ATTR_MAXDURATION,
    // Attributes for junction-internal lanes
        /// Information within the junction logic which internal lanes block external
    SUMO_ATTR_FOES,
    // Attributes for detectors
        /// Measures to retrieve
    SUMO_ATTR_MEASURES,
        /// Information whether the detector shall be continued on the folowing lanes
    SUMO_ATTR_CONT,
    SUMO_ATTR_HALTING_TIME_THRESHHOLD,
    SUMO_ATTR_HALTING_SPEED_THRESHHOLD,
    SUMO_ATTR_JAM_DIST_THRESHHOLD,
    SUMO_ATTR_DELETE_DATA_AFTER_SECONDS,
    SUMO_ATTR_DET_OFFSET,
    /// The number of vehicles to emit (od-amount
    SUMO_ATTR_VEHNO,
    /// The percentage of something
    SUMO_ATTR_PERC
};


/* =========================================================================
 * declarations
 * ======================================================================= */
/// The number of SUMO-XML element names
extern size_t noSumoTags;
/// The names of SUMO-XML elements
extern GenericSAX2Handler::Tag sumotags[48];

/// The number of SUMO-XML attributes
extern size_t noSumoAttrs;
/// The names of SUMO-XML attributes
extern AttributesHandler::Attr sumoattrs[78];


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

