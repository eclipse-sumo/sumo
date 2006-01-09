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
// Revision 1.9  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.8  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:18:52  dksumo
// max-alternatives options added
//
// Revision 1.1  2004/10/22 12:50:27  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
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
    RORDLoader_SUMOAlt(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end,
        SUMOReal gawronBeta, SUMOReal gawronA, int maxRouteNumber,
        const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMOAlt();

protected:
    //{ XML-handling methods
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);
    //}

protected:
    /// Begins the parsing of the next route alternative in the file
    void startAlternative(const Attributes &attrs);

    /// Begins the parsing of a route alternative of the opened route
    void startRoute(const Attributes &attrs);

    /// Ends the processing of a route alternative
    void endAlternative();

private:
    /// The current route alternatives
    RORouteDef_Alternatives *myCurrentAlternatives;

    /// The costs of the current alternative
    SUMOReal myCost;

    /// The probability of the current alternative's usage
    SUMOReal myProbability;

    /// gawron beta - value
    SUMOReal myGawronBeta;

    /// gawron beta - value
    SUMOReal myGawronA;

    /// The maximum route alternatives number
    int myMaxRouteNumber;

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

