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
/// @file    MeanDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The XML-Handler for meanMeanData elements loading
/****************************************************************************/
#pragma once
#include <config.h>

#include "CommonHandler.h"

// ===========================================================================
// class definitions
// ===========================================================================

class MeanDataHandler : public CommonHandler {

public:
    /// @brief Constructor
    MeanDataHandler(const std::string& filename);

    /// @brief Destructor
    virtual ~MeanDataHandler();

    /// @brief begin parse attributes
    bool beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief end parse attributes
    void endParseAttributes();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @brief run post parser tasks
    virtual bool postParserTasks() = 0;

    /// @name build functions
    /// @{

    /// @brief Builds edgeMeanData
    virtual bool buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& ID,
                                   const std::string& file, const std::string& type, const SUMOTime period, const SUMOTime begin,
                                   const SUMOTime end, const bool trackVehicles, const std::vector<std::string>& writtenAttributes,
                                   const bool aggregate, const std::vector<std::string>& edges, const std::string& edgeFile,
                                   const std::string& excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                                   const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes,
                                   const double speedThreshold) = 0;

    /// @brief Builds laneMeanData
    virtual bool buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& ID,
                                   const std::string& file, const std::string& type, const SUMOTime period, const SUMOTime begin,
                                   const SUMOTime end, const bool trackVehicles, const std::vector<std::string>& writtenAttributes,
                                   const bool aggregate, const std::vector<std::string>& edges, const std::string& edgeFile,
                                   const std::string& excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                                   const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes,
                                   const double speedThreshold) = 0;

    /// @}

private:
    /// @name parse meanMeanData attributes
    /// @{

    /// @brief parse edgeMeanData attributes
    void parseEdgeMeanData(const SUMOSAXAttributes& attrs);

    /// @brief parse laneMeanData attributes
    void parseLaneMeanData(const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief check mean data type
    bool checkType(const SumoXMLTag currentTag, const std::string& id, const std::string& type);

    /// @brief invalidate default onstructor
    MeanDataHandler() = delete;

    /// @brief invalidate copy constructor
    MeanDataHandler(const MeanDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    MeanDataHandler& operator=(const MeanDataHandler& s) = delete;
};
