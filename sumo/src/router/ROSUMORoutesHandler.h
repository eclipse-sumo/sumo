#ifndef ROSumoRoutesHandler_h
#define ROSumoRoutesHandler_h
//---------------------------------------------------------------------------//
//                        ROSUMORoutesHandler.h -
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
#include "ROTypedXMLRoutesLoader.h"
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
 * @class ROSumoRoutesHandler
 * A hanlder for SUMO-routes.
 */
class ROSumoRoutesHandler : public ROTypedXMLRoutesLoader {
public:
    /// Constructor
    ROSumoRoutesHandler(RONet &net, const std::string &file="");

    /// Destructor
    ~ROSumoRoutesHandler();

    /// Returns the fully configured sumo-loader
    ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const;

    /** @brief returns the name of the loaded data
        "precomputed sumo routes" is returned here */
    std::string getDataName() const;

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
    /// begins the processing of a route
    void startRoute(const Attributes &attrs);

    /// Parses a vehicle
    void startVehicle(const Attributes &attrs);

    /// Parses a vehicle type
    void startVehType(const Attributes &attrs);

    /// Parses a float from the attributes and reports errors, if any
    float getFloatReporting(const Attributes &attrs, AttrEnum attr,
        const std::string &id, const std::string &name) ;

private:
    /// the name of the current route
    std::string _currentRoute;

private:
    /// we made the copy constructor invalid
    ROSumoRoutesHandler(const ROSumoRoutesHandler &src);

    /// we made the assignment operator invalid
    ROSumoRoutesHandler &operator=(const ROSumoRoutesHandler &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROSUMORoutesHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

