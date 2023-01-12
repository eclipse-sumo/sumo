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
/// @file    GNEMeanData.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Class for representing MeanData
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEHierarchicalElement.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEMeanData
 * @brief An Element which don't belong to GNENet but has influence in the simulation
 */
class GNEMeanData : public GNEHierarchicalElement {

public:
    /// @brief Default constructor
    GNEMeanData(GNENet* net, SumoXMLTag tag, const std::string& id);

    /// @brief Parameter constructor
    GNEMeanData(GNENet* net, SumoXMLTag tag, std::string ID, std::string file, SUMOTime period,
                SUMOTime begin, SUMOTime end, const bool trackVehicles, const std::vector<SumoXMLAttr>& writtenAttributes,
                const bool aggregate, const std::vector<std::string>& edges, const std::string& edgeFile,
                std::string excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes, const double speedThreshold);

    /// @brief Destructor
    ~GNEMeanData();

    /**@brief write meanData element into a xml file
     * @param[in] device device in which write parameters of meanData element
     */
    void writeMeanData(OutputDevice& device) const;

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns element position in view
    Position getPositionInView() const;

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const;

    /**@brief method for setting the attribute and letting the object perform data set changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

protected:
    /// @brief id
    std::string myID;

    /// @brief filename
    std::string myFile;

    /// @brief period
    SUMOTime myPeriod = 0;

    /// @brief begin
    SUMOTime myBegin = 0;

    /// @brief end
    SUMOTime myEnd = 0;

    /// @brief Whether vehicles are tracked
    bool myTrackVehicles = false;

    /// @brief bit mask for checking attributes to be written
    std::vector<SumoXMLAttr> myWrittenAttributes;

    /// @brief whether the data for all edges shall be aggregated
    bool myAggregate = false;

    /// @brief list of edges
    std::vector<std::string> myEdges;

    /// @brief edge file
    std::string myEdgeFile;

    /// @brief exclude empty
    std::string myExcludeEmpty;

    /// @brief width internal
    bool myWithInternal = false;

    /// @brief detect persons
    std::vector<std::string> myDetectPersons;

    /// @brief minSamples
    double myMinSamples = 0;

    /// @brief max travel time
    double myMaxTravelTime = 0;

    /// @brief VTypes
    std::vector<std::string> myVTypes;

    /// @brief speed threshold
    double mySpeedThreshold = 0;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEMeanData(const GNEMeanData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMeanData& operator=(const GNEMeanData&) = delete;
};

/****************************************************************************/

