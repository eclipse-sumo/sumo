/****************************************************************************/
/// @file    MSStateHandler.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Thu, 13 Dec 2012
/// @version $Id$
///
// Parser and output filter for routes and vehicles state saving and loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSStateHandler_h
#define MSStateHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include "MSRouteHandler.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MESegment;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSStateHandler
 * @brief Parser and output filter for routes and vehicles state saving and loading
 */
class MSStateHandler : public MSRouteHandler {
public:
    /// standard constructor
    MSStateHandler(const std::string& file, const SUMOTime offset);

    /// standard destructor
    virtual ~MSStateHandler();

    /** @brief Saves the current state
     *
     * @param[in] file The file to write the state into
     */
    static void saveState(const std::string& file, SUMOTime step);

    SUMOTime getTime() const {
        return myTime;
    }

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

    /// Ends the processing of a vehicle
    void closeVehicle();

private:
    const SUMOTime myOffset;
    SUMOTime myTime;
    MESegment* mySegment;
    std::pair<int, int> myEdgeAndLane;
    int myQueIndex;

    /// @brief cached attrs (used when loading vehicles)
    SUMOSAXAttributes* myAttrs;

    /// @brief the last object that potentially carries parameters
    Parameterised* myLastParameterised;

    /// @brief vehicles that shall be removed when loading state
    std::set<std::string> myVehiclesToRemove;


private:
    /// @brief Invalidated copy constructor
    MSStateHandler(const MSStateHandler& s);

    /// @brief Invalidated assignment operator
    MSStateHandler& operator=(const MSStateHandler& s);

};


#endif

/****************************************************************************/
