#ifndef NLLoadFilter_h
#define NLLoadFilter_h
/***************************************************************************
                          NLDetectorBuilder.h
                          A building helper for the detectors
                             -------------------
    begin                : Mon, 15 Apr 2002
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
// Revision 1.4  2003/03/19 08:07:34  dkrajzew
// bugs and syntactical faults arised during compilation on Linux removed
//
// Revision 1.3  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 10:32:41  dkrajzew
// sources and detectors joined with triggers to additional-files; usage of standard SUMOSAXHandler instead of NLSAXHandler; loading of triggers implemented
//
// Revision 1.1  2002/10/16 15:36:48  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.6  2002/07/31 17:34:51  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.6  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.5  2002/06/11 14:39:25  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:18:47  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/16 06:50:20  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * The load filter is an information given to the handler to describe which
 * types of data shall be loaded.
 */
enum LoadFilter {
    /** load all known data */
    LOADFILTER_ALL = 255,

    /** load only the junction logics */
    LOADFILTER_LOGICS = 1,

    /** load detectors only */
    LOADFILTER_NETADD = 2,

    /** load only vehicles, their routes and theri types */
    LOADFILTER_DYNAMIC = 4,

    /** load static net elements only */
    LOADFILTER_NET = 1 | 2 | 8
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
/* =========================================================================
 * included modules
 * ======================================================================= */
//#include "LoadFilter.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
