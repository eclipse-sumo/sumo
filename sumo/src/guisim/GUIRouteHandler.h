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
// Revision 1.1  2004/07/02 08:56:12  dkrajzew
// coloring of routes and vehicle types added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
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
        const float length, const float maxspeed, const float bmax,
        const float dmax, const float sigma, const RGBColor &c);

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
