/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMeanDataHandler : public MeanDataHandler {

public:
    /// @brief Constructor
    GNEMeanDataHandler(GNENet* net, const std::string& filename, const bool allowUndoRedo);

    /// @brief Destructor
    virtual ~GNEMeanDataHandler();

    /// @brief run post parser tasks
    bool postParserTasks();

    /// @name build functions
    /// @{

    /// @brief Builds edgeMeanData
    bool buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id,
                           const std::string& file, const std::string& type, const SUMOTime period, const SUMOTime begin,
                           const SUMOTime end, const bool trackVehicles, const std::vector<std::string>& writtenAttributes,
                           const bool aggregate, const std::vector<std::string>& edgeIDs, const std::string& edgeFile,
                           const std::string& excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                           const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes,
                           const double speedThreshold);

    /// @brief Builds laneMeanData
    bool buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id,
                           const std::string& file, const std::string& type, const SUMOTime period, const SUMOTime begin,
                           const SUMOTime end, const bool trackVehicles, const std::vector<std::string>& writtenAttributes,
                           const bool aggregate, const std::vector<std::string>& edgeIDs, const std::string& edgeFile,
                           const std::string& excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                           const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes,
                           const double speedThreshold);

    /// @}

protected:
    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief allow undo/redo
    const bool myAllowUndoRedo;

    /// @brief parse edges
    std::vector<GNEEdge*> parseEdges(const SumoXMLTag tag, const std::vector<std::string>& edgeIDs);

    /// @brief parse attributes
    std::vector<SumoXMLAttr> parseAttributes(const SumoXMLTag tag, const std::vector<std::string>& attrStrs);

    /// @brief check if given ID correspond to a duplicated mean data element
    bool checkDuplicatedMeanDataElement(const SumoXMLTag tag, const std::string& id);

    /// @brief check if given excludeEmpty is valid
    bool checkExcludeEmpty(const SumoXMLTag tag, const std::string& id, const std::string& excludeEmpty);

private:
    /// @brief invalidate default onstructor
    GNEMeanDataHandler() = delete;

    /// @brief invalidate copy constructor
    GNEMeanDataHandler(const GNEMeanDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEMeanDataHandler& operator=(const GNEMeanDataHandler& s) = delete;
};
