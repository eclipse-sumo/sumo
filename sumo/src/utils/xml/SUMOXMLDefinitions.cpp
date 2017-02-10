/****************************************************************************/
/// @file    SUMOXMLDefinitions.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Clemens Honomichl
/// @author  Piotr Woznica
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Sept 2002
/// @version $Id$
///
// Definitions of elements and attributes known by SUMO
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include "SUMOXMLDefinitions.h"
#include <utils/common/StringBijection.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definitions
// ===========================================================================

StringBijection<int>::Entry SUMOXMLDefinitions::tags[] = {
    // Simulation elements
    { "net",                        SUMO_TAG_NET },
    { "edge",                       SUMO_TAG_EDGE },
    { "lane",                       SUMO_TAG_LANE },
    { "neigh",                      SUMO_TAG_NEIGH },
    { "poi",                        SUMO_TAG_POI },
    { "poly",                       SUMO_TAG_POLY },
    { "junction",                   SUMO_TAG_JUNCTION },
    { "restriction",                SUMO_TAG_RESTRICTION },
    { "e1Detector",                 SUMO_TAG_E1DETECTOR },
    { "inductionLoop",              SUMO_TAG_INDUCTION_LOOP },
    { "e2Detector",                 SUMO_TAG_E2DETECTOR },
    { "laneAreaDetector",           SUMO_TAG_LANE_AREA_DETECTOR },
    { "e3Detector",                 SUMO_TAG_E3DETECTOR },
    { "entryExitDetector",          SUMO_TAG_ENTRY_EXIT_DETECTOR },
    { "edgeData",                   SUMO_TAG_MEANDATA_EDGE },
    { "laneData",                   SUMO_TAG_MEANDATA_LANE },
    { "detEntry",                   SUMO_TAG_DET_ENTRY },
    { "detExit",                    SUMO_TAG_DET_EXIT },
    { "edgeFollowDetector",         SUMO_TAG_EDGEFOLLOWDETECTOR },
    { "instantInductionLoop",       SUMO_TAG_INSTANT_INDUCTION_LOOP },
    { "variableSpeedSign",          SUMO_TAG_VSS },
    { "calibrator",                 SUMO_TAG_CALIBRATOR },
    { "rerouter",                   SUMO_TAG_REROUTER },
    { "busStop",                    SUMO_TAG_BUS_STOP },
    { "trainStop",                  SUMO_TAG_TRAIN_STOP },
    { "access",                     SUMO_TAG_ACCESS },
    { "containerStop",              SUMO_TAG_CONTAINER_STOP },
    { "parkingArea",                SUMO_TAG_PARKING_AREA },
    { "space",                      SUMO_TAG_PARKING_SPACE },
    { "chargingStation",            SUMO_TAG_CHARGING_STATION },
    { "vTypeProbe",                 SUMO_TAG_VTYPEPROBE },
    { "routeProbe",                 SUMO_TAG_ROUTEPROBE },
    { "routes",                     SUMO_TAG_ROUTES },
    { "vehicle",                    SUMO_TAG_VEHICLE },
    { "vType",                      SUMO_TAG_VTYPE },
    { "route",                      SUMO_TAG_ROUTE },
    { "request",                    SUMO_TAG_REQUEST },
    { "source",                     SUMO_TAG_SOURCE },
    { "taz",                        SUMO_TAG_TAZ },
    { "tazSource",                  SUMO_TAG_TAZSOURCE },
    { "tazSink",                    SUMO_TAG_TAZSINK },
    { "trafficLight",               SUMO_TAG_TRAFFIC_LIGHT },
    { "tlLogic",                    SUMO_TAG_TLLOGIC },
    { "phase",                      SUMO_TAG_PHASE },
    { "trip",                       SUMO_TAG_TRIP },
    { "flow",                       SUMO_TAG_FLOW },
    { "step",                       SUMO_TAG_STEP },
    { "interval",                   SUMO_TAG_INTERVAL },
    { "timedEvent",                 SUMO_TAG_TIMEDEVENT },
    { "fromEdge",                   SUMO_TAG_FROMEDGE },
    { "toEdge",                     SUMO_TAG_TOEDGE },
    { "sink",                       SUMO_TAG_SINK },
    { "param",                      SUMO_TAG_PARAM },
    { "WAUT",                       SUMO_TAG_WAUT },
    { "wautSwitch",                 SUMO_TAG_WAUT_SWITCH },
    { "wautJunction",               SUMO_TAG_WAUT_JUNCTION },
    { "segment",                    SUMO_TAG_SEGMENT },
    { "delete",                     SUMO_TAG_DELETE },
    { "stop",                       SUMO_TAG_STOP },
    { "destProbReroute",            SUMO_TAG_DEST_PROB_REROUTE },
    { "closingReroute",             SUMO_TAG_CLOSING_REROUTE },
    { "closingLaneReroute",         SUMO_TAG_CLOSING_LANE_REROUTE },
    { "routeProbReroute",           SUMO_TAG_ROUTE_PROB_REROUTE },
    { "parkingZoneReroute",         SUMO_TAG_PARKING_ZONE_REROUTE },
    { "polygonType",                SUMO_TAG_POLYTYPE },
    { "connection",                 SUMO_TAG_CONNECTION },
    { "prohibition",                SUMO_TAG_PROHIBITION },
    { "split",                      SUMO_TAG_SPLIT },
    { "node",                       SUMO_TAG_NODE },
    { "type",                       SUMO_TAG_TYPE },
    { "detectorDefinition",         SUMO_TAG_DETECTOR_DEFINITION },
    { "routeDistribution",          SUMO_TAG_ROUTE_DISTRIBUTION },
    { "vTypeDistribution",          SUMO_TAG_VTYPE_DISTRIBUTION },
    { "vaporizer",                  SUMO_TAG_VAPORIZER },
    { "roundabout",                 SUMO_TAG_ROUNDABOUT },
    { "join",                       SUMO_TAG_JOIN },
    { "joinExclude",                SUMO_TAG_JOINEXCLUDE },
    { "crossing",                   SUMO_TAG_CROSSING },
    { "customShape",                SUMO_TAG_CUSTOMSHAPE },
    // OSM
    { "way",                        SUMO_TAG_WAY },
    { "nd",                         SUMO_TAG_ND },
    { "tag",                        SUMO_TAG_TAG },
    { "relation",                   SUMO_TAG_RELATION },
    { "member",                     SUMO_TAG_MEMBER },
    // View
    { "viewsettings",               SUMO_TAG_VIEWSETTINGS },
    { "decal",                      SUMO_TAG_VIEWSETTINGS_DECAL },
    { "light",                      SUMO_TAG_VIEWSETTINGS_LIGHT },
    { "scheme",                     SUMO_TAG_VIEWSETTINGS_SCHEME },
    { "opengl",                     SUMO_TAG_VIEWSETTINGS_OPENGL },
    { "background",                 SUMO_TAG_VIEWSETTINGS_BACKGROUND },
    { "edges",                      SUMO_TAG_VIEWSETTINGS_EDGES },
    { "vehicles",                   SUMO_TAG_VIEWSETTINGS_VEHICLES },
    { "persons",                    SUMO_TAG_VIEWSETTINGS_PERSONS },
    { "containers",                 SUMO_TAG_VIEWSETTINGS_CONTAINERS },
    { "junctions",                  SUMO_TAG_VIEWSETTINGS_JUNCTIONS },
    { "additionals",                SUMO_TAG_VIEWSETTINGS_ADDITIONALS },
    { "pois",                       SUMO_TAG_VIEWSETTINGS_POIS },
    { "polys",                      SUMO_TAG_VIEWSETTINGS_POLYS },
    { "legend",                     SUMO_TAG_VIEWSETTINGS_LEGEND },
    { "event",                      SUMO_TAG_VIEWSETTINGS_EVENT },
    { "jamTime",                    SUMO_TAG_VIEWSETTINGS_EVENT_JAM_TIME },
    { "include",                    SUMO_TAG_INCLUDE },
    { "delay",                      SUMO_TAG_DELAY },
    { "viewport",                   SUMO_TAG_VIEWPORT },
    { "snapshot",                   SUMO_TAG_SNAPSHOT },
    { "breakpoints-file",           SUMO_TAG_BREAKPOINTS_FILE },
    { "location",                   SUMO_TAG_LOCATION },
    { "colorScheme",                SUMO_TAG_COLORSCHEME },
    { "scalingScheme",              SUMO_TAG_SCALINGSCHEME },
    { "entry",                      SUMO_TAG_ENTRY },
    { "vehicleTransfer",            SUMO_TAG_VEHICLETRANSFER },
    // Cars
    { "carFollowing-IDM",           SUMO_TAG_CF_IDM },
    { "carFollowing-IDMM",          SUMO_TAG_CF_IDMM },
    { "carFollowing-Krauss",        SUMO_TAG_CF_KRAUSS },
    { "carFollowing-KraussPS",      SUMO_TAG_CF_KRAUSS_PLUS_SLOPE },
    { "carFollowing-KraussOrig1",   SUMO_TAG_CF_KRAUSS_ORIG1 },
    { "carFollowing-SmartSK",       SUMO_TAG_CF_SMART_SK },
    { "carFollowing-Daniel1",       SUMO_TAG_CF_DANIEL1 },
    { "carFollowing-PWagner2009",   SUMO_TAG_CF_PWAGNER2009 },
    { "carFollowing-BKerner",       SUMO_TAG_CF_BKERNER },
    { "carFollowing-Wiedemann",     SUMO_TAG_CF_WIEDEMANN },
    // Person
    { "person",                     SUMO_TAG_PERSON },
    { "personTrip",                 SUMO_TAG_PERSONTRIP },
    { "ride",                       SUMO_TAG_RIDE },
    { "walk",                       SUMO_TAG_WALK },
    // Transport
    { "container",                  SUMO_TAG_CONTAINER },
    { "transport",                  SUMO_TAG_TRANSPORT },
    { "tranship",                   SUMO_TAG_TRANSHIP },
    //Trajectories
    { "trajectories",               SUMO_TAG_TRAJECTORIES },
    { "timestep",                   SUMO_TAG_TIMESTEP },
    { "timeSlice",                  SUMO_TAG_TIMESLICE },
    { "actorConfig",                SUMO_TAG_ACTORCONFIG },
    { "motionState",                SUMO_TAG_MOTIONSTATE },
    { "odPair",                     SUMO_TAG_OD_PAIR },
    // ActivityGen statistics file
    { "general",                    AGEN_TAG_GENERAL },
    { "street",                     AGEN_TAG_STREET },
    { "workHours",                  AGEN_TAG_WORKHOURS },
    { "opening",                    AGEN_TAG_OPENING },
    { "closing",                    AGEN_TAG_CLOSING },
    { "schools",                    AGEN_TAG_SCHOOLS },
    { "school",                     AGEN_TAG_SCHOOL },
    { "busStation",                 AGEN_TAG_BUSSTATION },
    { "busLine",                    AGEN_TAG_BUSLINE },
    { "stations",                   AGEN_TAG_STATIONS },
    { "revStations",                AGEN_TAG_REV_STATIONS },
    { "station",                    AGEN_TAG_STATION },
    { "frequency",                  AGEN_TAG_FREQUENCY },
    { "population",                 AGEN_TAG_POPULATION },
    { "bracket",                    AGEN_TAG_BRACKET },
    { "cityGates",                  AGEN_TAG_CITYGATES },
    { "entrance",                   AGEN_TAG_ENTRANCE },
    { "parameters",                 AGEN_TAG_PARAM },
    // Other
    { "",                           SUMO_TAG_NOTHING } //< must be the last one
};


