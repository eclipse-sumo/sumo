/****************************************************************************/
/// @file    RORDLoader_TripDefs.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for loading trip definitions
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
#ifndef RORDLoader_TripDefs_h
#define RORDLoader_TripDefs_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/common/IDSupplier.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORDLoader_TripDefs
 * A handler for route definitions which consists of the origin and the
 * destination edge only. Additionally, read vehicles may have information
 * about a certain position to leave from and a certain speed to leave with.
 */
class RORDLoader_TripDefs : public ROTypedXMLRoutesLoader {
public:
    /// Constructor
    RORDLoader_TripDefs(RONet& net, SUMOTime begin, SUMOTime end,
                        bool emptyDestinationsAllowed, bool withTaz,
                        const std::string& file = "") ;

    /// Destructor
    ~RORDLoader_TripDefs() ;


    /// @name inherited from ROAbstractRouteDefLoader
    //@{

    /** @brief Returns the time the current (last read) route starts at
     *
     * @return The least time step that was read by this reader
     */
    SUMOTime getLastReadTimeStep() const {
        return myDepartureTime;
    }
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
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs) ;


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element) ;
    //@}



    /// @name inherited from ROTypedXMLRoutesLoader
    /// @{

    /** Returns the information whether a route was read
     *
     * @return Whether a further route was read
     * @see ROTypedXMLRoutesLoader::nextRouteRead
     */
    bool nextRouteRead() {
        return myNextRouteRead;
    }


    /** @brief Returns Initialises the reading of a further route
     *
     * @todo recheck/refactor
     * @see ROTypedXMLRoutesLoader::beginNextRoute
     */
    void beginNextRoute() ;
    //@}


protected:
    /// Parses the vehicle id
    std::string getVehicleID(const SUMOSAXAttributes& attrs);

    /// Parses a named edge frm the attributes
    ROEdge* getEdge(const SUMOSAXAttributes& attrs, const std::string& purpose,
                    SumoXMLAttr which, const std::string& id, bool emptyAllowed);

protected:
    /// generates numerical ids
    IDSupplier myIdSupplier;

    /// The starting edge
    ROEdge* myBeginEdge;

    /// The end edge
    ROEdge* myEndEdge;

    /** @brief Information whether empty destinations are allowed
        This is a feature used for the handling of explicit routes within the
        jtrrouter where the destination is not necessary */
    const bool myEmptyDestinationsAllowed;

    /// @brief Information whether zones (districts) are used as origins / destinations
    const bool myWithTaz;

    /// The information whether the next route was read
    bool myNextRouteRead;

    /// @brief The currently parsed vehicle type
    SUMOVTypeParameter* myCurrentVehicleType;

    SUMOVehicleParameter* myParameter;
    SUMOTime myDepartureTime;

    bool myHaveWarnedAboutDeprecatedTripDef;

private:
    /// @brief Invalidated copy constructor
    RORDLoader_TripDefs(const RORDLoader_TripDefs& src);

    /// @brief Invalidated assignment operator
    RORDLoader_TripDefs& operator=(const RORDLoader_TripDefs& src);

};


#endif

/****************************************************************************/

