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
#include <utils/gfx/RGBColor.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOBaseRouteHandler.h>


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class ROVehicleType;
class RORouteDef;
class MsgHandler;


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
    /** the user-impemlented handler method for an opening tag */
    virtual void myStartElement(SumoXMLTag element,
                                const Attributes &attrs) throw();

    /** the user-implemented handler method for a closing tag */
    virtual void myEndElement(SumoXMLTag element) throw();
    //@}

    /// Return the information whether a route was read
    bool nextRouteRead();

    /// Initialises the reading of a further route
    void beginNextRoute();

    /// Parses a vehicle type
    void startVehType(const Attributes &attrs);

    MsgHandler *getErrorHandlerMarkInvalid();

    /// begins the processing of a route
    virtual void startRoute(const Attributes &attrs) = 0;

    void closeVehicle() throw();


protected:
    /// The type of the parsed file to allow a distinction
    std::string myDataName;

    /// The color of the current route
    RGBColor myCurrentColor;

    /** @brief Information whether the current route shall not be processed
        This may occure on errors */
    bool mySkipCurrent;

    /// Information whether a further route has been read
    bool myHaveNextRoute;

    /// The vehicle type currently being parsed
    ROVehicleType *myCurrentVehicleType;

};


#endif

/****************************************************************************/