StringBijection<int>::Entry SUMOXMLDefinitions::attrs[] = {
    // Edge
    { "id",                     SUMO_ATTR_ID },
    { "refId",                  SUMO_ATTR_REFID },
    { "name",                   SUMO_ATTR_NAME },
    { "type",                   SUMO_ATTR_TYPE },
    { "priority",               SUMO_ATTR_PRIORITY },
    { "numLanes",               SUMO_ATTR_NUMLANES },
    { "speed",                  SUMO_ATTR_SPEED },
    { "oneway",                 SUMO_ATTR_ONEWAY },
    { "width",                  SUMO_ATTR_WIDTH },
    { "sidewalkWidth",          SUMO_ATTR_SIDEWALKWIDTH },
    { "bikeLaneWidth",          SUMO_ATTR_BIKELANEWIDTH },
    { "remove",                 SUMO_ATTR_REMOVE },
    { "length",                 SUMO_ATTR_LENGTH },
    // Split
    { "idBefore",               SUMO_ATTR_ID_BEFORE },
    { "idAfter",                SUMO_ATTR_ID_AFTER },
    // Positions
    { "x",                      SUMO_ATTR_X },
    { "y",                      SUMO_ATTR_Y },
    { "z",                      SUMO_ATTR_Z },
    { "centerX",                SUMO_ATTR_CENTER_X },
    { "centerY",                SUMO_ATTR_CENTER_Y },
    { "centerZ",                SUMO_ATTR_CENTER_Z },

    { "key",                    SUMO_ATTR_KEY },
    { "requestSize",            SUMO_ATTR_REQUESTSIZE },
    { "request",                SUMO_ATTR_REQUEST },
    { "response",               SUMO_ATTR_RESPONSE },
    { "programID",              SUMO_ATTR_PROGRAMID },
    { "offset",                 SUMO_ATTR_OFFSET },
    { "endOffset",              SUMO_ATTR_ENDOFFSET },
    { "incLanes",               SUMO_ATTR_INCLANES },
    { "intLanes",               SUMO_ATTR_INTLANES },

    { "weight",                 SUMO_ATTR_WEIGHT },
    { "node",                   SUMO_ATTR_NODE },
    { "edge",                   SUMO_ATTR_EDGE },
    { "edges",                  SUMO_ATTR_EDGES },
    // Vehicle
    { "depart",                 SUMO_ATTR_DEPART },
    { "departLane",             SUMO_ATTR_DEPARTLANE },
    { "departPos",              SUMO_ATTR_DEPARTPOS },
    { "departPosLat",           SUMO_ATTR_DEPARTPOS_LAT },
    { "departSpeed",            SUMO_ATTR_DEPARTSPEED },
    { "arrivalLane",            SUMO_ATTR_ARRIVALLANE },
    { "arrivalPos",             SUMO_ATTR_ARRIVALPOS },
    { "arrivalPosLat",          SUMO_ATTR_ARRIVALPOS_LAT },
    { "arrivalSpeed",           SUMO_ATTR_ARRIVALSPEED },
    { "route",                  SUMO_ATTR_ROUTE },
    { "maxSpeed",               SUMO_ATTR_MAXSPEED },
    { "maxSpeedLat",            SUMO_ATTR_MAXSPEED_LAT },
    { "latAlignment",           SUMO_ATTR_LATALIGNMENT },
    { "minGapLat",              SUMO_ATTR_MINGAP_LAT },
    { "accel",                  SUMO_ATTR_ACCEL },
    { "decel",                  SUMO_ATTR_DECEL },
    { "vClass",                 SUMO_ATTR_VCLASS },
    { "repno",                  SUMO_ATTR_REPNUMBER },
    { "speedFactor",            SUMO_ATTR_SPEEDFACTOR },
    { "speedDev",               SUMO_ATTR_SPEEDDEV },
    { "laneChangeModel",        SUMO_ATTR_LANE_CHANGE_MODEL },
    { "carFollowModel",         SUMO_ATTR_CAR_FOLLOW_MODEL },
    { "minGap",                 SUMO_ATTR_MINGAP },
    { "boardingDuration",       SUMO_ATTR_BOARDING_DURATION },
    { "loadingDuration",        SUMO_ATTR_LOADING_DURATION },
    // Charging Station
    { "power",                  SUMO_ATTR_CHARGINGPOWER },
    { "efficiency",             SUMO_ATTR_EFFICIENCY },
    { "chargeInTransit",        SUMO_ATTR_CHARGEINTRANSIT },
    { "chargeDelay",            SUMO_ATTR_CHARGEDELAY},

    { "sigma",                  SUMO_ATTR_SIGMA },
    { "tau",                    SUMO_ATTR_TAU },
    { "tmp1",                   SUMO_ATTR_TMP1 },
    { "tmp2",                   SUMO_ATTR_TMP2 },
    { "tmp3",                   SUMO_ATTR_TMP3 },
    { "tmp4",                   SUMO_ATTR_TMP4 },
    { "tmp5",                   SUMO_ATTR_TMP5 },

    { "lcStrategic",            SUMO_ATTR_LCA_STRATEGIC_PARAM },
    { "lcCooperative",          SUMO_ATTR_LCA_COOPERATIVE_PARAM },
    { "lcSpeedGain",            SUMO_ATTR_LCA_SPEEDGAIN_PARAM },
    { "lcKeepRight",            SUMO_ATTR_LCA_KEEPRIGHT_PARAM },
    { "lcSublane",              SUMO_ATTR_LCA_SUBLANE_PARAM },
    { "lcPushy",                SUMO_ATTR_LCA_PUSHY },
    { "lcAssertive",            SUMO_ATTR_LCA_ASSERTIVE },

    { "last",                   SUMO_ATTR_LAST },
    { "cost",                   SUMO_ATTR_COST },
    { "probability",            SUMO_ATTR_PROB },
    { "probabilities",          SUMO_ATTR_PROBS },
    { "routes",                 SUMO_ATTR_ROUTES },
    { "vTypes",                 SUMO_ATTR_VTYPES },

    { "lane",                   SUMO_ATTR_LANE },
    { "lanes",                  SUMO_ATTR_LANES },
    { "from",                   SUMO_ATTR_FROM },
    { "to",                     SUMO_ATTR_TO },
    { "period",                 SUMO_ATTR_PERIOD },
    { "fromTaz",                SUMO_ATTR_FROM_TAZ },
    { "toTaz",                  SUMO_ATTR_TO_TAZ },
    { "reroute",                SUMO_ATTR_REROUTE },
    { "personCapacity",         SUMO_ATTR_PERSON_CAPACITY },
    { "containerCapacity",      SUMO_ATTR_CONTAINER_CAPACITY },
    { "personNumber",           SUMO_ATTR_PERSON_NUMBER },
    { "containerNumber",        SUMO_ATTR_CONTAINER_NUMBER },
    { "modes",                  SUMO_ATTR_MODES },
    { "walkFactor",             SUMO_ATTR_WALKFACTOR },

    { "function",               SUMO_ATTR_FUNCTION },
    { "pos",                    SUMO_ATTR_POSITION },
    { "posLat",                 SUMO_ATTR_POSITION_LAT },
    { "freq",                   SUMO_ATTR_FREQUENCY },
    { "style",                  SUMO_ATTR_STYLE },
    { "file",                   SUMO_ATTR_FILE },
    { "junction",               SUMO_ATTR_JUNCTION },
    { "number",                 SUMO_ATTR_NUMBER },
    { "duration",               SUMO_ATTR_DURATION },
    { "until",                  SUMO_ATTR_UNTIL },
    { "routeProbe",             SUMO_ATTR_ROUTEPROBE },
    { "crossingEdges",          SUMO_ATTR_CROSSING_EDGES },
    // Traffic light & Nodes
    { "time",                   SUMO_ATTR_TIME },
    { "begin",                  SUMO_ATTR_BEGIN },
    { "end",                    SUMO_ATTR_END },
    { "tl",                     SUMO_ATTR_TLID },
    { "tlType",                 SUMO_ATTR_TLTYPE },
    { "linkIndex",              SUMO_ATTR_TLLINKINDEX },
    { "shape",                  SUMO_ATTR_SHAPE },
    { "spreadType",             SUMO_ATTR_SPREADTYPE },
    { "radius",                 SUMO_ATTR_RADIUS },
    { "customShape",            SUMO_ATTR_CUSTOMSHAPE },
    { "keepClear",              SUMO_ATTR_KEEP_CLEAR },
    { "color",                  SUMO_ATTR_COLOR },
    { "dir",                    SUMO_ATTR_DIR },
    { "state",                  SUMO_ATTR_STATE },
    { "layer",                  SUMO_ATTR_LAYER },
    { "fill",                   SUMO_ATTR_FILL },
    { "prefix",                 SUMO_ATTR_PREFIX },
    { "discard",                SUMO_ATTR_DISCARD },

    { "fromLane",               SUMO_ATTR_FROM_LANE },
    { "toLane",                 SUMO_ATTR_TO_LANE },
    { "dest",                   SUMO_ATTR_DEST },
    { "source",                 SUMO_ATTR_SOURCE },
    { "via",                    SUMO_ATTR_VIA },
    { "nodes",                  SUMO_ATTR_NODES },
    { "visibility",             SUMO_ATTR_VISIBILITY_DISTANCE },

    { "minDur",                 SUMO_ATTR_MINDURATION },
    { "maxDur",                 SUMO_ATTR_MAXDURATION },
    { "foes",                   SUMO_ATTR_FOES },
    // E2 detector
    { "cont",                   SUMO_ATTR_CONT },
    { "contPos",                SUMO_ATTR_CONTPOS },
    { "timeThreshold",          SUMO_ATTR_HALTING_TIME_THRESHOLD },
    { "speedThreshold",         SUMO_ATTR_HALTING_SPEED_THRESHOLD },
    { "jamThreshold",           SUMO_ATTR_JAM_DIST_THRESHOLD },

    { "wautID",                 SUMO_ATTR_WAUT_ID },
    { "junctionID",             SUMO_ATTR_JUNCTION_ID },
    { "procedure",              SUMO_ATTR_PROCEDURE },
    { "synchron",               SUMO_ATTR_SYNCHRON },
    { "refTime",                SUMO_ATTR_REF_TIME },
    { "startProg",              SUMO_ATTR_START_PROG },
    { "off",                    SUMO_ATTR_OFF },
    { "friendlyPos",            SUMO_ATTR_FRIENDLY_POS },
    { "splitByType",            SUMO_ATTR_SPLIT_VTYPE },
    { "uncontrolled",           SUMO_ATTR_UNCONTROLLED },
    { "pass",                   SUMO_ATTR_PASS },
    { "busStop",                SUMO_ATTR_BUS_STOP },
    { "containerStop",          SUMO_ATTR_CONTAINER_STOP },
    { "parkingArea",            SUMO_ATTR_PARKING_AREA },
    { "roadsideCapacity",       SUMO_ATTR_ROADSIDE_CAPACITY },
    { "chargingStation",        SUMO_ATTR_CHARGING_STATION},
    { "line",                   SUMO_ATTR_LINE },
    { "lines",                  SUMO_ATTR_LINES },
    { "value",                  SUMO_ATTR_VALUE },
    { "prohibitor",             SUMO_ATTR_PROHIBITOR },
    { "prohibited",             SUMO_ATTR_PROHIBITED },
    { "allow",                  SUMO_ATTR_ALLOW },
    { "disallow",               SUMO_ATTR_DISALLOW },
    { "prefer",                 SUMO_ATTR_PREFER },
    { "controlledInner",        SUMO_ATTR_CONTROLLED_INNER },
    { "vehsPerHour",            SUMO_ATTR_VEHSPERHOUR },
    { "output",                 SUMO_ATTR_OUTPUT },
    { "height",                 SUMO_ATTR_HEIGHT },
    { "guiShape",               SUMO_ATTR_GUISHAPE },
    { "osgFile",                SUMO_ATTR_OSGFILE },
    { "imgFile",                SUMO_ATTR_IMGFILE },
    { "angle",                  SUMO_ATTR_ANGLE },
    { "emissionClass",          SUMO_ATTR_EMISSIONCLASS },
    { "impatience",             SUMO_ATTR_IMPATIENCE },
    { "startPos",               SUMO_ATTR_STARTPOS },
    { "endPos",                 SUMO_ATTR_ENDPOS },
    { "triggered",              SUMO_ATTR_TRIGGERED },
    { "containerTriggered",     SUMO_ATTR_CONTAINER_TRIGGERED },
    { "parking",                SUMO_ATTR_PARKING },
    { "expected",               SUMO_ATTR_EXPECTED },
    { "expectedContainers",     SUMO_ATTR_EXPECTED_CONTAINERS },
    { "index",                  SUMO_ATTR_INDEX },

    { "entering",               SUMO_ATTR_ENTERING },
    { "excludeEmpty",           SUMO_ATTR_EXCLUDE_EMPTY },
    { "withInternal",           SUMO_ATTR_WITH_INTERNAL },
    { "trackVehicles",          SUMO_ATTR_TRACK_VEHICLES },
    { "maxTraveltime",          SUMO_ATTR_MAX_TRAVELTIME },
    { "minSamples",             SUMO_ATTR_MIN_SAMPLES },

    { "lon",                    SUMO_ATTR_LON },
    { "lat",                    SUMO_ATTR_LAT },
    { "geo",                    SUMO_ATTR_GEO },
    { "k",                      SUMO_ATTR_K },
    { "v",                      SUMO_ATTR_V },
    { "ref",                    SUMO_ATTR_REF },
    { "href",                   SUMO_ATTR_HREF },
    { "zoom",                   SUMO_ATTR_ZOOM },
    { "interpolated",           SUMO_ATTR_INTERPOLATED },
    { "threshold",              SUMO_ATTR_THRESHOLD },

    { "netOffset",              SUMO_ATTR_NET_OFFSET },
    { "convBoundary",           SUMO_ATTR_CONV_BOUNDARY },
    { "origBoundary",           SUMO_ATTR_ORIG_BOUNDARY },
    { "projParameter",          SUMO_ATTR_ORIG_PROJ },

    { "tauLast",                SUMO_ATTR_CF_PWAGNER2009_TAULAST },
    { "apProb",                 SUMO_ATTR_CF_PWAGNER2009_APPROB },
    { "delta",                  SUMO_ATTR_CF_IDM_DELTA },
    { "stepping",               SUMO_ATTR_CF_IDM_STEPPING },
    { "adaptFactor",            SUMO_ATTR_CF_IDMM_ADAPT_FACTOR },
    { "adaptTime",              SUMO_ATTR_CF_IDMM_ADAPT_TIME },
    { "phi",                    SUMO_ATTR_CF_KERNER_PHI },

    { "security",               SUMO_ATTR_CF_WIEDEMANN_SECURITY },
    { "estimation",             SUMO_ATTR_CF_WIEDEMANN_ESTIMATION },

    { "generateWalks",          SUMO_ATTR_GENERATE_WALKS },
    { "actType",                SUMO_ATTR_ACTTYPE },
    { "slope",                  SUMO_ATTR_SLOPE },
    { "version",                SUMO_ATTR_VERSION },
    { "junctionCornerDetail",   SUMO_ATTR_CORNERDETAIL },
    { "junctionLinkDetail",     SUMO_ATTR_LINKDETAIL },
    { "lefthand",               SUMO_ATTR_LEFTHAND },

    { "actorConfig",            SUMO_ATTR_ACTORCONFIG },
    { "vehicle",                SUMO_ATTR_VEHICLE },
    { "startTime",              SUMO_ATTR_STARTTIME },
    { "vehicleClass",           SUMO_ATTR_VEHICLECLASS },
    { "fuel",                   SUMO_ATTR_FUEL },
    { "acceleration",           SUMO_ATTR_ACCELERATION },
    { "amount",                 SUMO_ATTR_AMOUNT },
    { "origin",                 SUMO_ATTR_ORIGIN },
    { "destination",            SUMO_ATTR_DESTINATION },

#ifndef WIN32
    { "commandPosix",   SUMO_ATTR_COMMAND },
#else
    { "commandWindows", SUMO_ATTR_COMMAND },
#endif

    // ActivityGen statistics file
    { "inhabitants",            AGEN_ATTR_INHABITANTS },
    { "households",             AGEN_ATTR_HOUSEHOLDS },
    { "childrenAgeLimit",       AGEN_ATTR_CHILDREN },
    { "retirementAgeLimit",     AGEN_ATTR_RETIREMENT },
    { "carRate",                AGEN_ATTR_CARS },
    { "unemploymentRate",       AGEN_ATTR_UNEMPLOYEMENT },
    { "footDistanceLimit",      AGEN_ATTR_MAX_FOOT_DIST },
    { "incomingTraffic",        AGEN_ATTR_IN_TRAFFIC },
    { "incoming",               AGEN_ATTR_INCOMING },
    { "outgoingTraffic",        AGEN_ATTR_OUT_TRAFFIC },
    { "outgoing",               AGEN_ATTR_OUTGOING },
    { "population",             AGEN_ATTR_POPULATION },
    { "workPosition",           AGEN_ATTR_OUT_WORKPOSITION },
    { "hour",                   AGEN_ATTR_HOUR },
    { "proportion",             AGEN_ATTR_PROP },
    { "capacity",               AGEN_ATTR_CAPACITY },
    { "opening",                AGEN_ATTR_OPENING },
    { "closing",                AGEN_ATTR_CLOSING },
    { "maxTripDuration",        AGEN_ATTR_MAX_TRIP_DURATION },
    { "rate",                   AGEN_ATTR_RATE },
    { "beginAge",               AGEN_ATTR_BEGINAGE },
    { "endAge",                 AGEN_ATTR_ENDAGE },
    { "peopleNbr",              AGEN_ATTR_PEOPLENBR },
    { "carPreference",          AGEN_ATTR_CARPREF },
    { "meanTimePerKmInCity",    AGEN_ATTR_CITYSPEED },
    { "freeTimeActivityRate",   AGEN_ATTR_FREETIMERATE },
    { "uniformRandomTraffic",   AGEN_ATTR_UNI_RAND_TRAFFIC },
    { "departureVariation",     AGEN_ATTR_DEP_VARIATION },

    // netEdit
    { "modificationStatusNotForPrinting",   GNE_ATTR_MODIFICATION_STATUS },
    { "shapeStartNotForPrinting",           GNE_ATTR_SHAPE_START },
    { "shapeEndNotForPrinting",             GNE_ATTR_SHAPE_END },
    { "blockMovement",                      GNE_ATTR_BLOCK_MOVEMENT },
    { "parentOfAdditional",                 GNE_ATTR_PARENT },

    { "targetLanes", SUMO_ATTR_TARGETLANE },
    { "crossing", SUMO_ATTR_CROSSING },

    { "xmlns:xsi", SUMO_ATTR_XMLNS },
    { "xsi:noNamespaceSchemaLocation", SUMO_ATTR_SCHEMA_LOCATION },

    // Other
    { "",                       SUMO_ATTR_NOTHING } //< must be the last one
};


