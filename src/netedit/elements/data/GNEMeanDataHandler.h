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
    GNEMeanDataHandler(GNENet* net, const bool allowUndoRedo, const bool overwrite);

    /// @brief Destructor
    virtual ~GNEMeanDataHandler();

    /// @name build functions
    /// @{
    /// @brief Builds edgeMeanData
    void buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& ID,
                           const std::string& file, SUMOTime period, SUMOTime begin, SUMOTime end, const bool trackVehicles,
                           const std::vector<std::string>& writtenAttributes, const bool aggregate, const std::vector<std::string>& edgeIDs,
                           const std::string& edgeFile, std::string excludeEmpty, const bool withInternal,
                           const std::vector<std::string>& detectPersons, const double minSamples, const double maxTravelTime,
                           const std::vector<std::string>& vTypes, const double speedThreshold);

    /// @brief Builds laneMeanData
    void buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& ID,
                           const std::string& file, SUMOTime period, SUMOTime begin, SUMOTime end, const bool trackVehicles,
                           const std::vector<std::string>& writtenAttributes, const bool aggregate, const std::vector<std::string>& edgeIDs,
                           const std::string& edgeFile, std::string excludeEmpty, const bool withInternal,
                           const std::vector<std::string>& detectPersons, const double minSamples, const double maxTravelTime,
                           const std::vector<std::string>& vTypes, const double speedThreshold);

    /// @}

protected:
    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief allow undo/redo
    const bool myAllowUndoRedo;

    /// @brief check if overwrite
    const bool myOverwrite;

    /// @brief parse edges
    std::vector<GNEEdge*> parseEdges(const SumoXMLTag tag, const std::vector<std::string>& edgeIDs);

    /// @brief parse attributes
    std::vector<SumoXMLAttr> parseAttributes(const SumoXMLTag tag, const std::vector<std::string>& attrStrs);

private:
    /// @brief invalidate copy constructor
    GNEMeanDataHandler(const GNEMeanDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEMeanDataHandler& operator=(const GNEMeanDataHandler& s) = delete;
};
