#ifndef ROTripHandler_h
#define ROTripHandler_h
//---------------------------------------------------------------------------//
//                        ROTripHandler.h -
//  The basic class for route definition loading
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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
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
 * class declarations
 * ======================================================================= */
class SAX2XMLReader;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROTripHandler
 * A handler for route definitions which consists of the origin and the
 * destination edge only. Additionally, read vehicles may have information
 * about a certain position to leave from and a certain speed to leave with.
 */
class ROTripHandler : public ROTypedXMLRoutesLoader {
public:
    /// Constructor
    ROTripHandler(RONet &net, const std::string &file="");

    /// Destructor
    ~ROTripHandler();

    /// Returns the fully configurated loader of this type
    ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const;

    /** @brief Returns the name of the data read.
        "XML-route definitions" is returned here */
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
    /// Parses the vehicle id
    std::string getVehicleID(const Attributes &attrs);

    /// Parses a named edge frm the attributes
    ROEdge *getEdge(const Attributes &attrs, const std::string &purpose,
        AttrEnum which, const std::string &id);

    /// Parses the vehicle type
    std::string getVehicleType(const Attributes &attrs);

    /** @brief Parses a float value which is optional;
        Avoids exceptions */
    float getOptionalFloat(const Attributes &attrs,
        const std::string &name, AttrEnum which, const std::string &place);

    /// Parses and returns the time the vehicle should start at
    long getDepartureTime(const Attributes &attrs, const std::string &id);

    /// Parses and returns the period the trip shall be repeated with
    int getPeriod(const Attributes &attrs, const std::string &id);

    /// Parses and returns the number of cars that shall use the same parameter
    int getRepetitionNumber(const Attributes &attrs, const std::string &id);

    /** @brief Parses and returns the name of the lane the vehicle starts at
        This value is optional */
    std::string getLane(const Attributes &attrs);

    RGBColor getRGBColorReporting(const Attributes &attrs,
        const std::string &id);

private:
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
    int myDepartureTime;

    /// The period the next vehicle with the same route shall be emitted at
    int myPeriodTime;

    /// The number of times cars with the current parameter shall be reemitted
    int myNumberOfRepetitions;

    /// The optional name of the lane the vehicle shall start at
    std::string myLane;

    /// The list of edges to take when given
    ROEdgeVector myEdges;

    /// The color of the vehicle
    RGBColor myColor;

private:
    /// we made the copy constructor invalid
    ROTripHandler(const ROTripHandler &src);

    /// we made the assignment operator invalid
    ROTripHandler &operator=(const ROTripHandler &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROTripHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