StringBijection<SumoXMLNodeType>::Entry SUMOXMLDefinitions::sumoNodeTypeValues[] = {
    {"traffic_light",               NODETYPE_TRAFFIC_LIGHT},
    {"traffic_light_unregulated",   NODETYPE_TRAFFIC_LIGHT_NOJUNCTION},
    {"traffic_light_right_on_red",  NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED},
    {"rail_signal",                 NODETYPE_RAIL_SIGNAL},
    {"rail_crossing",               NODETYPE_RAIL_CROSSING},
    {"priority",                    NODETYPE_PRIORITY},
    {"priority_stop",               NODETYPE_PRIORITY_STOP},
    {"right_before_left",           NODETYPE_RIGHT_BEFORE_LEFT},
    {"allway_stop",                 NODETYPE_ALLWAY_STOP},
    {"zipper",                      NODETYPE_ZIPPER},
    {"district",                    NODETYPE_DISTRICT},
    {"unregulated",                 NODETYPE_NOJUNCTION},
    {"internal",                    NODETYPE_INTERNAL},
    {"dead_end",                    NODETYPE_DEAD_END},
    {"DEAD_END",                    NODETYPE_DEAD_END_DEPRECATED},
    {"unknown",                     NODETYPE_UNKNOWN} //< must be the last one
};


