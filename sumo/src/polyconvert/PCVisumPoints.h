#ifndef PCVisumPoints_h
#define PCVisumPoints_h
/***************************************************************************
                          PCVisumPoints.h
    A reader of pois stored in visum-format
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Thu, 02.11.2006
    copyright            : (C) 2006 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2007/01/08 14:43:59  dkrajzew
// code beautifying; prliminary import for Visum points added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include "PCPolyContainer.h"
#include "PCTypeMap.h"


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * @class PCVisumPoints
 * @brief A reader of pois stored in visum-format
 */
class PCVisumPoints {
public:
    /// Constructor
    PCVisumPoints(PCPolyContainer &toFill,
        const Boundary &netBoundary, PCTypeMap &tm);

    /// Destructor
    ~PCVisumPoints();

    /// loads Elmar's data parsed from GDF
    void load(OptionsCont &oc);


private:
    /// The container to store the converted polygons into
    PCPolyContainer &myCont;

    /// The type map to use
    PCTypeMap &myTypeMap;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
