/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEGeneralHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
// General element handler for NETEDIT
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/handlers/GeneralHandler.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/elements/demand/GNERouteHandler.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEGeneralHandler : public GeneralHandler {

public:
    /**@brief Constructor
     * @param[in] net GNENet
     * @param[in] file Name of the parsed file
     * @param[in] allowUndoRedo enable or disable undoRedo
     */
    GNEGeneralHandler(GNENet* net, const std::string& file, const bool allowUndoRedo);

    /// @brief Destructor
    ~GNEGeneralHandler();

private:
    /// @brief tagType
    struct TagType {
        /// @brief constructor
        TagType(SumoXMLTag tag, const bool additional, const bool demand);
        
        /// @brief tag related with this TagType
        const SumoXMLTag tag;

        /// @brief tagType is additional
        const bool additional;

        /// @brief tagType is demand
        const bool demand;
    };

    /// @brief queue with the inserted tags
    std::list<TagType> myQueue;

    /// @brief additional handler
    GNEAdditionalHandler myAdditionalHandler;

    /// @brief demand handler
    GNERouteHandler myDemandHandler;

    /// @brief start element
    void beginTag(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief end element
    void endTag();

    /// @brief invalidate copy constructor
    GNEGeneralHandler(const GNEGeneralHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEGeneralHandler& operator=(const GNEGeneralHandler& s) = delete;
};

/****************************************************************************/

