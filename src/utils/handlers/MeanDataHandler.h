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
/// @file    MeanDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The XML-Handler for meanMeanData elements loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MeanDataHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class MeanDataHandler {

public:
    /// @brief Constructor
    MeanDataHandler();

    /// @brief Destructor
    virtual ~MeanDataHandler();

    /// @brief begin parse attributes
    bool beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief end parse attributes
    void endParseAttributes();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @name build functions
    /// @{
    /// @brief Builds edgeMeanData
    virtual void buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& ID,
                                   const std::string& file, SUMOTime period, SUMOTime begin, SUMOTime end, const bool trackVehicles,
                                   const std::vector<std::string>& writtenAttributes, const bool aggregate, const std::vector<std::string>& edges,
                                   const std::string& edgeFile, std::string excludeEmpty, const bool withInternal,
                                   const std::vector<std::string>& detectPersons, const double minSamples, const double maxTravelTime,
                                   const std::vector<std::string>& vTypes, const double speedThreshold) = 0;

    /// @brief Builds laneMeanData
    virtual void buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& ID,
                                   const std::string& file, SUMOTime period, SUMOTime begin, SUMOTime end, const bool trackVehicles,
                                   const std::vector<std::string>& writtenAttributes, const bool aggregate, const std::vector<std::string>& edges,
                                   const std::string& edgeFile, std::string excludeEmpty, const bool withInternal,
                                   const std::vector<std::string>& detectPersons, const double minSamples, const double maxTravelTime,
                                   const std::vector<std::string>& vTypes, const double speedThreshold) = 0;

    /// @}

    /// @brief get flag for check if a element wasn't created
    bool isErrorCreatingElement() const;

protected:
    /// @brief write error and enable error creating element
    void writeError(const std::string& error);

private:
    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @name parse meanMeanData attributes
    /// @{
    /// @brief parse edgeMeanData attributes
    void parseEdgeMeanData(const SUMOSAXAttributes& attrs);

    /// @brief parse laneMeanData attributes
    void parseLaneMeanData(const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief invalidate copy constructor
    MeanDataHandler(const MeanDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    MeanDataHandler& operator=(const MeanDataHandler& s) = delete;
};
