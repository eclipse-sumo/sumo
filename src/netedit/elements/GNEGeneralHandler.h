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
/// @file    GNEGeneralHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
// General element handler for netedit
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/handlers/GeneralHandler.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/elements/data/GNEMeanDataHandler.h>

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
     * @param[in] overwrite enable or disable overwrite elements
     */
    GNEGeneralHandler(GNENet* net, const std::string& file, const bool allowUndoRedo, const bool overwrite);

    /// @brief Destructor
    ~GNEGeneralHandler();

    /// @brief get flag for check if a element wasn't created
    bool isErrorCreatingElement() const;

    /// @brief check if the parser file is a additional file
    bool isAdditionalFile() const;

    /// @brief check if the parser file is a route file
    bool isRouteFile() const;

    /// @brief check if the parser file is a meanData file
    bool isMeanDataFile() const;

private:
    /// @brief tagType
    struct TagType {

        enum class Type {
            NONE,
            NETWORK,
            ADDITIONAL,
            DEMAND,
            DATA,
            MEANDATA,
        };

        /// @brief constructor
        TagType(SumoXMLTag tag, Type type);

        /// @brief is network element
        bool isNetwork() const;

        /// @brief is network element
        bool isAdditional() const;

        /// @brief is network element
        bool isDemand() const;

        /// @brief is network element
        bool isData() const;

        /// @brief is network element
        bool isMeanData() const;

        /// @brief tag related with this TagType
        const SumoXMLTag tag;

    private:
        /// @brief tag type
        const Type myType;
    };

    /// @brief queue with the inserted tags
    std::list<TagType> myQueue;

    /// @brief additional handler
    GNEAdditionalHandler myAdditionalHandler;

    /// @brief demand handler
    GNERouteHandler myDemandHandler;

    /// @brief meanData handler
    GNEMeanDataHandler myMeanDataHandler;

    /// @brief flag for set file type
    TagType::Type fileType = TagType::Type::NONE;

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

