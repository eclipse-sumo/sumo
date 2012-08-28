/****************************************************************************/
/// @file    RORDLoader_SUMOBase.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The base class for SUMO-native route handlers
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
#ifndef RORDLoader_SUMOBase_h
#define RORDLoader_SUMOBase_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "ROTypedXMLRoutesLoader.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVTypeParameter;
class RORouteDef;
class MsgHandler;
class RORouteDef_Alternatives;
class RORouteDef_Complete;
class RORoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORDLoader_SUMOBase
 * @brief The base class for SUMO-native route handlers
 *
 * As both the SUMO-routes and the SUMO-route alternatives handlers share
 *  a set of processing functions, we've joined these functions within a
 *  single class (this one).
 */
class RORDLoader_SUMOBase :
    public ROTypedXMLRoutesLoader {
public:
    /// Constructor
    RORDLoader_SUMOBase(RONet& net, SUMOTime begin, SUMOTime end,
                        const bool tryRepair, const bool withTaz,
                        const std::string& file = "");

    /// Destructor
    ~RORDLoader_SUMOBase();


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
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(int element);
    //@}

    /// Begins the parsing of the next route alternative in the file
    void startAlternative(const SUMOSAXAttributes& attrs);

    /// Begins the parsing of a route alternative of the opened route
    void startRoute(const SUMOSAXAttributes& attrs);

    bool closeVehicle();

    /** @brief Parses a route edge list
     *
     * @param[in] chars The read route
     * @exception ProcessError If something fails
     */
    void parseRoute(const std::string& chars);



protected:
    /// @brief The parsed vehicle parameter
    SUMOVehicleParameter* myVehicleParameter;

    /// @brief The color of the current route/vehicle
    RGBColor* myColor;

    /// @brief Information whether the currently parsed item is valid
    bool myCurrentIsOk;

    /// @brief Information whether the currently parsed alternatives set is valid
    bool myAltIsValid;

    /// @brief The currently parsed route alternatives
    RORouteDef* myCurrentAlternatives;

    /// @brief The costs of the current alternative
    SUMOReal myCost;

    /// @brief The probability of the current alternative's usage
    SUMOReal myProbability;

    /// @brief Information whether a read route shall be tried to be repaired
    bool myTryRepair;

    /// @brief Information whether zones (districts) are used as origins / destinations
    const bool myWithTaz;

    /// @brief The currently parsed route
    RORouteDef* myCurrentRoute;

    /// @brief The name of the currently parsed route
    std::string myCurrentRouteName;

    /// @brief The currently parsed vehicle type
    SUMOVTypeParameter* myCurrentVType;


private:
    /// @brief Invalidated copy constructor
    RORDLoader_SUMOBase(const RORDLoader_SUMOBase& src);

    /// @brief Invalidated assignment operator
    RORDLoader_SUMOBase& operator=(const RORDLoader_SUMOBase& src);

};


#endif

/****************************************************************************/

