/****************************************************************************/
/// @file    ROAbstractRouteDefLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for loading routes or route definitions
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
#ifndef ROAbstractRouteDefLoader_h
#define ROAbstractRouteDefLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROLoader;
class RONet;
class OptionsCont;
class ROVehicleBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROAbstractRouteDefLoader
 * @brief The basic class for loading routes or route definitions
 *
 * XML-reading loaders are not derived directly, but use the derived
 *  ROTypedXMLRoutesLoader class as their upper class.
 */
class ROAbstractRouteDefLoader
{
public:
    /// Constructor
    ROAbstractRouteDefLoader(ROVehicleBuilder &vb, RONet &net,
                             SUMOTime begin, SUMOTime end, const std::string &file="");

    /// Destructor
    virtual ~ROAbstractRouteDefLoader();

    /** @brief Skips routes which begin before the given time
     *
     * This method uses the method myReadRoutesAtLeastUntil(time) to overread
     * the first routes, so the loaders must determine by themselves whether
     * to build a route or not (the departure time has to be between myBegin
     * and the given timestep */
    void skipUntilBegin();

    /// Adds routes from the file until the given time is reached
    void readRoutesAtLeastUntil(SUMOTime time);

    /// Closes the reading of the routes
    virtual void closeReading() = 0;

    /// Returns the name of the route type
    virtual std::string getDataName() const = 0;

    /// Initialises the reader
    virtual bool init(OptionsCont &options) = 0;

    /// Returns the time the current (last read) route starts at
    virtual SUMOTime getCurrentTimeStep() const = 0;

    /// Returns the information whether no routes are available from this loader anymore
    virtual bool ended() const = 0;

    friend class ROLoader;

protected:
    /** @brief Builds routes
     *
     * All routes between the loader's current time step and the one given shall
     * be processed. If the route's departure time is lower than the value of
     * "myBegin", the route should not be added into the container. */ // !!! not very good
    virtual bool myReadRoutesAtLeastUntil(SUMOTime time) = 0;

protected:
    /// The network to add routes to
    RONet &myNet;

    /// The time for which the first route shall be compute
    SUMOTime myBegin;

    /// The time for which the first route shall be compute
    SUMOTime myEnd;

    /// The vehicle builder to use
    ROVehicleBuilder &myVehicleBuilder;

};


#endif

/****************************************************************************/

