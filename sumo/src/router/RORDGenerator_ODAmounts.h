/****************************************************************************/
/// @file    RORDGenerator_ODAmounts.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Class for loading trip amount definitions and route generation
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
#ifndef RORDGenerator_ODAmounts_h
#define RORDGenerator_ODAmounts_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <set>
#include <utils/options/OptionsCont.h>
#include <utils/common/IDSupplier.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "RORDLoader_TripDefs.h"
#include "RONet.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORDGenerator_ODAmounts
 * This class is a file-based generator. Within the first step, all route
 *  definitions are parsed into the memory from a file and then consecutively
 *  build on demand.
 */
class RORDGenerator_ODAmounts : public RORDLoader_TripDefs {
public:
    /// Constructor
    RORDGenerator_ODAmounts(RONet &net,
                            SUMOTime begin, SUMOTime end, bool emptyDestinationsAllowed,
                            bool randomize, const std::string &file="") throw(ProcessError);

    /// Destructor
    ~RORDGenerator_ODAmounts() throw();


    /// @name inherited from ROAbstractRouteDefLoader
    //@{

    /** @brief Adds routes from the file until the given time is reached
     *
     * @param[in] time The time until which route definitions shall be loaded
     * @param[in] skipping Whether routes shall not be added
     * @return Whether any errors occured
     * @exception ProcessError If a major error occured
     * @see ROAbstractRouteDefLoader::readRoutesAtLeastUntil
     */
    bool readRoutesAtLeastUntil(SUMOTime until, bool skipping) throw(ProcessError);
    /// @}


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when a closing tag occures
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}

    /// Parses the interval information
    void parseInterval(const SUMOSAXAttributes &attrs);

    /// Parses the trip amount definition
    void parseFlowAmountDef(const SUMOSAXAttributes &attrs) throw(ProcessError);

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
                unsigned int vehicles2Emit, bool randomize);

        /// Destructor
        ~FlowDef();

        /// Returns the information whether this flow includes the given time
        bool applicableForTime(SUMOTime time) const;

        /// Adds routes to start within the given time into the given net
        void addRoutes(RONet &net, SUMOTime time);

        /// Adds a single route to start within the given time into the given net
        void addSingleRoute(RONet &net, SUMOTime time);

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

        /// The list of generated departure times in the case randomized departures are used
        std::vector<SUMOTime> myDepartures;

        /// Information whether randomized departures are used
        bool myRandom;

    };

private:
    /// Builds the routes between the current time step and the one given
    void buildRoutes(SUMOTime until) throw();

    /// Builds the routes for the given time step
    void buildForTimeStep(SUMOTime time) throw();

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
    unsigned int myVehicle2EmitNumber;

    /// The current time step read
    SUMOTime myCurrentTimeStep;

    /// Definition of a container for flo definitions (vector)
    typedef std::vector<FlowDef*> FlowDefV;

    /// The definitions of the read flows
    FlowDefV myFlows;

    /// A storage for ids (!!! this should be done router-wide)
    std::set<std::string> myKnownIDs;

    /// Information whether randomized departures are used
    bool myRandom;


private:
    /// @brief Invalidated copy constructor
    RORDGenerator_ODAmounts(const RORDGenerator_ODAmounts &src);

    /// @brief Invalidated assignment operator
    RORDGenerator_ODAmounts &operator=(const RORDGenerator_ODAmounts &src);

};


#endif

/****************************************************************************/

