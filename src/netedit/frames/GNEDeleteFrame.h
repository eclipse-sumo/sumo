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
        /// @brief pointer to delete Frame Parent
        GNEDeleteFrame* myDeleteFrameParent;

        /// @brief checkbox for enable/disable automatic deletion of additionals childs
        FXCheckButton* myForceDeleteAdditionals;

        /// @brief checkbox for enable/disable delete only geometry points
        FXCheckButton* myDeleteOnlyGeometryPoints;

        /// @brief checkbox for enable/disable automatic deletion of demand childs
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
     * @param ac Attribute Carrier to remove
     * @param ignoreOptions ignore delete options and ALWAYS remove AC
     */
    void removeAttributeCarrier(GNEAttributeCarrier* ac, bool ignoreOptions = false);

    /// @brief get delete options
    DeleteOptions* getDeleteOptions() const;

private:
    /// @brief modul for delete options
    DeleteOptions* myDeleteOptions;

    /// @brief modul for hierarchy
    ACHierarchy* myACHierarchy;

    /// @brief pointer to marked attributeCarrier (element)
    GNEAttributeCarrier* myMarkedAC;

    /// @brief current attribute carrier selected in three
    GNEAttributeCarrier* myClickedAC;
};


#endif

/****************************************************************************/

