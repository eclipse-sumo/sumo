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
 * LoadFilter
 * The load filter is an information given to the handler to describe which
 * types of data shall be loaded. 
 */
enum LoadFilter { 
    /** load all known data */
    LOADFILTER_ALL = 255,
    /** load only the junction logics */
    LOADFILTER_LOGICS = 1,
    /** load detectors only */
    LOADFILTER_DETECTORS = 2,
    /** load only vehicles, their routes and theri types */
    LOADFILTER_DYNAMIC = 4,
    /** load static net elements only */
    LOADFILTER_NET = 3
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "LoadFilter.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// 