StringBijection<SumoXMLEdgeFunc>::Entry SUMOXMLDefinitions::sumoEdgeFuncValues[] = {
    {"normal",       EDGEFUNC_NORMAL},
    {"connector",    EDGEFUNC_CONNECTOR},
    {"sink",         EDGEFUNC_SINK},
    {"source",       EDGEFUNC_SOURCE},
    {"crossing",     EDGEFUNC_CROSSING},
    {"walkingarea",  EDGEFUNC_WALKINGAREA},

    {"internal",     EDGEFUNC_INTERNAL} //< must be the last one
};


StringBijection<LaneSpreadFunction>::Entry SUMOXMLDefinitions::laneSpreadFunctionValues[] = {
    {"right",   LANESPREAD_RIGHT }, // default
    {"center",  LANESPREAD_CENTER } //< must be the last one
};


StringBijection<LinkState>::Entry SUMOXMLDefinitions::linkStateValues[] = {
    { "G", LINKSTATE_TL_GREEN_MAJOR },
    { "g", LINKSTATE_TL_GREEN_MINOR },
    { "r", LINKSTATE_TL_RED },
    { "u", LINKSTATE_TL_REDYELLOW },
    { "Y", LINKSTATE_TL_YELLOW_MAJOR },
    { "y", LINKSTATE_TL_YELLOW_MINOR },
    { "o", LINKSTATE_TL_OFF_BLINKING },
    { "O", LINKSTATE_TL_OFF_NOSIGNAL },
    { "M", LINKSTATE_MAJOR },
    { "m", LINKSTATE_MINOR },
    { "=", LINKSTATE_EQUAL },
    { "s", LINKSTATE_STOP },
    { "w", LINKSTATE_ALLWAY_STOP },
    { "Z", LINKSTATE_ZIPPER },
    { "-", LINKSTATE_DEADEND } //< must be the last one
};


