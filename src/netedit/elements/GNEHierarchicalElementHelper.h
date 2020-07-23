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
/// @file    GNEHierarchicalElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Helper class for GNEHierarchicalElements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEGeometry.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/geom/Position.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNENetworkElement;
class GNEAdditional;
class GNETAZElement;
class GNEShape;
class GNEDemandElement;
class GNEGenericData;

// ===========================================================================
// class definitions
// ===========================================================================

// @struct GNEHierarchicalElementHelper
struct GNEHierarchicalElementHelper {

	class Container {

    public:
        /// @brief default constructor
        Container();

        /// @brief parameter constructor
        Container(
            const std::vector<GNEJunction*>& parentJunctions,
            const std::vector<GNEEdge*>& parentEdges,
            const std::vector<GNELane*>& parentLanes,
            const std::vector<GNEAdditional*>& parentAdditionals,
            const std::vector<GNEShape*>& parentShapes,
            const std::vector<GNETAZElement*>& parentTAZElements,
            const std::vector<GNEDemandElement*>& ParentDemandElements,
            const std::vector<GNEGenericData*>& parentGenericDatas,
            const std::vector<GNEJunction*>& childJunctions,
            const std::vector<GNEEdge*>& childEdges,
            const std::vector<GNELane*>& childLanes,
            const std::vector<GNEAdditional*>& childAdditionals,
            const std::vector<GNEShape*>& childShapes,
            const std::vector<GNETAZElement*>& childTAZElements,
            const std::vector<GNEDemandElement*>& childDemandElements,
            const std::vector<GNEGenericData*>& childGenericDatas);

        /// @brief add parent element
        template<typename T>
        void addParentElement(const GNEAttributeCarrier* AC, T* element, const int position);

        /// @brief remove parent element
        template<typename T>
        void removeParentElement(const GNEAttributeCarrier* AC, T* element);

        /// @brief add child element
        template<typename T>
        void addChildElement(const GNEAttributeCarrier* AC, T* element, const int position);

        /// @brief remove child element
        template<typename T>
        void removeChildElement(const GNEAttributeCarrier* AC, T* element);

        /// @brief vector of junction parents
        std::vector<GNEJunction*> parentJunctions;

        /// @brief vector of edge parents
        std::vector<GNEEdge*> parentEdges;

        /// @brief vector of lane parents
        std::vector<GNELane*> parentLanes;

        /// @brief vector of additional parents
        std::vector<GNEAdditional*> parentAdditionals;

        /// @brief vector of shape parents
        std::vector<GNEShape*> parentShapes;

        /// @brief vector of TAZElement parents
        std::vector<GNETAZElement*> parentTAZElements;

        /// @brief vector of demand elements parents
        std::vector<GNEDemandElement*> parentDemandElements;

        /// @brief vector of generic datas parents
        std::vector<GNEGenericData*> parentGenericDatas;

        /// @brief vector with the child junctions
        std::vector<GNEJunction*> childJunctions;

        /// @brief vector with the child edges
        std::vector<GNEEdge*> childEdges;

        /// @brief vector with the child lanes
        std::vector<GNELane*> childLanes;

        /// @brief vector with the child additionas
        std::vector<GNEAdditional*> childAdditionals;

        /// @brief vector with the child lanes
        std::vector<GNEShape*> childShapes;

        /// @brief vector with the child TAZ Elements
        std::vector<GNETAZElement*> childTAZElements;

        /// @brief vector with the demand elements children
        std::vector<GNEDemandElement*> childDemandElements;

        /// @brief vector with the generic data elements children
        std::vector<GNEGenericData*> childGenericDatas;

        /// @brief vector with the demand elements children sorted by type and filtered (to avoid duplicated
        std::map<SumoXMLTag, std::vector<GNEDemandElement* >> myDemandElementsByType;
	};

    /// @brief class to pack all variables and functions relative to connections between hierarchical element and their children
    class ChildConnections {

    private:
        /// @brief connection geometry
        class ConnectionGeometry {

        public:
            /// @brief parameter constructor
            ConnectionGeometry(GNELane* lane);

            /// @brief get lane
            const GNELane* getLane() const;

            /// @brief get position
            const Position &getPosition() const;

            /// @brief get rotation
            double getRotation() const;

        private:
            /// @brief lane
            GNELane* myLane;

            /// @brief position
            Position myPosition;

            /// @brief rotation
            double myRotation;

            /// @brief default constructor
            ConnectionGeometry();
        };

    public:
        /// @brief constructor
        ChildConnections(GNEHierarchicalElement* hierarchicalElement);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void drawConnection(const GUIVisualizationSettings& s, const GUIGlObjectType parentType, const double exaggeration) const;

        /// @brief draw dotted connections between Parent and childrens
        void drawDottedConnection(const GUIVisualizationSettings& s, const double exaggeration) const;

        /// @brief position and rotation of every symbol over lane
        std::vector<ConnectionGeometry> symbolsPositionAndRotation;

        /// @brief geometry connections between parents an their children
        std::vector<GNEGeometry::Geometry> connectionsGeometries;

    private:
        /// @brief pointer to hierarchical element parent
        GNEHierarchicalElement* myHierarchicalElement;
    };
};

