/****************************************************************************/
/// @file    GUIRouteHandler.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id$
///
// Parser and container for routes during their loading (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIRouteHandler_h
#define GUIRouteHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSRouteHandler.h>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIRouteHandler
 * @brief Parser and container for routes during their loading (gui-version)
 */
class GUIRouteHandler : public MSRouteHandler
{
public:
    /// standard constructor
    GUIRouteHandler(const std::string &file,
                    bool addVehiclesDirectly);

    /// standard destructor
    virtual ~GUIRouteHandler() throw();

protected:
    /// Builds the route by calling the constructor
    MSRoute* buildRoute() throw();

    /// Starts route processing
    void openRoute(const SUMOSAXAttributes &attrs);

private:
    /// The currently parsed route's color
    RGBColor myColor;

private:
    /** invalid copy constructor */
    GUIRouteHandler(const GUIRouteHandler &s);

    /** invalid assignment operator */
    GUIRouteHandler &operator=(const GUIRouteHandler &s);

};


#endif

/****************************************************************************/

