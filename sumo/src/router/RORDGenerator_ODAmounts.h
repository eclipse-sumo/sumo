/****************************************************************************/
/// @file    RORDGenerator_ODAmounts.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Class for loading trip amount definitions and route generation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
// class declarations
// ===========================================================================
class RORouteDef_OrigDest;


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
    RORDGenerator_ODAmounts(RONet& net,
                            SUMOTime begin, SUMOTime end, bool emptyDestinationsAllowed,
                            bool randomize, const std::string& file = "");

    /// Destructor
    ~RORDGenerator_ODAmounts();


    /** @brief Adds routes from the file until the given time is reached
     *
     * @param[in] time The time until which route definitions shall be loaded
     * @return Whether any errors occured
     * @exception ProcessError If a major error occured
     * @see ROAbstractRouteDefLoader::readRoutesAtLeastUntil
     */
    bool readRoutesAtLeastUntil(SUMOTime until);


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
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element);
    //@}

    /// Parses the trip amount definition
    void parseFlowAmountDef(const SUMOSAXAttributes& attrs);

    /// Closes the trip amount parsing
    void myEndFlowAmountDef();

    /**
     * @class FlowDef
     * The definition of a flow.
     */
    class FlowDef {
    public:
        /// Constructor
        FlowDef(ROVehicle* vehicle, SUMOVTypeParameter* type, RORouteDef* route,
                SUMOTime intBegin, SUMOTime intEnd,
                unsigned int vehicles2insert, bool randomize);

        /// Destructor
        ~FlowDef();

        /// Returns the information whether this flow includes the given time
        bool applicableForTime(SUMOTime time) const;

        /// Adds routes to start within the given time into the given net
        void addRoutes(RONet& net, SUMOTime time);

        /// Adds a single route to start within the given time into the given net
        void addSingleRoute(RONet& net, SUMOTime time);

        /// Returns the end of the period this FlowDef describes
        SUMOTime getIntervalEnd() const;

    private:
        /// (description of) the vehicle to insert
        const ROVehicle* const myVehicle;

        /// (description of) the vehicle type to use
        const SUMOVTypeParameter* const myVehicleType;

        /// (description of) the route to use
        const RORouteDef* const myRoute;

        /// The begin of the described interval
        const SUMOTime myIntervalBegin;

        /// The end of the described interval
        const SUMOTime myIntervalEnd;

        /// The number of vehicles to insert within the described interval
        const unsigned int myVehicle2InsertNumber;

        /// The number of vehicles already inserted
        unsigned int myInserted;

        /// The list of generated departure times in the case randomized departures are used
        std::vector<SUMOTime> myDepartures;

        /// Information whether randomized departures are used
        bool myRandom;

    private:
        /** @brief invalidated assignment operator */
        FlowDef& operator=(const FlowDef& s);

    };

private:
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

    /// The number of vehicles to insert
    unsigned int myVehicle2InsertNumber;

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
    RORDGenerator_ODAmounts(const RORDGenerator_ODAmounts& src);

    /// @brief Invalidated assignment operator
    RORDGenerator_ODAmounts& operator=(const RORDGenerator_ODAmounts& src);

};


#endif

/****************************************************************************/

