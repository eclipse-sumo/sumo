//---------------------------------------------------------------------------//
//                        SUMOXMLDefinitions.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
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
// Revision 1.5  2003/03/03 15:27:00  dkrajzew
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
#include "SUMOXMLDefinitions.h"

/* =========================================================================
 * definitions
 * ======================================================================= */
size_t noSumoTags = 38;

GenericSAX2Handler::Tag sumotags[38] =
{
      { "simulation",       SUMO_TAG_SIMULATION },
      { "edge",             SUMO_TAG_EDGE },
      { "lane",             SUMO_TAG_LANE },
      { "lanes",            SUMO_TAG_LANES },
      { "cedge",            SUMO_TAG_CEDGE },
      { "junction",         SUMO_TAG_JUNCTION },
      { "inlanes",          SUMO_TAG_INLANES },
      { "detector",         SUMO_TAG_DETECTOR },
      { "vehicle",          SUMO_TAG_VEHICLE },
      { "vtype",            SUMO_TAG_VTYPE },
      { "route",            SUMO_TAG_ROUTE },
      { "succ",             SUMO_TAG_SUCC },
      { "succlane",         SUMO_TAG_SUCCLANE },
      { "key",              SUMO_TAG_KEY },
      { "junctionlogic",    SUMO_TAG_JUNCTIONLOGIC },
      { "requestsize",      SUMO_TAG_REQUESTSIZE },
      { "lanenumber",       SUMO_TAG_LANENUMBER },
      { "logicitem",        SUMO_TAG_LOGICITEM },
      { "row-logic",        SUMO_TAG_ROWLOGIC },
      { "source",           SUMO_TAG_SOURCE },
      { "district",         SUMO_TAG_DISTRICT },
      { "dsource",          SUMO_TAG_DSOURCE },
      { "dsink",            SUMO_TAG_DSINK },
      { "edges",            SUMO_TAG_EDGES },
      { "node_count",       SUMO_TAG_NODECOUNT },
      { "tl-logic",         SUMO_TAG_TLLOGIC },
      { "offset",           SUMO_TAG_OFFSET },
      { "initstep",         SUMO_TAG_INITSTEP },
      { "phaseno",          SUMO_TAG_PHASENO },
      { "phase",            SUMO_TAG_PHASE },
      { "edgepos",          SUMO_TAG_EDGEPOS },
      { "routealt",         SUMO_TAG_ROUTEALT },
      { "tripdef",          SUMO_TAG_TRIPDEF },
      { "logicno",          SUMO_TAG_LOGICNO },
      { "trigger",          SUMO_TAG_TRIGGER },
      { "step",             SUMO_TAG_STEP },
      { "interval",         SUMO_TAG_INTERVAL },
      { "shape",            SUMO_TAG_SHAPE }
};

size_t noSumoAttrs = 59;

AttributesHandler::Attr sumoattrs[59] =
{
	{ "id",             SUMO_ATTR_ID },
    { "name",           SUMO_ATTR_NAME },
    { "type",           SUMO_ATTR_TYPE },
    { "priority",       SUMO_ATTR_PRIORITY },
    { "nolanes",        SUMO_ATTR_NOLANES },
    { "speed",          SUMO_ATTR_SPEED },
    { "length",         SUMO_ATTR_LENGTH },
    { "fromnode",       SUMO_ATTR_FROMNODE },
    { "tonode",         SUMO_ATTR_TONODE },
    { "xfrom",          SUMO_ATTR_XFROM },
    { "yfrom",          SUMO_ATTR_YFROM },
    { "xto",            SUMO_ATTR_XTO },
    { "yto",            SUMO_ATTR_YTO },
    { "x",              SUMO_ATTR_X },
    { "y",              SUMO_ATTR_Y },
    { "key",            SUMO_ATTR_KEY },
    { "weight",         SUMO_ATTR_WEIGHT },
    { "depart",         SUMO_ATTR_DEPART },
    { "route",          SUMO_ATTR_ROUTE },
    { "maxspeed",       SUMO_ATTR_MAXSPEED },
    { "accel",          SUMO_ATTR_ACCEL },
    { "decel",          SUMO_ATTR_DECEL },
    { "sigma",          SUMO_ATTR_SIGMA },
    { "last",           SUMO_ATTR_LAST },
    { "cost",           SUMO_ATTR_COST },
    { "propability",    SUMO_ATTR_PROP },
    { "pos",            SUMO_ATTR_POS },
    { "lane",           SUMO_ATTR_LANE },
    { "from",           SUMO_ATTR_FROM },
    { "to",             SUMO_ATTR_TO },
    { "function",       SUMO_ATTR_FUNC },
    { "changeurge",     SUMO_ATTR_CHANGEURGE },
    { "request",        SUMO_ATTR_REQUEST },
    { "response",       SUMO_ATTR_RESPONSE },
    { "pos",            SUMO_ATTR_POSITION },
    { "freq",           SUMO_ATTR_SPLINTERVAL },
    { "style",          SUMO_ATTR_STYLE },
    { "file",           SUMO_ATTR_FILE },
    { "junction",       SUMO_ATTR_JUNCTION },
    { "yield",          SUMO_ATTR_YIELD },
    { "no",             SUMO_ATTR_NO },
    { "phase",          SUMO_ATTR_PHASE },
    { "brake",          SUMO_ATTR_BRAKE },
    { "yellow",         SUMO_ATTR_YELLOW },
    { "duration",       SUMO_ATTR_DURATION },
    { "objecttype",     SUMO_ATTR_OBJECTTYPE },
    { "attr",           SUMO_ATTR_ATTR },
    { "objectid",       SUMO_ATTR_OBJECTID },
    { "time",           SUMO_ATTR_TIME },
    { "multi_ref",      SUMO_ATTR_MULTIR },
    { "traveltime",     SUMO_ATTR_VALUE },
    { "begin",          SUMO_ATTR_BEGIN },
    { "end",            SUMO_ATTR_END },
    { "period",         SUMO_ATTR_PERIOD },
    { "repno",          SUMO_ATTR_REPNUMBER },
    { "tl",             SUMO_ATTR_TLID },
    { "linkno",         SUMO_ATTR_TLLINKNO },
    { "shape",          SUMO_ATTR_SHAPE },
    { "spread_type",    SUMO_ATTR_SPREADFUNC }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "SUMOXMLDefinitions.icc"
//#endif

// Local Variables:
// mode:C++
// End:


