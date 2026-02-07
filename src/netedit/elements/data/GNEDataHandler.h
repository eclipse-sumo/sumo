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
/// @file    GNEDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/handlers/DataHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDataHandler : public DataHandler {

public:
    /**@brief Constructor
     * @param[in] net GNENet
     * @param[in] bucket FileBucket in which place the element
     * @param[in] allowUndoRedo enable or disable undoRedo
     */
    GNEDataHandler(GNENet* net, FileBucket* fileBucket, const bool allowUndoRedo);

    /// @brief Destructor
    ~GNEDataHandler();

    /// @name build functions
    /// @{
    /**@brief Builds DataSet (exclusive of netedit)
     * @param[in] dataSetID new dataSet
     */
    bool buildDataSet(const std::string& id);

    /**@brief Builds DataInterval
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] dataSetID interval's dataSet
     * @param[in] begin interval begin
     * @param[in] end interval end
     */
    bool buildDataInterval(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& dataSetID,
                           const double begin, const double end);

    /**@brief Builds edgeData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] edgeID edge ID
     * @param[in] parameters parameters map
     */
    bool buildEdgeData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID,
                       const Parameterised::Map& parameters);

    /**@brief Builds edgeRelationData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] fromEdge edge from
     * @param[in] toEdge edge to
     * @param[in] parameters parameters map
     */
    bool buildEdgeRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID,
                               const std::string& toEdgeID, const Parameterised::Map& parameters);

    /**@brief Builds TAZRelationData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] fromTAZ TAZ from
     * @param[in] toTAZ TAZ to
     * @param[in] parameters parameters map
     */
    bool buildTAZRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromTAZID,
                              const std::string& toTAZID, const Parameterised::Map& parameters);
    /// @}

protected:
    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief allow undo/redo
    const bool myAllowUndoRedo;

    /// @brief check if given ID correspond to a duplicated dataSet
    bool checkDuplicatedDataSet(const std::string& id);

private:
    /// @brief invalidate default constructor
    GNEDataHandler() = delete;

    /// @brief invalidate copy constructor
    GNEDataHandler(const GNEDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEDataHandler& operator=(const GNEDataHandler& s) = delete;
};
