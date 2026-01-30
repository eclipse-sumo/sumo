/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    METypeHandler.h
/// @author  Jakob Erdmann
/// @date    Jan 2026
///
// The XML-Handler for loading meso edge types
// This is a dedicated handler because meso types must be loaded from additional
// files before any other objects are loaded from them
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/SUMOSAXHandler.h>

// ===========================================================================
// class definitions
// ===========================================================================

class MSNet;

/**
 * @class METypeHandler
 * @brief The XML-Handler for meso edge type loading
 */
class METypeHandler : public SUMOSAXHandler {

public:
    /** @brief Constructor
     *
     * @param[in] file Name of the parsed file
     * @param[in, out] net The network to fill
     * @param[in] detBuilder The detector builder to use
     * @param[in] triggerBuilder The trigger builder to use
     * @param[in] edgeBuilder The builder of edges to use
     * @param[in] junctionBuilder The builder of junctions to use
     */
    METypeHandler(const std::string& file, MSNet& net);


    /// @brief Destructor
    virtual ~METypeHandler();

    bool haveSeenMesoEdgeType() const {
        return myHaveSeenMesoEdgeType;
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
     * @todo Refactor/describe
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     * @todo Refactor/describe
     */
    void myEndElement(int element);
    //@}


protected:
    /** @brief Loads edge type specific meso parameters
     * @param[in] attrs The attributes that hold the parameters
     */
    void addMesoEdgeType(const SUMOSAXAttributes& attrs);


protected:
    /// @brief The net to fill (preinitialised)
    MSNet& myNet;

    /// The id of the currently processed edge type
    std::string myCurrentTypeID;

    /// @brief whether edge type specific meso parameters were loaded
    bool myHaveSeenMesoEdgeType;

private:
    /** invalid copy constructor */
    METypeHandler(const METypeHandler& s);

    /** invalid assignment operator */
    METypeHandler& operator=(const METypeHandler& s);

};
