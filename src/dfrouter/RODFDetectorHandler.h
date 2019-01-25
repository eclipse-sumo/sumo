/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef RODFDetectorHandler_h
#define RODFDetectorHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

