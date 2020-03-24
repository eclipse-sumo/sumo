/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEDeleteFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2016
///
// The Widget for remove network-elements
/****************************************************************************/
#pragma once

#include <netedit/frames/GNEFrame.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDeleteFrame
 * The Widget for deleting elements
 */
class GNEDeleteFrame : public GNEFrame {

public:

    // ===========================================================================
    // class DeleteOptions
    // ===========================================================================

    class DeleteOptions : protected FXGroupBox {

    public:
        /// @brief constructor
        DeleteOptions(GNEDeleteFrame* deleteFrameParent);

        /// @brief destructor
        ~DeleteOptions();

        /// @brief check if force delete additionals checkbox is enabled
        bool forceDeleteAdditionals() const;

        /// @brief check if only delete geometry points checkbox is enabled
        bool deleteOnlyGeometryPoints() const;

        /// @brief check if protect demand elements checkbox is enabled
        bool protectDemandElements() const;

    private:
        /// @brief checkbox for enable/disable automatic deletion of additionals children
        FXCheckButton* myForceDeleteAdditionals;

        /// @brief checkbox for enable/disable delete only geometry points
        FXCheckButton* myDeleteOnlyGeometryPoints;

        /// @brief checkbox for enable/disable automatic deletion of demand children
        FXCheckButton* myProtectDemandElements;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEDeleteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEDeleteFrame();

    /// @brief show delete frame
    void show();

    /// @brief hide delete frame
    void hide();

    /// @brief remove selected attribute carriers (element)
    void removeSelectedAttributeCarriers();

    /**@brief remove attribute carrier (element)
     * @param objectsUnderCursor objects under cursors
     * @param ignoreOptions ignore delete options and ALWAYS remove AC
     */
    void removeAttributeCarrier(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool ignoreOptions = false);

    /// @brief get delete options
    DeleteOptions* getDeleteOptions() const;

protected:

    /// @brief struct for saving subordinated elements (Junction->Edge->Lane->(Additional | DemandElement)
    class SubordinatedElements {

    public:
        /// @brief constructor (for junctions)
        SubordinatedElements(const GNEJunction* junction);

        /// @brief constructor (for edges)
        SubordinatedElements(const GNEEdge* edge);

        /// @brief constructor (for lanes)
        SubordinatedElements(const GNELane* lane);

        /// @brief constructor (for additionals)
        SubordinatedElements(const GNEAdditional* additional);

        /// @brief constructor (for shapes)
        SubordinatedElements(const GNEShape* shape);

        /// @brief constructor (for demandElements)
        SubordinatedElements(const GNEDemandElement* demandElement);

        /// @brief constructor (for shapes)
        SubordinatedElements(const GNEGenericData* genericData);

        bool checkElements();

        /// @brief parent additionals
        size_t additionalParents;

        /// @brief child additional
        size_t additionalChilds;

        /// @brief parent shapes
        size_t shapeParents;

        /// @brief child shape
        size_t shapeChilds;

        /// @brief parent demand elements
        size_t demandElementParents;

        /// @brief child demand elements
        size_t demandElementChilds;

        /// @brief parent demand elements
        size_t genericDataParents;

        /// @brief child demand elements
        size_t genericDataChilds;

    private:
        // default constructor for non-net elements
        SubordinatedElements();

        // default constructor for Net Elements
        SubordinatedElements(const GNEHierarchicalParentElements* hierarchicalParent, const GNEHierarchicalChildElements* hierarchicalChild);

        /// @brief add operator
        void add(SubordinatedElements*originalSE, const SubordinatedElements& newSE);

        void add(const GNEHierarchicalParentElements *hierarchicalParent, const GNEHierarchicalChildElements* hierarchicalChild);

        // @brief open warning dialog
        void openWarningDialog(GNEViewNet* viewnet, const std::string& tagParent,
            const std::string& parentID, const std::string& elementType, const size_t number);
    };

    /// @brief check if there is ACs to delete
    bool ACsToDelete() const;

private:
    /// @brief modul for delete options
    DeleteOptions* myDeleteOptions;

    /// @brief modul for hierarchy
    GNEFrameModuls::AttributeCarrierHierarchy* myAttributeCarrierHierarchy;
};
