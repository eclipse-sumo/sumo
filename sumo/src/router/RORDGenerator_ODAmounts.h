#ifndef RORDGenerator_ODAmounts_h
#define RORDGenerator_ODAmounts_h
//---------------------------------------------------------------------------//
//                        RORDGenerator_ODAmounts.h -
//      Class for loading trip amount definitions and route generation
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
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2005/02/17 10:33:40  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.6  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.5  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.4  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.3  2004/02/02 16:19:23  dkrajzew
// trying to catch up false user input (using the same name for different routes)
//
// Revision 1.2  2004/01/27 08:45:00  dkrajzew
// given flow definitions an own tag
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
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
#include <set>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/gfx/RGBColor.h>
#include <utils/router/IDSupplier.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "RORDLoader_TripDefs.h"
#include "RONet.h"
#include <utils/common/SUMOTime.h>


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
    RORDGenerator_ODAmounts(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end, bool emptyDestinationsAllowed,
        const std::string &file="");

    /// Destructor
    ~RORDGenerator_ODAmounts();

    /** @brief Returns the name of the data read.
        "XML-route definitions" is returned here */
    std::string getDataName() const;

    /// Returns the information whether no routes are available from this loader anymore
    bool ended() const;

    /** @brief Closes the reading of routes
        Overridden, as no parse toke is used herein as within the other parsers */
    void closeReading();

    /// reader dependent initialisation
    virtual bool init(OptionsCont &options);



protected:
    /** @brief Reads the until the specified time is reached
        Do read the comments on ROAbstractRouteDefLoader::myReadRoutesAtLeastUntil
        for the modalities! */
    bool myReadRoutesAtLeastUntil(SUMOTime until);

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

    /**
     * @class FlowDef
     * The definition of a flow.
     */
    class FlowDef {
    public:
        /// Constructor
        FlowDef(ROVehicle *vehicle, ROVehicleType *type, RORouteDef *route,
            SUMOTime intBegin, SUMOTime intEnd,
            unsigned int vehicles2Emit);

        /// Destructor
        ~FlowDef();

        /// Returns the information whether this flow includes the given time
        bool applicableForTime(SUMOTime time) const;

        /// Adds routes to start within the given time into the given net
        void addRoutes(ROVehicleBuilder &vb, RONet &net, SUMOTime time);

        /// Adds a single route to start within the given time into the given net
        void addSingleRoute(ROVehicleBuilder &vb,
            RONet &net, SUMOTime time);

        /// Returns the end of the period this FlowDef describes
        SUMOTime getIntervalEnd() const;

    private:
        /// (description of) the vehicle to emit
        ROVehicle *myVehicle;

        /// (description of) the vehicle type to use
        ROVehicleType *myVehicleType;

        /// (description of) the route to use
        RORouteDef *myRoute;

        /// The begin of the described interval
        SUMOTime myIntervalBegin;

        /// The end of the described interval
        SUMOTime myIntervalEnd;

        /// The number of vehicles to emit within the described interval
        unsigned int myVehicle2EmitNumber;

        /// The number of vehicles already emitted
        unsigned int myEmitted;

    };

private:
    /// Builds the routes between the current time step and the one given
    void buildRoutes(SUMOTime until);

    /// Builds the routes for the given time step
    void buildForTimeStep(SUMOTime time);

private:
    /// The begin of the interval current read
    SUMOTime myIntervalBegin;

    /// The end of the interval current read
    SUMOTime myIntervalEnd;

    /// The begin of the embedding interval (if given)
    SUMOTime myUpperIntervalBegin;

    /// The end of the embedding interval (if given)
    SUMOTime myUpperIntervalEnd;

    /// The number of vehicles to emit
    size_t myVehicle2EmitNumber;

    /// The current time step read
    SUMOTime myCurrentTimeStep;

    /// Definition of a container for flo definitions (vector)
    typedef std::vector<FlowDef*> FlowDefV;

    /// The definitions of the read flows
    FlowDefV myFlows;

    /// The information whether no further routes exist
    bool myEnded;

    /// A storage for ids (!!! this should be done router-wide)
    std::set<std::string> myKnownIDs;

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

