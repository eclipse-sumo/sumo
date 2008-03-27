/****************************************************************************/
/// @file    RORDLoader_SUMOBase.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for SUMO-native route handlers
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
#include <utils/xml/SUMOBaseRouteHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROVehicleType;
class RORouteDef;
class MsgHandler;
class RORouteDef_Alternatives;
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
            public ROTypedXMLRoutesLoader, public SUMOBaseRouteHandler
{
public:
    /// Constructor
    RORDLoader_SUMOBase(ROVehicleBuilder &vb, RONet &net,
                        SUMOTime begin, SUMOTime end,
                        SUMOReal gawronBeta, SUMOReal gawronA,
                        int maxRouteNumber,
                        const std::string &dataName, const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMOBase() throw();

    /** @brief Returns the name of the data
        "precomputed sumo route alternatives" is returned here */
    std::string getDataName() const;

    /// Returns the time the current (last read) route starts at
    SUMOTime getCurrentTimeStep() const;

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
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    void myCharacters(SumoXMLTag element,
                      const std::string &chars) throw(ProcessError);


    /** @brief Called when a closing tag occures
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}

    /// Begins the parsing of the next route alternative in the file
    void startAlternative(const SUMOSAXAttributes &attrs);

    /// Begins the parsing of a route alternative of the opened route
    void startRoute(const SUMOSAXAttributes &attrs);

    /// Return the information whether a route was read
    bool nextRouteRead();

    /// Initialises the reading of a further route
    void beginNextRoute();

    /// Parses a vehicle type
    void startVehType(const SUMOSAXAttributes &attrs);

    MsgHandler *getErrorHandlerMarkInvalid();

    bool closeVehicle() throw();

    /// Stores color from the attributes
    bool parseVehicleColor(const SUMOSAXAttributes &attrs) throw();


protected:
    /// The type of the parsed file to allow a distinction
    std::string myDataName;

    /// The color of the current route
    std::string myColorString;

    /// The color of the current vehicle
    std::string myVehicleColorString;

    /** @brief Information whether the current route shall not be processed
        This may occure on errors */
    bool mySkipCurrent;

    /// Information whether a further route has been read
    bool myHaveNextRoute;

    /// The vehicle type currently being parsed
    ROVehicleType *myCurrentVehicleType;

    /// The current route alternatives
    RORouteDef_Alternatives *myCurrentAlternatives;

    /// The costs of the current alternative
    SUMOReal myCost;

    /// The probability of the current alternative's usage
    SUMOReal myProbability;

    /// gawron beta - value
    SUMOReal myGawronBeta;

    /// gawron beta - value
    SUMOReal myGawronA;

    /// The maximum route alternatives number
    int myMaxRouteNumber;


    RORoute *myCurrentRoute;

};


#endif

/****************************************************************************/

