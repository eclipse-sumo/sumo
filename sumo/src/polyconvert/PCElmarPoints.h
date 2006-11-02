#ifndef PCElmarPoints_h
#define PCElmarPoints_h
/***************************************************************************
                          PCElmarPoints.h
    A reader of pois stored in elmar-format
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
// Revision 1.1  2006/11/02 12:19:50  dkrajzew
// added parsing of Elmar's pointcollections
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
 * @class PCElmarPoints
 * @brief A reader of pois stored in elmar-format
 *
 * Reads polygons stored in "pointcollection.txt",
 *  applies the given projection and network offset and stores the so build
 *  polygons into the given map.
 */
class PCElmarPoints {
public:
    /// Constructor
    PCElmarPoints(PCPolyContainer &toFill,
        const Boundary &netBoundary, PCTypeMap &tm);

    /// Destructor
    ~PCElmarPoints();

    /// loads Elmar's data parsed from GDF
    void loadElmar(OptionsCont &oc);


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
