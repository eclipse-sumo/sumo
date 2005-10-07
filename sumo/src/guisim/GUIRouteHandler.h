#ifndef GUIRouteHandler_h
#define GUIRouteHandler_h
/***************************************************************************
                          GUIRouteHandler.h
              Parser and container for routes during their loading
                             -------------------
    project              : SUMO
    begin                : Thu, 17. Jun 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.5  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
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
#include <microsim/MSRouteHandler.h>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIRouteHandler
 */
class GUIRouteHandler : public MSRouteHandler
{
public:
    /// standard constructor
    GUIRouteHandler(const std::string &file, bool addVehiclesDirectly);

    /// standard destructor
    virtual ~GUIRouteHandler();

protected:
    /** parses an occured vehicle type definition */
    virtual void addVehicleType(const Attributes &attrs);

    /** adds the parsed vehicle type */
    virtual void addParsedVehicleType(const std::string &id,
        const SUMOReal length, const SUMOReal maxspeed, const SUMOReal bmax,
        const SUMOReal dmax, const SUMOReal sigma, const RGBColor &c);

    void closeRoute();
    void openRoute(const Attributes &attrs);

private:
    RGBColor myColor;

private:
    /** invalid copy constructor */
    GUIRouteHandler(const GUIRouteHandler &s);

    /** invalid assignment operator */
    GUIRouteHandler &operator=(const GUIRouteHandler &s);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