StringBijection<LinkDirection>::Entry SUMOXMLDefinitions::linkDirectionValues[] = {
    { "s",      LINKDIR_STRAIGHT },
    { "t",      LINKDIR_TURN },
    { "T",      LINKDIR_TURN_LEFTHAND },
    { "l",      LINKDIR_LEFT },
    { "r",      LINKDIR_RIGHT },
    { "L",      LINKDIR_PARTLEFT },
    { "R",      LINKDIR_PARTRIGHT },
    { "invalid", LINKDIR_NODIR } //< must be the last one
};


StringBijection<TrafficLightType>::Entry SUMOXMLDefinitions::trafficLightTypesVales[] = {
    { "static",         TLTYPE_STATIC },
    { "rail",           TLTYPE_RAIL },
    { "actuated",       TLTYPE_ACTUATED },
    { "delayBased",     TLTYPE_DELAYBASED },
    { "sotl_phase",     TLTYPE_SOTL_PHASE },
    { "sotl_platoon",   TLTYPE_SOTL_PLATOON },
    { "sotl_request",   TLTYPE_SOTL_REQUEST },
    { "sotl_wave",      TLTYPE_SOTL_WAVE },
    { "sotl_marching",  TLTYPE_SOTL_MARCHING },
    { "swarm",          TLTYPE_SWARM_BASED },
    { "deterministic",  TLTYPE_HILVL_DETERMINISTIC },
    { "<invalid>",      TLTYPE_INVALID } //< must be the last one
};


