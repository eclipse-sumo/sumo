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
/// @file    GNENetHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Helper for GNENet
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <fx.h>
#include <foreign/rtree/SUMORTree.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netedit/changes/GNEChange.h>
#include <utils/common/IDSupplier.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================

class NBNetBuilder;
class GNEAdditional;
class GNEDataSet;
class GNEDemandElement;
class GNEApplicationWindow;
class GNEAttributeCarrier;
class GNEConnection;
class GNECrossing;
class GNEEdge;
class GNEJunction;
class GNELane;
class GNENetworkElement;
class GNEPOI;
class GNEPoly;
class GNEShape;
class GNEUndoList;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

struct GNENetHelper {

    /// @brief struct used for saving all attribute carriers of net, in different formats
    class AttributeCarriers : public ShapeContainer {

    public:
        /// @brief constructor
        AttributeCarriers(GNENet* net);

        /// @brief destructor
        ~AttributeCarriers();

        /// @brief fill tags
        void fillTags();

        /// @brief update ID
        void updateID(GNEAttributeCarrier* AC, const std::string newID);

        /// @brief retrieve generic datas within the given interval
        std::vector<GNEGenericData*> retrieveGenericDatas(const SumoXMLTag genericDataTag, const double begin, const double end);

        /// @brief map with the ID and pointer to junctions of net
        std::map<std::string, GNEJunction*> junctions;

        /// @brief map with the ID and pointer to edges of net
        std::map<std::string, GNEEdge*> edges;

        /// @brief map with the ID and pointer to additional elements of net
        std::map<SumoXMLTag, std::map<std::string, GNEAdditional*> > additionals;

        /// @brief map with the ID and pointer to demand elements of net
        std::map<SumoXMLTag, std::map<std::string, GNEDemandElement*> > demandElements;

        /// @brief map with the ID and pointer to data sets of net
        std::map<std::string, GNEDataSet*> dataSets;

        /// @brief special map used for saving Demand Elements of type "Vehicle" (Vehicles, routeFlows, etc.) sorted by depart time
        std::map<std::string, GNEDemandElement*> vehicleDepartures;

        /// @brief add shape into ShapeContainer
        bool addShape(GNEShape* shape);

        /// @brief delete shape from ShapeContainer
        bool removeShape(GNEShape* shape);

        /// @brief clear all shapes
        void clearShapes();

    protected:
        /// @brief update junction ID in container
        void updateJunctionID(GNEAttributeCarrier* AC, const std::string& newID);

        /// @brief update edge ID in container
        void updateEdgeID(GNEAttributeCarrier* AC, const std::string& newID);

        /// @brief update additional ID in container
        void updateAdditionalID(GNEAttributeCarrier* AC, const std::string& newID);

        /// @brief update shape ID in container
        void updateShapeID(GNEAttributeCarrier* AC, const std::string& newID);

        /// @brief update demand element ID in container
        void updateDemandElementID(GNEAttributeCarrier* AC, const std::string& newID);

        /// @brief update data element ID in container
        void updateDataSetID(GNEAttributeCarrier* AC, const std::string& newID);

        /// @brief pointer to net
        GNENet* myNet;

    private:
        /// @brief Invalidated copy constructor.
        AttributeCarriers(const AttributeCarriers&) = delete;

        /// @brief Invalidated assignment operator.
        AttributeCarriers& operator=(const AttributeCarriers&) = delete;
    };
};