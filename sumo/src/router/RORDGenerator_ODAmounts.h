#ifndef RORDGenerator_ODAmounts_h
#define RORDGenerator_ODAmounts_h
//---------------------------------------------------------------------------//
//                        RORDGenerator_ODAmounts.h -
//		Class for loading trip amount definitions and route generation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 21 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2004/01/27 08:45:00  dkrajzew
// given flow definitions an own tag
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
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
#include "RORDLoader_TripDefs.h"
#include "RONet.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDGenerator_ODAmounts
 * This class is a file-based generator. Within the first step, all route
 *  definitions are parsed into the memory from a file and then consecutively
 *  build on demand.
 */
class RORDGenerator_ODAmounts : public RORDLoader_TripDefs {
public:
    /// Constructor
    RORDGenerator_ODAmounts(RONet &net, bool emptyDestinationsAllowed,
		const std::string &file="");

    /// Destructor
    ~RORDGenerator_ODAmounts();

    /// Returns the fully configurated loader of this type
    ROAbstractRouteDefLoader *getAssignedDuplicate(const std::string &file) const;

    /** @brief Returns the name of the data read.
        "XML-route definitions" is returned here */
    std::string getDataName() const;

    /// Returns the information whether no routes are available from this loader anymore
	bool ended() const;

    /** @brief Closes the reading of routes
        Overridden, as no parse toke is used herein as within the other parsers */
    void closeReading();


protected:
    /** @brief Reads the until the specified time is reached
        Do read the comments on ROAbstractRouteDefLoader::myReadRoutesAtLeastUntil
        for the modalities! */
	bool myReadRoutesAtLeastUntil(unsigned int until);

    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

protected:
    /// Parses the interval information
    void parseInterval(const Attributes &attrs);

    /// Parses the trip amount definition
    void parseFlowAmountDef(const Attributes &attrs);

    /// Closes the current embedding interval
    void myEndInterval();

    /// Closes the trip amount parsing
    void myEndFlowAmountDef();

protected:
    /// reader dependent initialisation
    bool myInit(OptionsCont &options);

    /**
     * @class FlowDef
     * The definition of a flow.
     */
    class FlowDef {
    public:
        /// Constructor
        FlowDef(ROVehicle *vehicle, ROVehicleType *type, RORouteDef *route,
            unsigned int intBegin, unsigned int intEnd,
            unsigned int vehicles2Emit);

        /// Destructor
        ~FlowDef();

        /// Returns the information whether this flow includes the given time
        bool applicableForTime(unsigned int time) const;

        /// Adds routes to start within the given time into the given net
        void addRoutes(RONet &net, unsigned int time);

        /// Adds a single route to start within the given time into the given net
        void addSingleRoute(RONet &net, unsigned int time);

        /// Returns the end of the period this FlowDef describes
        unsigned int getIntervalEnd() const;

    private:
        /// (description of) the vehicle to emit
        ROVehicle *myVehicle;

        /// (description of) the vehicle type to use
        ROVehicleType *myVehicleType;

        /// (description of) the route to use
        RORouteDef *myRoute;

        /// The begin of the described interval
        unsigned int myIntervalBegin;

        /// The end of the described interval
        unsigned int myIntervalEnd;

        /// The number of vehicles to emit within the described interval
        unsigned int myVehicle2EmitNumber;

        /// The number of vehicles already emitted
        unsigned int myEmitted;

    };

private:
    /// Builds the routes between the current time step and the one given
    void buildRoutes(unsigned int until);

    /// Builds the routes for the given time step
    void buildForTimeStep(unsigned int time);

private:
    /// The begin of the interval current read
    int myIntervalBegin;

    /// The end of the interval current read
    int myIntervalEnd;

    /// The begin of the embedding interval (if given)
    int myUpperIntervalBegin;

    /// The end of the embedding interval (if given)
    int myUpperIntervalEnd;

    /// The number of vehicles to emit
    int myVehicle2EmitNumber;

    /// The current time step read
    unsigned int myCurrentTimeStep;

    /// Definition of a container for flo definitions (vector)
    typedef std::vector<FlowDef*> FlowDefV;

    /// The definitions of the read flows
    FlowDefV myFlows;

    /// The information whether no further routes exist
    bool myEnded;

private:
    /// we made the copy constructor invalid
    RORDGenerator_ODAmounts(const RORDGenerator_ODAmounts &src);

    /// we made the assignment operator invalid
    RORDGenerator_ODAmounts &operator=(const RORDGenerator_ODAmounts &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

