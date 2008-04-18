/****************************************************************************/
/// @file    RORDGenerator_Random.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 9. Apr 2003
/// @version $Id$
///
// A "trip loader" for random trip generation
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
#ifndef RORDGenerator_Random_h
#define RORDGenerator_Random_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/IDSupplier.h>
#include <utils/common/RGBColor.h>
#include <utils/common/UtilExceptions.h>
#include "ROAbstractRouteDefLoader.h"


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORDGenerator_Random
 * Base class for loaders of route which do have a certain format. XML-reading
 * loaders are not derived directly, but use the derived ROTypedXMLRoutesLoader
 * class as their upper class.
 */
class RORDGenerator_Random :
            public ROAbstractRouteDefLoader
{
public:
    /// Constructor
    RORDGenerator_Random(ROVehicleBuilder &vb, RONet &net,
                         SUMOTime begin, SUMOTime end, bool removeFirst) throw(ProcessError);

    /// Destructor
    ~RORDGenerator_Random() throw();


    /// @name inherited from ROAbstractRouteDefLoader
    //@{

    /** @brief Returns the name of the read type
     *
     * @return The name of the data
     */
    std::string getDataName() const throw() {
        return "random_trips";
    }


    /** @brief Adds routes from the file until the given time is reached
     *
     * @param[in] time The time until which route definitions shall be loaded
     * @param[in] skipping Whether routes shall not be added
     * @return Whether any errors occured
     * @see ROAbstractRouteDefLoader::readRoutesAtLeastUntil
     */
    bool readRoutesAtLeastUntil(SUMOTime time, bool skipping) throw();


    /** @brief Returns the time the current (last read) route starts at
     *
     * @return The least time step that was read by this reader
     */
    SUMOTime getLastReadTimeStep() const throw() {
        return myCurrentTime;
    }


    /** @brief Returns the information whether no routes are available from this loader anymore
     *
     * @return Whether the whole input has been processed
     */
    bool ended() const throw() {
        return false;
    }
    /// @}


private:
    /** @brief The number of cars to emit per second
        Values lower than one are allowed, too */
    SUMOReal myWishedPerSecond;

    /** The counter for vehicle emission */
    SUMOReal myCurrentProgress;

    /** The supplier for ids */
    IDSupplier myIDSupplier;

    /// The color random routes shall have
    RGBColor myColor;

    /// The (pseudo) information about the current time
    SUMOTime myCurrentTime;

    /// The information whether a new route was read
    bool myReadNewRoute;

    /// Information whether the first and last edge shall be removed
    bool myRemoveFirst;


private:
    /// @brief Invalidated copy constructor
    RORDGenerator_Random(const RORDGenerator_Random &src);

    /// @brief Invalidated assignment operator
    RORDGenerator_Random &operator=(const RORDGenerator_Random &src);

};


#endif

/****************************************************************************/

