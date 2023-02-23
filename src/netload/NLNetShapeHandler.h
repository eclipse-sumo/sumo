/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NLNetShapeHandler.h
/// @author  Jakob Erdmann
/// @date    Sept 2022
///
// The XML-Handler for loading secondary network shapes
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/SUMOSAXHandler.h>

// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================


/**
 * @class NLNetShapeHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class NLNetShapeHandler : public SUMOSAXHandler {

public:
    /** @brief Constructor
     *
     * @param[in] file Name of the parsed file
     * @param[in, out] net The network to fill
     */
    NLNetShapeHandler(const std::string& file, MSNet& net);


    /// @brief Destructor
    virtual ~NLNetShapeHandler();

    /// @brief resolve mismatch between internal lane ids of both networks
    void sortInternalShapes();

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Refactor/describe
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);

    //@}


private:

    /// adds a secondary lane shape
    void addLane(const SUMOSAXAttributes& attrs);

    /// adds a junction position
    void addJunction(const SUMOSAXAttributes& attrs);

    /// records connection topology for later resorting
    void addConnection(const SUMOSAXAttributes& attrs);

protected:
    /// @brief The net to fill (preinitialised)
    MSNet& myNet;

    /// @brief mapping between primary internal lane and corresponding secondary internal lane
    std::map<const MSJunction*, std::map<MSLane*, MSLane*> > myShuffledJunctions;

private:
    /** invalid copy constructor */
    NLNetShapeHandler(const NLNetShapeHandler& s);

    /** invalid assignment operator */
    NLNetShapeHandler& operator=(const NLNetShapeHandler& s);

};
