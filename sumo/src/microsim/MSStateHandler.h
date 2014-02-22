/****************************************************************************/
/// @file    MSStateHandler.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Thu, 13 Dec 2012
/// @version $Id$
///
// Parser and output filter for routes and vehicles state saving and loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifdef HAVE_INTERNAL
class MESegment;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSStateHandler
 * @brief Parser and output filter for routes and vehicles state saving and loading
 */
class MSStateHandler : public SUMOSAXHandler {
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


private:
    const SUMOTime myOffset;
    SUMOTime myTime;
#ifdef HAVE_INTERNAL
    MESegment* mySegment;
#endif
    std::pair<int, int> myEdgeAndLane;
    unsigned int myQueIndex;

    /// @brief The currently parsed vehicle type
    SUMOVTypeParameter* myCurrentVType;

private:
    /// @brief Invalidated copy constructor
    MSStateHandler(const MSStateHandler& s);

    /// @brief Invalidated assignment operator
    MSStateHandler& operator=(const MSStateHandler& s);

};


#endif

/****************************************************************************/
