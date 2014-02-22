/****************************************************************************/
/// @file    RODFDetectorHandler.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// missing_desc
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
#ifndef RODFDetectorHandler_h
#define RODFDetectorHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/options/OptionsCont.h>
#include "RODFDetector.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFDetectorHandler
 * @brief SAX2-Handler for loading DFROUTER-detector definitions
 */
class RODFDetectorHandler : public SUMOSAXHandler {
public:
    /// Constructor
    RODFDetectorHandler(RODFNet* optNet, bool ignoreErrors, RODFDetectorCon& con,
                        const std::string& file);

    /// Destructor
    virtual ~RODFDetectorHandler();

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
    //@}

private:
    /// the net
    RODFNet* myNet;

    /// whether to ignore errors on parsing
    bool myIgnoreErrors;

    /// the container to put the detectors into
    RODFDetectorCon& myContainer;


private:
    /// invalidated copy constructor
    RODFDetectorHandler(const RODFDetectorHandler& src);

    /// invalidated assignment operator
    RODFDetectorHandler& operator=(const RODFDetectorHandler& src);

};


#endif

/****************************************************************************/

