#ifndef PCElmar_h
#define PCElmar_h
/***************************************************************************
                          PCElmar.h
    A reader of polygons stored in elmar-format
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
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
// Revision 1.2  2006/09/18 10:14:35  dkrajzew
// changed the way geocoordinates are processed
//
// Revision 1.1  2006/08/01 07:52:46  dkrajzew
// polyconvert added
//
// Revision 1.2  2006/03/27 07:36:34  dksumo
// some further work...
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
 * @class PCElmar
 * @brief A reader of polygons stored in elmar-format
 *
 * Reads polygons stored in "..._polygons.txt" or "..._water_polygons.txt",
 *  applies the given projection and network offset and stores the so build
 *  polygons into the given map.
 */
class PCElmar {
public:
    /// Constructor
    PCElmar(PCPolyContainer &toFill,
        const Boundary &netBoundary, const Position2D &netOffset,
        PCTypeMap &tm);

    /// Destructor
    ~PCElmar();

    /// loads Elmar's data parsed from GDF
    void loadElmar(OptionsCont &oc);


private:
    /// The container to store the converted polygons into
	PCPolyContainer &myCont;

    /// First x and y positions read used if no projection is given
    SUMOReal myInitX, myInitY;

    /** @brief The network offset
     *
     * The offset the network was shifted by. This offset must be applied to the
     *  polygons, too.
     */
    Position2D myNetOffset;

    /// The type map to use
    PCTypeMap &myTypeMap;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
