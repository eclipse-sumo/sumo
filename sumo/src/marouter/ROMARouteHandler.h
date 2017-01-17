/****************************************************************************/
/// @file    ROMARouteHandler.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROMARouteHandler_h
#define ROMARouteHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ODMatrix;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROMARouteHandler
 * @brief Parser and container for routes during their loading
 *
 * ROMARouteHandler transforms vehicles, trips and flows into contributions
 * to an ODMatrix.
 */
class ROMARouteHandler : public SUMOSAXHandler {
public:
    /// standard constructor
    ROMARouteHandler(ODMatrix& matrix);

    /// standard destructor
    virtual ~ROMARouteHandler();

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
    void myStartElement(int element, const SUMOSAXAttributes& attrs);

    void myEndElement(int element);
    //@}


private:
    /// @brief The matrix to fill
    ODMatrix& myMatrix;
    /// @brief The keys for reading taz
    std::vector<std::string> myTazParamKeys;
    /// @brief The current vehicle parameters
    SUMOVehicleParameter* myVehicleParameter;

private:
    /// @brief Invalidated copy constructor
    ROMARouteHandler(const ROMARouteHandler& s);

    /// @brief Invalidated assignment operator
    ROMARouteHandler& operator=(const ROMARouteHandler& s);

};


#endif

/****************************************************************************/

