/****************************************************************************/
/// @file    RORDLoader_TripDefs.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for loading trip definitions
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
    RORDLoader_TripDefs(RONet &net, SUMOTime begin, SUMOTime end,
                        bool emptyDestinationsAllowed, const std::string &file="") throw(ProcessError);

    /// Destructor
    ~RORDLoader_TripDefs() throw();


    /// @name inherited from ROAbstractRouteDefLoader
    //@{

    /** @brief Returns the time the current (last read) route starts at
     *
     * @return The least time step that was read by this reader
     */
    SUMOTime getLastReadTimeStep() const throw() {
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



    /// @name inherited from ROTypedXMLRoutesLoader
    /// @{

    /** Returns the information whether a route was read
     *
     * @return Whether a further route was read
     * @see ROTypedXMLRoutesLoader::nextRouteRead
     */
    bool nextRouteRead() throw() {
        return myNextRouteRead;
    }


    /** @brief Returns Initialises the reading of a further route
     *
     * @todo recheck/refactor
     * @see ROTypedXMLRoutesLoader::beginNextRoute
     */
    void beginNextRoute() throw();
    //@}


protected:
    /// Parses the vehicle id
    std::string getVehicleID(const SUMOSAXAttributes &attrs);

    /// Parses a named edge frm the attributes
    ROEdge *getEdge(const SUMOSAXAttributes &attrs, const std::string &purpose,
                    SumoXMLAttr which, const std::string &id, bool emptyAllowed);

    /** @brief Parses a SUMOReal value which is optional;
        Avoids exceptions */
    SUMOReal getOptionalFloat(const SUMOSAXAttributes &attrs,
                              const std::string &name, SumoXMLAttr which, const std::string &place);

    /// Parses and returns the period the trip shall be repeated with
    SUMOTime getPeriod(const SUMOSAXAttributes &attrs, const std::string &id);

    /// Parses and returns the number of cars that shall use the same parameter
    int getRepetitionNumber(const SUMOSAXAttributes &attrs, const std::string &id);

    /** @brief Parses and returns the name of the lane the vehicle starts at
        This value is optional */
    std::string getLane(const SUMOSAXAttributes &attrs);

protected:
    /// generates numerical ids
    IDSupplier myIdSupplier;

    /// The starting edge
    ROEdge *myBeginEdge;

    /// The end edge
    ROEdge *myEndEdge;

    /** @brief Information whether empty destinations are allowed
        This is a feature used for the handling of explicite routes within the
        jp-router where the destination is not necessary */
    bool myEmptyDestinationsAllowed;

    /// The information whether the next route was read
    bool myNextRouteRead;

    /// @brief The currently parsed vehicle type
    SUMOVTypeParameter *myCurrentVehicleType;

    SUMOVehicleParameter *myParameter;
    SUMOTime myDepartureTime;

private:
    /// @brief Invalidated copy constructor
    RORDLoader_TripDefs(const RORDLoader_TripDefs &src);

    /// @brief Invalidated assignment operator
    RORDLoader_TripDefs &operator=(const RORDLoader_TripDefs &src);

};


#endif

/****************************************************************************/

