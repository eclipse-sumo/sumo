#ifndef RORDLoader_TripDefs_h
#define RORDLoader_TripDefs_h
//---------------------------------------------------------------------------//
//                        RORDLoader_TripDefs.h -
//      The basic class for loading trip definitions
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
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:25:52  dkrajzew
// debugging
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
// Revision 1.5  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.4  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.3  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/03/03 15:22:36  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/gfx/RGBColor.h>
#include <utils/router/IDSupplier.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDLoader_TripDefs
 * A handler for route definitions which consists of the origin and the
 * destination edge only. Additionally, read vehicles may have information
 * about a certain position to leave from and a certain speed to leave with.
 */
class RORDLoader_TripDefs : public ROTypedXMLRoutesLoader {
public:
    /// Constructor
    RORDLoader_TripDefs(ROVehicleBuilder &vb,
        RONet &net, SUMOTime begin, SUMOTime end,
        bool emptyDestinationsAllowed, const std::string &file="");

    /// Destructor
    ~RORDLoader_TripDefs();

    /** @brief Returns the name of the data read.
        "XML-route definitions" is returned here */
    std::string getDataName() const;

    /// Returns the time the current (last read) route starts at
    SUMOTime getCurrentTimeStep() const;

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

protected:
    /// Parses the vehicle id
    std::string getVehicleID(const Attributes &attrs);

    /// Parses a named edge frm the attributes
    ROEdge *getEdge(const Attributes &attrs, const std::string &purpose,
        AttrEnum which, const std::string &id, bool emptyAllowed);

    /// Parses the vehicle type
    std::string getVehicleType(const Attributes &attrs);

    /** @brief Parses a float value which is optional;
        Avoids exceptions */
    float getOptionalFloat(const Attributes &attrs,
        const std::string &name, AttrEnum which, const std::string &place);

    /// Parses and returns the time the vehicle should start at
    SUMOTime getTime(const Attributes &attrs, AttrEnum which,
        const std::string &id);

    /// Parses and returns the period the trip shall be repeated with
    SUMOTime getPeriod(const Attributes &attrs, const std::string &id);

    /// Parses and returns the number of cars that shall use the same parameter
    int getRepetitionNumber(const Attributes &attrs, const std::string &id);

    /** @brief Parses and returns the name of the lane the vehicle starts at
        This value is optional */
    std::string getLane(const Attributes &attrs);

    RGBColor getRGBColorReporting(const Attributes &attrs,
        const std::string &id);

protected:
    /// generates numerical ids
    IDSupplier _idSupplier;

    /// The id of the vehicle
    std::string myID;

    /// The starting edge
    ROEdge *myBeginEdge;

    /// The end edge
    ROEdge *myEndEdge;

    /// The type of the vehicle
    std::string myType;

    /// The optional position the vehicle shall star at
    double myPos;

    /// The optional speed the vehicle shall start with
    double mySpeed;

    /// The time the vehicle shall start at
    SUMOTime myDepartureTime;

    /// The period the next vehicle with the same route shall be emitted at
    SUMOTime myPeriodTime;

    /// The number of times cars with the current parameter shall be reemitted
    int myNumberOfRepetitions;

    /// The optional name of the lane the vehicle shall start at
    std::string myLane;

    /// The list of edges to take when given
    ROEdgeVector myEdges;

    /// The color of the vehicle
    RGBColor myColor;

    /** @brief Information whether empty destinations are allowed
        This is a feature used for the handling of explicite routes within the
        jp-router where the destination is not necessary */
    bool myEmptyDestinationsAllowed;

    /// The information whether the next route was read
    bool _nextRouteRead;

private:
    /// we made the copy constructor invalid
    RORDLoader_TripDefs(const RORDLoader_TripDefs &src);

    /// we made the assignment operator invalid
    RORDLoader_TripDefs &operator=(const RORDLoader_TripDefs &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

