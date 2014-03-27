/****************************************************************************/
/// @file    ODAmitranHandler.h
/// @author  Michael Behrisch
/// @date    27.03.2014
/// @version $Id$
///
// An XML-Handler for Amitran OD matrices
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ODAmitranHandler_h
#define ODAmitranHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ODMatrix;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODAmitranHandler
 * @brief An XML-Handler for districts
 *
 * This SUMOSAXHandler parses OD matrices in the Amitran format.
 */
class ODAmitranHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     *
     * Saves the given matrix in order to fill it.
     * @param[in] cont The matrix to fill
     * @param[in] file The file that will be processed
     */
    ODAmitranHandler(ODMatrix& matrix, const std::string& file);


    /// @brief Destructor
    ~ODAmitranHandler();


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called when an opening-tag occurs
     *
     * Processes district elements via openDistrict, their sinks (via
     *  addSink) and sources (via addSource).
     *
     * @param[in] element The enum of the currently opened element
     * @param[in] attrs Attributes of the currently opened element
     * @exception ProcessError If an error within the parsed file occurs
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs);
    //@}


private:
    /// The matrix to add demand to
    ODMatrix& myMatrix;

    std::string myVehicleType;

    SUMOTime myBegin;
    SUMOTime myEnd;
private:
    /// @brief invalidated copy constructor
    ODAmitranHandler(const ODAmitranHandler& s);

    /// @brief invalidated assignment operator
    ODAmitranHandler& operator=(const ODAmitranHandler& s);


};


#endif

/****************************************************************************/
