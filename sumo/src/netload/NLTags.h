#ifndef NLTags_h
#define NLTags_h
/***************************************************************************
                          NLTags.h
			  Enumeration for known tags fopr better handling
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/VF http://ivf.dlr.de/
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
// Revision 1.1  2002/10/16 15:36:49  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.6  2002/07/31 17:34:51  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.6  2002/07/22 12:44:33  dkrajzew
// Source loading structures added
//
// Revision 1.5  2002/06/11 14:39:25  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:18:48  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:07:57  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.1  2002/03/15 09:38:32  traffic
// New known tags (number of incoming lanes) included
//
// Revision 2.0  2002/02/14 14:43:25  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:46  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:12  traffic
// moved from netbuild
//
// Revision 1.3  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//

/* =========================================================================
 * enumeration definitions
 * ======================================================================= */
/**
 * NLTag
 * an enumeration type for possible XML-tags
 */
typedef enum _NLTag {
    /** invalid tag */
    NLTag_nothing,
    /** begin/end of the simulation description */
    NLTag_simulation,
    /** begin/end of the description of an edge */
    NLTag_edge,
    /** begin/end of the description of a single lane */
    NLTag_lane,
    /** begin/end of the list of lane descriptions */
    NLTag_lanes,
    /** description of a connected edge */
    NLTag_cedge,
    /** begin/end of the description of a junction */
    NLTag_junction,
    /** begin/end of the list of incoming lanes (into a junction) */
    NLTag_inlane,
    /** description of a vehicle */
    NLTag_vehicle,
    /** description of a vehicle type */
    NLTag_vtype,
    /** begin/end of the description of a route */
    NLTag_route,
    /** begin/end of the description of succeding lanes */
    NLTag_succ,
    /** description of a single succeding lane */
    NLTag_succlane,
    /** begin/end of the key of a junction */
    NLTag_key,
    /** begin/end of the list of junction logic parts */
    NLTag_junctionlogic,
    /** begin of a right-of-way logic */
    NLTag_rowlogic,
    /** the request size (of a junction logic) */
    NLTag_requestsize,
    /** the response size (of a junction logic) */
    NLTag_responsesize,
    /** the number of lanes */
    NLTag_lanenumber,
    /** description of a single junction logic part */
    NLTag_logicitem,
    /** a transformation item */
    NLTag_trafoitem,
    /** a detector */
    NLTag_detector,
    /** a source */
    NLTag_source,
    /** a district */
    NLTag_district,
    /** a source within a district (connection road) */
    NLTag_dsource,
    /** a sink within a district (connection road) */
    NLTag_dsink,
    /** the list of edge names for preallocation */
    NLTag_edges,
    /** the number of nodes within the net */
    NLTag_nodecount,
    /** a traffic light logic */
    NLTag_tllogic,
    /** the time offset from the first switch */
    NLTag_offset,
    /** the initial step of the traffic light */
    NLTag_initstep,
    /** the number of phases of the traffic light */
    NLTag_phaseno,
    /** a single phase description */
    NLTag_phase,
    /** the position of an edge */
    NLTag_edgepos,
    /** the number of a logic */
    NLTag_logicno
} NLTag;

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLTags.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
