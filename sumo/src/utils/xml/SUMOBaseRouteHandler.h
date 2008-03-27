/****************************************************************************/
/// @file    SUMOBaseRouteHandler.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 12.12.2005
/// @version $Id$
///
// Base class for parsing vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOBaseRouteHandler_h
#define SUMOBaseRouteHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOBaseRouteHandler
 * @brief Base class for vehicle parsing.
 *
 * Because vehicles have to be parsed by several applications (SUMO, GUISIM,
 *  routers) the same way, some of the methods are shared within this class which
 *  is meant to be a common base for vehicle parsers.
 *
 * This class supports helper methods for parsing a vehicle's color, class and
 *  other. Still, the main usage is to call "openVehicle" on an opening vehicle-tag
 *  what parses the information about the vehicle stored in the attributes. Then, when
 *  the vehicle-tag is closed, "closeVehicle" should be called and the vehicle may
 *  be instantiated using the read values.
 */
class SUMOBaseRouteHandler
{
public:
    /// Constructor
    SUMOBaseRouteHandler() throw();


    /// Destructor
    virtual ~SUMOBaseRouteHandler() throw();


protected:
    /// @name parse helper (reporting errors)
    //@{

    /**
     * @brief Parses the vehicle class
     *
     * When given, the vehicle class is parsed using getVehicleClassID.
     *  Exceptions occuring within this process are catched and reported.
     *
     * If no vehicle class is available in the attributes, the default class (SVC_UNKNOWN)
     *  is returned.
     */
    SUMOVehicleClass parseVehicleClass(const SUMOSAXAttributes &attrs, const std::string &type,
                                       const std::string &id) throw();


    /**
     * @brief Parses the vehicle departure time
     *
     * Tries to parse the vehicle's departure time. If it is not within the attributes,
     *  is empty or not a SUMOTime, a -1 will be returned.
     */
    SUMOTime getVehicleDepartureTime(const SUMOSAXAttributes &attrs, const std::string &id) throw();


    /**
     * @brief Parses the vehicle color
     *
     * This is to be overriden in subclasses which really need the color.
     * Default implementation does nothing and returns true.
     */
    virtual bool parseVehicleColor(const SUMOSAXAttributes &attrs) throw();


    /**
     * @brief Parses the information from the vehicle-element; Returns whether it succeeded
     *
     * Tries to parse: the vehicle id, the repetition number and offset, the route
     *  name, the name of the vehicle type and the departure time.
     *
     * Error messages are generated if something fails. The method returns false if one
     *  of the mandatory values is missing: the vehicle id, the departure time and if
     *  the vehicle type is given but empty.
     *
     * The method also sets myAmInEmbeddedMode to true which allows starting to process
     *  additional, embedded values.
     */
    bool openVehicle(const SUMOSAXAttributes &attrs) throw();


    /** @brief Closes the processing of a vehicle
     *
     * The method sets myAmInEmbeddedMode to false ending processing additional,
     *  embedded values.
     */
    virtual bool closeVehicle() throw(ProcessError);
    //@}


protected:
    /// @name variables used during parsing of vehicles
    //@{

    /// the id of the current vehicle
    std::string myActiveVehicleID;

    /// Information about how often vehicles like the currently read shall be emitted
    int myRepNumber;

    /// Information about the dureation of the period between such emissions
    int myRepOffset;

    /// name of the currently read vehicle's type
    std::string myCurrentVType;

    /// name of the currently read vehicle's route
    std::string myCurrentRouteName;

    /// The currently read vehicle's depart
    SUMOTime myCurrentDepart;

    /// information whether the route is read in vehicle-embedded mode
    bool myAmInEmbeddedMode;
    //@}

    /// information whether an error occured
    bool myHadErrors;

};


#endif

/****************************************************************************/

