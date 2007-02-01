/****************************************************************************/
/// @file    NLLoadFilter.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id: $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NLLoadFilter_h
#define NLLoadFilter_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// definitions
// ===========================================================================
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
    /** load the sources */
    LOADFILTER_SOURCES = 8,
    /** load static net elements only */
    LOADFILTER_NET = 1 | 2 | 8
};


#endif

/****************************************************************************/