StringBijection<LaneChangeModel>::Entry SUMOXMLDefinitions::laneChangeModelValues[] = {
    { "DK2008",     LCM_DK2008 },
    { "LC2013",     LCM_LC2013 },
    { "SL2015",     LCM_SL2015 },
    { "default",    LCM_DEFAULT } //< must be the last one
};

StringBijection<SumoXMLTag>::Entry SUMOXMLDefinitions::carFollowModelValues[] = {
    { "IDM",         SUMO_TAG_CF_IDM },
    { "IDMM",        SUMO_TAG_CF_IDMM },
    { "Krauss",      SUMO_TAG_CF_KRAUSS },
    { "KraussPS",    SUMO_TAG_CF_KRAUSS_PLUS_SLOPE },
    { "KraussOrig1", SUMO_TAG_CF_KRAUSS_ORIG1 },
    { "SmartSK",     SUMO_TAG_CF_SMART_SK },
    { "Daniel1",     SUMO_TAG_CF_DANIEL1 },
    { "PWagner2009", SUMO_TAG_CF_PWAGNER2009 },
    { "BKerner",     SUMO_TAG_CF_BKERNER },
    { "Wiedemann",   SUMO_TAG_CF_WIEDEMANN } //< must be the last one
};

StringBijection<LateralAlignment>::Entry SUMOXMLDefinitions::lateralAlignmentValues[] = {
    { "right",         LATALIGN_RIGHT },
    { "center",        LATALIGN_CENTER },
    { "arbitrary",     LATALIGN_ARBITRARY },
    { "nice",          LATALIGN_NICE },
    { "compact",       LATALIGN_COMPACT },
    { "left",          LATALIGN_LEFT } //< must be the last one
};

