#ifndef GUIRoute_h
#define GUIRoute_h
//---------------------------------------------------------------------------//
//                        GUIRoute.h -
//  A vehicle route
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 17. Jun 2004
//  copyright            : (C) 2004 by DLR http://www.dlr.de/vf
//  author               : Daniel Krajzewicz
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
// Revision 1.4  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 08:02:55  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/07/02 08:56:12  dkrajzew
// coloring of routes and vehicle types added
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
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIRoute : public MSRoute {
public:
    /// Constructor
    GUIRoute(const RGBColor &c,
        const std::string &id, const MSEdgeVector &edges,
        bool multipleReferenced);

    /// Destructor
    ~GUIRoute();

    const RGBColor &getColor() const;

private:
    RGBColor myColor;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

