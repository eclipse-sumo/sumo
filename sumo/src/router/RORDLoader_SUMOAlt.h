#ifndef RORDLoader_SUMOAlt_h
#define RORDLoader_SUMOAlt_h
//---------------------------------------------------------------------------//
//                        RORDLoader_SUMOAlt.h -
//  A SAX-handler for SUMO-route-alternatives
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
// Revision 1.4  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
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


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class RORouteDef_Alternatives;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDLoader_SUMOAlt
 * A SAX-Handler which parses SUMO-Route-Alternatives.
 */
class RORDLoader_SUMOAlt : public RORDLoader_SUMOBase {
public:
    /// Constructor
    RORDLoader_SUMOAlt(RONet &net, double gawronBeta,
        double gawronA, const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMOAlt();

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
    /// Begins the parsing of the next route alternative in the file
    void startAlternative(const Attributes &attrs);

    /// Begins the parsing of a route alternative of the opened route
    void startRoute(const Attributes &attrs);

    /// Ends the processing of a route alternative
    void endAlternative();

private:
    /// The current route alternatives
    RORouteDef_Alternatives *_currentAlternatives;

    /// The costs of the current alternative
    double _cost;

    /// The propability of the current alternative's usage
    double _prob;

    /// gawron beta - value
    double _gawronBeta;

    /// gawron beta - value
    double _gawronA;

    /// The information whether the next route was read
    bool _nextRouteRead;

private:
    /// we made the copy constructor invalid
    RORDLoader_SUMOAlt(const RORDLoader_SUMOAlt &src);

    /// we made the assignment operator invalid
    RORDLoader_SUMOAlt &operator=(const RORDLoader_SUMOAlt &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