StringBijection<LaneChangeAction>::Entry SUMOXMLDefinitions::laneChangeActionValues[] = {
    { "stay",        LCA_STAY },
    { "left",        LCA_LEFT },
    { "right",       LCA_RIGHT },
    { "strategic",   LCA_STRATEGIC },
    { "cooperative", LCA_COOPERATIVE },
    { "speedGain",   LCA_SPEEDGAIN },
    { "keepRight",   LCA_KEEPRIGHT },
    { "sublane",     LCA_SUBLANE },
    { "traci",       LCA_TRACI },
    { "urgent",      LCA_URGENT },
    { "blocked",     LCA_BLOCKED } //< must be the last one
};

StringBijection<int> SUMOXMLDefinitions::Tags(
    SUMOXMLDefinitions::tags, SUMO_TAG_NOTHING);

StringBijection<int> SUMOXMLDefinitions::Attrs(
    SUMOXMLDefinitions::attrs, SUMO_ATTR_NOTHING);

StringBijection<SumoXMLNodeType> SUMOXMLDefinitions::NodeTypes(
    SUMOXMLDefinitions::sumoNodeTypeValues, NODETYPE_UNKNOWN);

StringBijection<SumoXMLEdgeFunc> SUMOXMLDefinitions::EdgeFunctions(
    SUMOXMLDefinitions::sumoEdgeFuncValues, EDGEFUNC_INTERNAL);

