#ifndef RORDLoader_SUMORoutes_h
#define RORDLoader_SUMORoutes_h
//---------------------------------------------------------------------------//
//                        RORDLoader_SUMORoutes.h -
//  A SAX-handler for SUMO-routes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
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
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.4  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.3  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include "RORDLoader_SUMOBase.h"
#include <utils/gfx/RGBColor.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDLoader_SUMORoutes
 * A handler for SUMO-routes.
 */
class RORDLoader_SUMORoutes : public RORDLoader_SUMOBase {
public:
    /// Constructor
    RORDLoader_SUMORoutes(RONet &net, const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMORoutes();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

protected:
    /// Return the information whether a route was read
    bool nextRouteRead();

    /// Initialises the reading of a further route
    void beginNextRoute();

private:
    /// begins the processing of a route
    void startRoute(const Attributes &attrs);

private:
    /// the name of the current route
    std::string _currentRoute;

    /// The color of the current route
    RGBColor myCurrentColor;

    /// The information whether the next route was read
    bool _nextRouteRead;

private:
    /// we made the copy constructor invalid
    RORDLoader_SUMORoutes(const RORDLoader_SUMORoutes &src);

    /// we made the assignment operator invalid
    RORDLoader_SUMORoutes &operator=(const RORDLoader_SUMORoutes &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

