/****************************************************************************/
/// @file    RORDLoader_SUMORoutes.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SAX-handler for SUMO-routes
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
#ifndef RORDLoader_SUMORoutes_h
#define RORDLoader_SUMORoutes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "RORDLoader_SUMOBase.h"
#include <utils/gfx/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORDLoader_SUMORoutes
 * A handler for SUMO-routes.
 */
class RORDLoader_SUMORoutes : public RORDLoader_SUMOBase
{
public:
    /// Constructor
    RORDLoader_SUMORoutes(ROVehicleBuilder &vb, RONet &net,
                          SUMOTime begin, SUMOTime end, const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMORoutes();

protected:
    //{ XML-handling methods
    /** the user-implemented handler method for characters */
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars) throw();

    /** the user-implemented handler method for a closing tag */
    void myEndElement(SumoXMLTag element, const std::string &name) throw();
    //}

protected:
    /// begins the processing of a route
    void startRoute(const Attributes &attrs);

private:
    /// we made the copy constructor invalid
    RORDLoader_SUMORoutes(const RORDLoader_SUMORoutes &src);

    /// we made the assignment operator invalid
    RORDLoader_SUMORoutes &operator=(const RORDLoader_SUMORoutes &src);

};


#endif

/****************************************************************************/