StringBijection<LaneSpreadFunction> SUMOXMLDefinitions::LaneSpreadFunctions(
    SUMOXMLDefinitions::laneSpreadFunctionValues, LANESPREAD_CENTER);

StringBijection<LinkState> SUMOXMLDefinitions::LinkStates(
    SUMOXMLDefinitions::linkStateValues, LINKSTATE_DEADEND);

StringBijection<LinkDirection> SUMOXMLDefinitions::LinkDirections(
    SUMOXMLDefinitions::linkDirectionValues, LINKDIR_NODIR);

StringBijection<TrafficLightType> SUMOXMLDefinitions::TrafficLightTypes(
    SUMOXMLDefinitions::trafficLightTypesVales, TLTYPE_INVALID);

StringBijection<LaneChangeModel> SUMOXMLDefinitions::LaneChangeModels(
    SUMOXMLDefinitions::laneChangeModelValues, LCM_DEFAULT);

StringBijection<SumoXMLTag> SUMOXMLDefinitions::CarFollowModels(
    SUMOXMLDefinitions::carFollowModelValues, SUMO_TAG_CF_WIEDEMANN);

StringBijection<LateralAlignment> SUMOXMLDefinitions::LateralAlignments(
    SUMOXMLDefinitions::lateralAlignmentValues, LATALIGN_LEFT);

StringBijection<LaneChangeAction> SUMOXMLDefinitions::LaneChangeActions(
    SUMOXMLDefinitions::laneChangeActionValues, LCA_BLOCKED);

std::string
SUMOXMLDefinitions::getJunctionIDFromInternalEdge(const std::string internalEdge) {
    assert(internalEdge[0] == ':');
    return internalEdge.substr(1, internalEdge.rfind('_') - 1);
}

std::string
SUMOXMLDefinitions::getEdgeIDFromLane(const std::string laneID) {
    return laneID.substr(0, laneID.rfind('_'));
}

/****************************************************************************/

