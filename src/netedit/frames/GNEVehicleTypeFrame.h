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

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEVehicleTypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEVehicleTypeFrame();

    /// @brief show Frame
    void show();

    /**@brief add vehicle element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if vehicle was sucesfully added
     */
    bool addVehicle(const GNEViewNet::ObjectsUnderCursor& objectsUnderCursor);

protected:
    /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

    /// @brief build common vehicle attributes
    bool buildVehicleCommonAttributes(std::map<SumoXMLAttr, std::string>& valuesMap, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build vehicle over an edge (parent of lane)
    bool buildVehicleOverEdge(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build vehicle over a single lane
    bool buildVehicleOverLane(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief build vehicle over view
    bool buildVehicleOverView(std::map<SumoXMLAttr, std::string>& valuesMap, const GNEAttributeCarrier::TagProperties& tagValues);

    /// @brief item selector
    ItemSelector* myItemSelector;

    /// @brief internal vehicle attributes
    ACAttributes* myVehicleAttributes;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;
};


#endif

/****************************************************************************/
