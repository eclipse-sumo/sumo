#ifndef ROSUMOAltRoutesHandler_h
#define ROSUMOAltRoutesHandler_h
//---------------------------------------------------------------------------//
//                        ROSUMOAltRoutesHandler.h -
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
// Revision 1.3  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include "ROSUMOHandlerBase.h"
#include <utils/sumoxml/SUMOXMLDefinitions.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class RORouteAlternativesDef;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROSUMOAltRoutesHandler
 * A SAX-Handler which parses SUMO-Route-Alternatives.
 */
class ROSUMOAltRoutesHandler : public ROSUMOHandlerBase {
public:
    /// Constructor
    ROSUMOAltRoutesHandler(RONet &net, double gawronBeta,
        double gawronA, const std::string &file="");

    /// Destructor
    ~ROSUMOAltRoutesHandler();

    /// Returns a fully configured duplicate
    ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const;

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

private:
    /// Begins the parsing of the next route alternative in the file
    void startAlternative(const Attributes &attrs);

    /// Begins the parsing of a route alternative of the opened route
    void startRoute(const Attributes &attrs);

    /// Ends the processing of a route alternative
    void endAlternative();

private:
    /// The current route alternatives
    RORouteAlternativesDef *_currentAlternatives;

    /// The costs of the current alternative
    double _cost;

    /// The propability of the current alternative's usage
    double _prob;

    /// gawron beta - value
    double _gawronBeta;

    /// gawron beta - value
    double _gawronA;

private:
    /// we made the copy constructor invalid
    ROSUMOAltRoutesHandler(const ROSUMOAltRoutesHandler &src);

    /// we made the assignment operator invalid
    ROSUMOAltRoutesHandler &operator=(const ROSUMOAltRoutesHandler &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROSUMOAltRoutesHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

