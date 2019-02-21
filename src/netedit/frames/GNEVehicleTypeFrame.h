/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleTypeFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// The Widget for edit Vehicle Type elements
/****************************************************************************/
#ifndef GNEVehicleTypeFrame_h
#define GNEVehicleTypeFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVehicle;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEVehicleTypeFrame
 */
class GNEVehicleTypeFrame : public GNEFrame {

public:

    // ===========================================================================
    // class vehicleTypeSelector
    // ===========================================================================

    class vehicleTypeSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEVehicleTypeFrame::vehicleTypeSelector)

    public:
        /// @brief constructor
        vehicleTypeSelector(GNEVehicleTypeFrame* vehicleTypeFrameParent);

        /// @brief destructor
        ~vehicleTypeSelector();

        /// @brief get current Vehicle Type
        GNEDemandElement* getCurrentVType() const;

        /// @brief set current Vehicle Type manually
        void setCurrentVType(GNEDemandElement *vType);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        vehicleTypeSelector() {}

    private:
        /// @brief pointer to Frame Parent
        GNEVehicleTypeFrame* myVehicleTypeFrameParent;

        /// @brief comboBox with the list of elements type
        FXComboBox* myTypeMatchBox;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEVehicleTypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEVehicleTypeFrame();

    /// @brief show Frame
    void show();

protected:
    /// @brief enable moduls depending of item selected in vehicleTypeSelector
    void enableModuls(GNEDemandElement *vType);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief item selector
    vehicleTypeSelector* myvehicleTypeSelector;

    /// @brief internal vehicle type attributes
    ACAttributes* myVehicleTypeAttributes;
};


#endif

/****************************************************************************/
