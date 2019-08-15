/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDeleteFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2016
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/
#ifndef GNEDeleteFrame_h
#define GNEDeleteFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

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
    struct SubordinatedElements {

        /// @brief constructor (for junctions)
        SubordinatedElements(const GNEJunction* junction);

        /// @brief constructor (for edges)
        SubordinatedElements(const GNEEdge* edge);

        /// @brief constructor (for lanes)
        SubordinatedElements(const GNELane* lane);

        /// @brief constructor (for additionals)
        SubordinatedElements(const GNEAdditional* additional);

        /// @brief constructor (for demandElements)
        SubordinatedElements(const GNEDemandElement* demandElement);

        /// @brief additional parents
        int additionalParents;

        /// @brief additional children
        int additionalChildren;

        /// @brief demand element parents
        int demandElementParents;

        /// @brief demand element children
        int demandElementChildren;

    private:
        /// @brief add operator
        SubordinatedElements& operator+=(const SubordinatedElements& other);
    };

    /// @brief check if there is ACs to delete
    bool ACsToDelete() const;

private:
    /// @brief modul for delete options
    DeleteOptions* myDeleteOptions;

    /// @brief modul for hierarchy
    GNEFrameModuls::AttributeCarrierHierarchy* myAttributeCarrierHierarchy;
};


#endif

/****************************************************************************/

