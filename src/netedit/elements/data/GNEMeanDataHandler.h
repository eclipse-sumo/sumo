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
/// @file    GNEMeanDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 22
///
// Builds meanData objects for netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/handlers/MeanDataHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNEMeanDataHandler
class GNEMeanDataHandler : public MeanDataHandler {

public:
    /// @brief Constructor
    GNEMeanDataHandler(GNENet* net, const std::string& file, const bool allowUndoRedo);

    /// @brief Destructor
    ~GNEMeanDataHandler();

    /// @name build functions
    /// @{
    /**@brief Builds edgeMeanData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] edgeID edge id
     * @param[in] file path to file output
     */
    void buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
        const std::string& edgeID, const std::string& file);

    /**@brief Builds laneMeanData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] laneID lane id
     * @param[in] file path to file output
     */
    void buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
        const std::string& laneID, const std::string& file);

    /// @}

protected:
    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief allow undo/redo
    const bool myAllowUndoRedo;

private:
    /// @brief invalidate copy constructor
    GNEMeanDataHandler(const GNEMeanDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEMeanDataHandler& operator=(const GNEMeanDataHandler& s) = delete;
};
