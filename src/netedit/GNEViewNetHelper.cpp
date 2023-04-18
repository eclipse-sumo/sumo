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
/// @file    GNEViewNetHelper.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
///
// A file used to reduce the size of GNEViewNet.h grouping structs and classes
/****************************************************************************/
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/elements/data/GNEEdgeRelData.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEWalkingArea.h>
#include <netedit/elements/network/GNEInternalLane.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/foxtools/MFXMenuCheckIcon.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/options/OptionsCont.h>

#include "GNEViewNetHelper.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEViewParent.h"
#include "GNEApplicationWindow.h"


// ===========================================================================
// static members
// ===========================================================================

std::vector<RGBColor> GNEViewNetHelper::myRainbowScaledColors;

// ---------------------------------------------------------------------------
// GNEViewNetHelper::LockManager - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::LockManager::LockManager(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
    // fill myLockedElements objects
    myLockedElements[GLO_JUNCTION] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_EDGE] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_LANE] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_CONNECTION] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_CROSSING] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_WALKINGAREA] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_ADDITIONALELEMENT] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_WIRE] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_TAZ] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_WIRE] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_POLYGON] = OperationLocked(Supermode::NETWORK);
    myLockedElements[GLO_POI] = OperationLocked(Supermode::NETWORK);
    // fill myLockedElements objects
    myLockedElements[GLO_ROUTE] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_VEHICLE] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_PERSON] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_PERSONTRIP] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_RIDE] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_WALK] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_CONTAINER] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_TRANSPORT] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_TRANSHIP] = OperationLocked(Supermode::DEMAND);
    myLockedElements[GLO_STOP] = OperationLocked(Supermode::DEMAND);
    // fill myLockedElements objects
    myLockedElements[GLO_EDGEDATA] = OperationLocked(Supermode::DATA);
    myLockedElements[GLO_EDGERELDATA] = OperationLocked(Supermode::DATA);
    myLockedElements[GLO_TAZRELDATA] = OperationLocked(Supermode::DATA);
}


GNEViewNetHelper::LockManager::~LockManager() {}


bool
GNEViewNetHelper::LockManager::isObjectLocked(GUIGlObjectType objectType, const bool selected) const {
    if (selected && (myViewNet->getViewParent()->getGNEAppWindows()->getLockMenuCommands().menuCheckLockSelectedElements->getCheck() == TRUE)) {
        return true;
    } else if ((objectType >= GLO_ADDITIONALELEMENT) && (objectType <= GLO_ACCESS)) {
        // additionals
        return myLockedElements.at(GLO_ADDITIONALELEMENT).lock;
    } else if ((objectType >= GLO_WIRE) && (objectType <= GLO_TRACTIONSUBSTATION)) {
        // wires
        return myLockedElements.at(GLO_WIRE).lock;
    } else if ((objectType >= GLO_VEHICLE) && (objectType <= GLO_ROUTEFLOW)) {
        // vehicles
        return myLockedElements.at(GLO_VEHICLE).lock;
    } else if ((objectType == GLO_PERSON) || (objectType == GLO_PERSONFLOW)) {
        // persons
        return myLockedElements.at(GLO_PERSON).lock;
    } else if ((objectType == GLO_CONTAINER) || (objectType == GLO_CONTAINERFLOW)) {
        // containers
        return myLockedElements.at(GLO_PERSON).lock;
    } else if ((objectType >= GLO_STOP) && (objectType <= GLO_STOP_CONTAINER)) {
        // stops
        return myLockedElements.at(GLO_ADDITIONALELEMENT).lock;
    } else {
        return myLockedElements.at(objectType).lock;
    }
}


void
GNEViewNetHelper::LockManager::updateFlags() {
    // get lock menu commands
    GNEApplicationWindowHelper::LockMenuCommands& lockMenuCommands = myViewNet->getViewParent()->getGNEAppWindows()->getLockMenuCommands();
    // network
    myLockedElements[GLO_JUNCTION].lock = lockMenuCommands.menuCheckLockJunction->getCheck() == TRUE;
    myLockedElements[GLO_EDGE].lock = lockMenuCommands.menuCheckLockEdges->getCheck() == TRUE;
    myLockedElements[GLO_LANE].lock = lockMenuCommands.menuCheckLockLanes->getCheck() == TRUE;
    myLockedElements[GLO_CONNECTION].lock = lockMenuCommands.menuCheckLockConnections->getCheck() == TRUE;
    myLockedElements[GLO_CROSSING].lock = lockMenuCommands.menuCheckLockCrossings->getCheck() == TRUE;
    myLockedElements[GLO_WALKINGAREA].lock = lockMenuCommands.menuCheckLockWalkingAreas->getCheck() == TRUE;
    myLockedElements[GLO_ADDITIONALELEMENT].lock = lockMenuCommands.menuCheckLockAdditionals->getCheck() == TRUE;
    myLockedElements[GLO_WIRE].lock = lockMenuCommands.menuCheckLockWires->getCheck() == TRUE;
    myLockedElements[GLO_TAZ].lock = lockMenuCommands.menuCheckLockTAZs->getCheck() == TRUE;
    myLockedElements[GLO_POLYGON].lock = lockMenuCommands.menuCheckLockPolygons->getCheck() == TRUE;
    myLockedElements[GLO_POI].lock = lockMenuCommands.menuCheckLockPOIs->getCheck() == TRUE;
    // demand
    myLockedElements[GLO_ROUTE].lock = lockMenuCommands.menuCheckLockRoutes->getCheck() == TRUE;
    myLockedElements[GLO_VEHICLE].lock = lockMenuCommands.menuCheckLockVehicles->getCheck() == TRUE;
    myLockedElements[GLO_PERSON].lock = lockMenuCommands.menuCheckLockPersons->getCheck() == TRUE;
    myLockedElements[GLO_PERSONTRIP].lock = lockMenuCommands.menuCheckLockPersonTrip->getCheck() == TRUE;
    myLockedElements[GLO_WALK].lock = lockMenuCommands.menuCheckLockWalk->getCheck() == TRUE;
    myLockedElements[GLO_RIDE].lock = lockMenuCommands.menuCheckLockRides->getCheck() == TRUE;
    myLockedElements[GLO_CONTAINER].lock = lockMenuCommands.menuCheckLockContainers->getCheck() == TRUE;
    myLockedElements[GLO_TRANSPORT].lock = lockMenuCommands.menuCheckLockTransports->getCheck() == TRUE;
    myLockedElements[GLO_TRANSHIP].lock = lockMenuCommands.menuCheckLockTranships->getCheck() == TRUE;
    myLockedElements[GLO_STOP].lock = lockMenuCommands.menuCheckLockStops->getCheck() == TRUE;
    // data
    myLockedElements[GLO_EDGEDATA].lock = lockMenuCommands.menuCheckLockEdgeDatas->getCheck() == TRUE;
    myLockedElements[GLO_EDGERELDATA].lock = lockMenuCommands.menuCheckLockEdgeRelDatas->getCheck() == TRUE;
    myLockedElements[GLO_TAZRELDATA].lock = lockMenuCommands.menuCheckLockEdgeTAZRels->getCheck() == TRUE;
}


void
GNEViewNetHelper::LockManager::updateLockMenuBar() {
    // get lock menu commands
    GNEApplicationWindowHelper::LockMenuCommands& lockMenuCommands = myViewNet->getViewParent()->getGNEAppWindows()->getLockMenuCommands();
    // network
    lockMenuCommands.menuCheckLockJunction->setCheck(myLockedElements[GLO_JUNCTION].lock);
    lockMenuCommands.menuCheckLockEdges->setCheck(myLockedElements[GLO_EDGE].lock);
    lockMenuCommands.menuCheckLockLanes->setCheck(myLockedElements[GLO_LANE].lock);
    lockMenuCommands.menuCheckLockConnections->setCheck(myLockedElements[GLO_CONNECTION].lock);
    lockMenuCommands.menuCheckLockCrossings->setCheck(myLockedElements[GLO_CROSSING].lock);
    lockMenuCommands.menuCheckLockWalkingAreas->setCheck(myLockedElements[GLO_WALKINGAREA].lock);
    lockMenuCommands.menuCheckLockAdditionals->setCheck(myLockedElements[GLO_ADDITIONALELEMENT].lock);
    lockMenuCommands.menuCheckLockWires->setCheck(myLockedElements[GLO_WIRE].lock);
    lockMenuCommands.menuCheckLockTAZs->setCheck(myLockedElements[GLO_TAZ].lock);
    lockMenuCommands.menuCheckLockPolygons->setCheck(myLockedElements[GLO_POLYGON].lock);
    lockMenuCommands.menuCheckLockPOIs->setCheck(myLockedElements[GLO_POI].lock);
    // demand
    lockMenuCommands.menuCheckLockRoutes->setCheck(myLockedElements[GLO_ROUTE].lock);
    lockMenuCommands.menuCheckLockVehicles->setCheck(myLockedElements[GLO_VEHICLE].lock);
    lockMenuCommands.menuCheckLockPersons->setCheck(myLockedElements[GLO_PERSON].lock);
    lockMenuCommands.menuCheckLockPersonTrip->setCheck(myLockedElements[GLO_PERSONTRIP].lock);
    lockMenuCommands.menuCheckLockWalk->setCheck(myLockedElements[GLO_WALK].lock);
    lockMenuCommands.menuCheckLockRides->setCheck(myLockedElements[GLO_RIDE].lock);
    lockMenuCommands.menuCheckLockContainers->setCheck(myLockedElements[GLO_CONTAINER].lock);
    lockMenuCommands.menuCheckLockTransports->setCheck(myLockedElements[GLO_TRANSPORT].lock);
    lockMenuCommands.menuCheckLockTranships->setCheck(myLockedElements[GLO_TRANSHIP].lock);
    lockMenuCommands.menuCheckLockStops->setCheck(myLockedElements[GLO_STOP].lock);
    // data
    lockMenuCommands.menuCheckLockEdgeDatas->setCheck(myLockedElements[GLO_EDGEDATA].lock);
    lockMenuCommands.menuCheckLockEdgeRelDatas->setCheck(myLockedElements[GLO_EDGERELDATA].lock);
    lockMenuCommands.menuCheckLockEdgeTAZRels->setCheck(myLockedElements[GLO_TAZRELDATA].lock);
}


GNEViewNetHelper::LockManager::OperationLocked::OperationLocked():
    mySupermode(Supermode::NETWORK) {
}


GNEViewNetHelper::LockManager::OperationLocked::OperationLocked(Supermode supermode) :
    mySupermode(supermode) {
}


GNEViewNetHelper::LockManager::OperationLocked::~OperationLocked() {}


Supermode
GNEViewNetHelper::LockManager::OperationLocked::getSupermode() const {
    return mySupermode;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::ObjectsUnderCursor - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::ObjectsUnderCursor::ObjectsUnderCursor(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    mySwapLane2edge(false) {
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateObjectUnderCursor(const std::vector<GUIGlObject*>& GUIGlObjects) {
    // reset flag
    mySwapLane2edge = false;
    // clear elements
    myEdgeObjects.clearElements();
    myLaneObjects.clearElements();
    // filter duplicated objects
    const auto filteredObjects = filterDuplicatedObjects(GUIGlObjects);
    // sort GUIGLObjects
    sortGUIGlObjects(filteredObjects);
    // process GUIGLObjects using myEdgeObjects.GUIGlObjects and myLaneObjects.GUIGlObjects
    processGUIGlObjects();
}


void
GNEViewNetHelper::ObjectsUnderCursor::swapLane2Edge() {
    // enable flag
    mySwapLane2edge = true;
}


void
GNEViewNetHelper::ObjectsUnderCursor::filterLockedElements(const GNEViewNetHelper::LockManager& lockManager, std::vector<GUIGlObjectType> forcedIgnoredTiped) {
    // make a copy of edge and lane Attribute carriers
    auto edgeACs = myEdgeObjects.attributeCarriers;
    auto laneACs = myLaneObjects.attributeCarriers;
    // clear elements
    myEdgeObjects.clearElements();
    myLaneObjects.clearElements();
    // filter GUIGLObjects
    for (const auto& edgeAC : edgeACs) {
        if (std::find(forcedIgnoredTiped.begin(), forcedIgnoredTiped.end(), edgeAC->getGUIGlObject()->getType()) != forcedIgnoredTiped.end()) {
            continue;
        } else if (!lockManager.isObjectLocked(edgeAC->getGUIGlObject()->getType(), edgeAC->isAttributeCarrierSelected())) {
            myEdgeObjects.GUIGlObjects.push_back(edgeAC->getGUIGlObject());
        }
    }
    for (const auto& laneAC : laneACs) {
        if (std::find(forcedIgnoredTiped.begin(), forcedIgnoredTiped.end(), laneAC->getGUIGlObject()->getType()) != forcedIgnoredTiped.end()) {
            continue;
        } else if (!lockManager.isObjectLocked(laneAC->getGUIGlObject()->getType(), laneAC->isAttributeCarrierSelected())) {
            myLaneObjects.GUIGlObjects.push_back(laneAC->getGUIGlObject());
        }
    }
    // process GUIGLObjects using myEdgeObjects.GUIGlObjects and myLaneObjects.GUIGlObjects
    processGUIGlObjects();
}


void
GNEViewNetHelper::ObjectsUnderCursor::shortDataElements() {
    // declare set for sort generic datas by begin->end->AC
    std::set<std::pair<double, std::pair<double, GNEAttributeCarrier*> > > sortedDataElements;
    // short only lane objects (because shortData elements is used only in data mode)
    for (const auto& AC : myLaneObjects.attributeCarriers) {
        if (AC->getTagProperty().isGenericData()) {
            const double begin = GNEAttributeCarrier::parse<double>(AC->getAttribute(SUMO_ATTR_BEGIN));
            const double end = GNEAttributeCarrier::parse<double>(AC->getAttribute(SUMO_ATTR_END));
            sortedDataElements.insert(std::make_pair(begin, std::make_pair(end, AC)));
        }
    }
    myLaneObjects.attributeCarriers.clear();
    for (const auto& sortedDataElement : sortedDataElements) {
        myLaneObjects.attributeCarriers.push_back(sortedDataElement.second.second);
    }
}


GUIGlID
GNEViewNetHelper::ObjectsUnderCursor::getGlIDFront() const {
    if (getGUIGlObjectFront()) {
        return getGUIGlObjectFront()->getGlID();
    } else {
        return 0;
    }
}


GUIGlObjectType
GNEViewNetHelper::ObjectsUnderCursor::getGlTypeFront() const {
    if (getGUIGlObjectFront()) {
        return getGUIGlObjectFront()->getType();
    } else {
        return GLO_NETWORK;
    }
}


GUIGlObject*
GNEViewNetHelper::ObjectsUnderCursor::getGUIGlObjectFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.attributeCarriers.size() > 0) {
            return myEdgeObjects.GUIGlObjects.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.attributeCarriers.size() > 0) {
            return myLaneObjects.GUIGlObjects.front();
        } else {
            return nullptr;
        }
    }
}


GNEAttributeCarrier*
GNEViewNetHelper::ObjectsUnderCursor::getAttributeCarrierFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.attributeCarriers.size() > 0) {
            return myEdgeObjects.attributeCarriers.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.attributeCarriers.size() > 0) {
            return myLaneObjects.attributeCarriers.front();
        } else {
            return nullptr;
        }
    }
}


GNENetworkElement*
GNEViewNetHelper::ObjectsUnderCursor::getNetworkElementFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.networkElements.size() > 0) {
            return myEdgeObjects.networkElements.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.networkElements.size() > 0) {
            return myLaneObjects.networkElements.front();
        } else {
            return nullptr;
        }
    }
}


GNEAdditional*
GNEViewNetHelper::ObjectsUnderCursor::getAdditionalFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.additionals.size() > 0) {
            return myEdgeObjects.additionals.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.additionals.size() > 0) {
            return myLaneObjects.additionals.front();
        } else {
            return nullptr;
        }
    }
}


GNEDemandElement*
GNEViewNetHelper::ObjectsUnderCursor::getDemandElementFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.demandElements.size() > 0) {
            return myEdgeObjects.demandElements.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.demandElements.size() > 0) {
            return myLaneObjects.demandElements.front();
        } else {
            return nullptr;
        }
    }
}


GNEGenericData*
GNEViewNetHelper::ObjectsUnderCursor::getGenericDataElementFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.genericDatas.size() > 0) {
            return myEdgeObjects.genericDatas.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.genericDatas.size() > 0) {
            return myLaneObjects.genericDatas.front();
        } else {
            return nullptr;
        }
    }
}


GNEJunction*
GNEViewNetHelper::ObjectsUnderCursor::getJunctionFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.junctions.size() > 0) {
            return myEdgeObjects.junctions.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.junctions.size() > 0) {
            return myLaneObjects.junctions.front();
        } else {
            return nullptr;
        }
    }
}


GNEEdge*
GNEViewNetHelper::ObjectsUnderCursor::getEdgeFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.edges.size() > 0) {
            return myEdgeObjects.edges.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.edges.size() > 0) {
            return myLaneObjects.edges.front();
        } else {
            return nullptr;
        }
    }
}


GNELane*
GNEViewNetHelper::ObjectsUnderCursor::getLaneFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.lanes.size() > 0) {
            return myEdgeObjects.lanes.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.lanes.size() > 0) {
            return myLaneObjects.lanes.front();
        } else {
            return nullptr;
        }
    }
}


GNELane*
GNEViewNetHelper::ObjectsUnderCursor::getLaneFrontNonLocked() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.lanes.size() > 0) {
            for (auto& lane : myEdgeObjects.lanes) {
                if (!(lane->isAttributeCarrierSelected() || lane->getParentEdge()->isAttributeCarrierSelected()) ||
                        !myViewNet->getViewParent()->getGNEAppWindows()->getLockMenuCommands().menuCheckLockSelectedElements->getCheck()) {
                    return lane;
                }
            }
            // all locked, then return nullptr
            return nullptr;
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.lanes.size() > 0) {
            for (auto& lane : myLaneObjects.lanes) {
                if (!(lane->isAttributeCarrierSelected() || lane->getParentEdge()->isAttributeCarrierSelected()) ||
                        !myViewNet->getViewParent()->getGNEAppWindows()->getLockMenuCommands().menuCheckLockSelectedElements->getCheck()) {
                    return lane;
                }
            }
            // all locked, then return nullptr
            return nullptr;
        } else {
            return nullptr;
        }
    }
}


const std::vector<GNELane*>&
GNEViewNetHelper::ObjectsUnderCursor::getLanes() const {
    if (mySwapLane2edge) {
        return myEdgeObjects.lanes;
    } else {
        return myLaneObjects.lanes;
    }
}


GNECrossing*
GNEViewNetHelper::ObjectsUnderCursor::getCrossingFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.crossings.size() > 0) {
            return myEdgeObjects.crossings.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.crossings.size() > 0) {
            return myLaneObjects.crossings.front();
        } else {
            return nullptr;
        }
    }
}


GNEWalkingArea*
GNEViewNetHelper::ObjectsUnderCursor::getWalkingAreaFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.walkingAreas.size() > 0) {
            return myEdgeObjects.walkingAreas.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.walkingAreas.size() > 0) {
            return myLaneObjects.walkingAreas.front();
        } else {
            return nullptr;
        }
    }
}


GNEConnection*
GNEViewNetHelper::ObjectsUnderCursor::getConnectionFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.connections.size() > 0) {
            return myEdgeObjects.connections.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.connections.size() > 0) {
            return myLaneObjects.connections.front();
        } else {
            return nullptr;
        }
    }
}


GNEInternalLane*
GNEViewNetHelper::ObjectsUnderCursor::getInternalLaneFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.internalLanes.size() > 0) {
            return myEdgeObjects.internalLanes.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.internalLanes.size() > 0) {
            return myLaneObjects.internalLanes.front();
        } else {
            return nullptr;
        }
    }
}


GNEPOI*
GNEViewNetHelper::ObjectsUnderCursor::getPOIFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.POIs.size() > 0) {
            return myEdgeObjects.POIs.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.POIs.size() > 0) {
            return myLaneObjects.POIs.front();
        } else {
            return nullptr;
        }
    }
}


GNEPoly*
GNEViewNetHelper::ObjectsUnderCursor::getPolyFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.polys.size() > 0) {
            return myEdgeObjects.polys.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.polys.size() > 0) {
            return myLaneObjects.polys.front();
        } else {
            return nullptr;
        }
    }
}


GNETAZ*
GNEViewNetHelper::ObjectsUnderCursor::getTAZFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.TAZs.size() > 0) {
            return myEdgeObjects.TAZs.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.TAZs.size() > 0) {
            return myLaneObjects.TAZs.front();
        } else {
            return nullptr;
        }
    }
}


GNEEdgeData*
GNEViewNetHelper::ObjectsUnderCursor::getEdgeDataElementFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.edgeDatas.size() > 0) {
            return myEdgeObjects.edgeDatas.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.edgeDatas.size() > 0) {
            return myLaneObjects.edgeDatas.front();
        } else {
            return nullptr;
        }
    }
}


GNEEdgeRelData*
GNEViewNetHelper::ObjectsUnderCursor::getEdgeRelDataElementFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.edgeRelDatas.size() > 0) {
            return myEdgeObjects.edgeRelDatas.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.edgeRelDatas.size() > 0) {
            return myLaneObjects.edgeRelDatas.front();
        } else {
            return nullptr;
        }
    }
}

const std::vector<GUIGlObject*>&
GNEViewNetHelper::ObjectsUnderCursor::getClickedGLObjects() const {
    if (mySwapLane2edge) {
        return myEdgeObjects.GUIGlObjects;
    } else {
        return myLaneObjects.GUIGlObjects;
    }
}


const std::vector<GNEAttributeCarrier*>&
GNEViewNetHelper::ObjectsUnderCursor::getClickedAttributeCarriers() const {
    if (mySwapLane2edge) {
        return myEdgeObjects.attributeCarriers;
    } else {
        return myLaneObjects.attributeCarriers;
    }
}


const std::vector<GNEJunction*>&
GNEViewNetHelper::ObjectsUnderCursor::getClickedJunctions() const {
    if (mySwapLane2edge) {
        return myEdgeObjects.junctions;
    } else {
        return myLaneObjects.junctions;
    }
}


const std::vector<GNEDemandElement*>&
GNEViewNetHelper::ObjectsUnderCursor::getClickedDemandElements() const {
    if (mySwapLane2edge) {
        return myEdgeObjects.demandElements;
    } else {
        return myLaneObjects.demandElements;
    }
}


GNEViewNetHelper::ObjectsUnderCursor::ObjectsContainer::ObjectsContainer() {}


void
GNEViewNetHelper::ObjectsUnderCursor::ObjectsContainer::clearElements() {
    // just clear all containers
    GUIGlObjects.clear();
    attributeCarriers.clear();
    networkElements.clear();
    additionals.clear();
    demandElements.clear();
    junctions.clear();
    edges.clear();
    lanes.clear();
    crossings.clear();
    walkingAreas.clear();
    connections.clear();
    internalLanes.clear();
    TAZs.clear();
    POIs.clear();
    polys.clear();
    genericDatas.clear();
    edgeDatas.clear();
    edgeRelDatas.clear();
}


std::vector<GUIGlObject*>
GNEViewNetHelper::ObjectsUnderCursor::filterDuplicatedObjects(const std::vector<GUIGlObject*>& GUIGlObjects) const {
    // declare vector for filter objects
    std::vector<GUIGlObject*> filteredGUIGlObjects;
    // iterate over GUIGlObjects
    for (const auto& GLObject : GUIGlObjects) {
        // find GLObject in filteredGUIGlObjects
        const auto it = std::find(filteredGUIGlObjects.begin(), filteredGUIGlObjects.end(), GLObject);
        if (it == filteredGUIGlObjects.end()) {
            filteredGUIGlObjects.push_back(GLObject);
        }
    }
    return filteredGUIGlObjects;
}


void
GNEViewNetHelper::ObjectsUnderCursor::sortGUIGlObjects(const std::vector<GUIGlObject*>& GUIGlObjects) {
    // declare a map to save GUIGlObjects sorted by GLO_TYPE
    std::map<double, std::vector<GUIGlObject*> > mySortedGUIGlObjects;
    // iterate over set
    for (const auto& GLObject : GUIGlObjects) {
        // try to parse shape
        const Shape* shape = dynamic_cast<Shape*>(GLObject);
        if (shape) {
            mySortedGUIGlObjects[shape->getShapeLayer()].push_back(GLObject);
        } else {
            mySortedGUIGlObjects[GLObject->getType()].push_back(GLObject);
        }
    }
    // move sorted GUIGlObjects into myGUIGlObjectLanes using a reverse iterator
    for (std::map<double, std::vector<GUIGlObject*> >::reverse_iterator i = mySortedGUIGlObjects.rbegin(); i != mySortedGUIGlObjects.rend(); i++) {
        for (const auto& GlObject : i->second) {
            // avoid GLO_NETWORKELEMENT
            if (GlObject->getType() != GLO_NETWORKELEMENT) {
                // add it in GUIGlObject splitting by edge/lanes
                if (GlObject->getType() == GLO_LANE) {
                    myLaneObjects.GUIGlObjects.push_back(GlObject);
                } else {
                    myEdgeObjects.GUIGlObjects.push_back(GlObject);
                    myLaneObjects.GUIGlObjects.push_back(GlObject);
                }
            }
        }
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateAttributeCarriers(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // special case for edges and lanes
    if (frontAC && (frontAC->getTagProperty().getTag() == SUMO_TAG_EDGE) && (AC->getTagProperty().getTag() == SUMO_TAG_LANE)) {
        // compare IDs
        if (AC->getAttribute(GNE_ATTR_PARENT) == frontAC->getID()) {
            // insert at front
            container.attributeCarriers.insert(container.attributeCarriers.begin(), AC);
        } else {
            // insert at back
            container.attributeCarriers.push_back(AC);
        }
    } else {
        // add it in attributeCarriers
        if (AC == frontAC) {
            // insert at front
            container.attributeCarriers.insert(container.attributeCarriers.begin(), AC);
        } else {
            // insert at back
            container.attributeCarriers.push_back(AC);
        }
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateNetworkElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // check front element
    if (AC == frontAC) {
        // insert at front
        container.networkElements.insert(container.networkElements.begin(), dynamic_cast<GNENetworkElement*>(AC));
    } else {
        // insert at back
        container.networkElements.push_back(dynamic_cast<GNENetworkElement*>(AC));
    }
    // cast specific network element
    switch (AC->getGUIGlObject()->getType()) {
        case GLO_JUNCTION: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.junctions.insert(container.junctions.begin(), dynamic_cast<GNEJunction*>(AC));
            } else {
                // insert at back
                container.junctions.push_back(dynamic_cast<GNEJunction*>(AC));
            }
            break;
        }
        case GLO_EDGE: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.edges.insert(container.edges.begin(), dynamic_cast<GNEEdge*>(AC));
            } else {
                // insert at back
                container.edges.push_back(dynamic_cast<GNEEdge*>(AC));
            }
            break;
        }
        case GLO_LANE: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.lanes.insert(container.lanes.begin(), dynamic_cast<GNELane*>(AC));
            } else {
                // insert at back
                container.lanes.push_back(dynamic_cast<GNELane*>(AC));
            }
            break;
        }
        case GLO_CROSSING: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.crossings.insert(container.crossings.begin(), dynamic_cast<GNECrossing*>(AC));
            } else {
                // insert at back
                container.crossings.push_back(dynamic_cast<GNECrossing*>(AC));
            }
            break;
        }
        case GLO_WALKINGAREA: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.walkingAreas.insert(container.walkingAreas.begin(), dynamic_cast<GNEWalkingArea*>(AC));
            } else {
                // insert at back
                container.walkingAreas.push_back(dynamic_cast<GNEWalkingArea*>(AC));
            }
            break;
        }
        case GLO_CONNECTION: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.connections.insert(container.connections.begin(), dynamic_cast<GNEConnection*>(AC));
            } else {
                // insert at back
                container.connections.push_back(dynamic_cast<GNEConnection*>(AC));
            }
            break;
        }
        case GLO_TLLOGIC: {
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.internalLanes.insert(container.internalLanes.begin(), dynamic_cast<GNEInternalLane*>(AC));
            } else {
                // insert at back
                container.internalLanes.push_back(dynamic_cast<GNEInternalLane*>(AC));
            }
            break;
        }
        default:
            break;
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateAdditionalElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get additional element
    GNEAdditional* additionalElement = myViewNet->getNet()->getAttributeCarriers()->retrieveAdditional(AC);
    // insert depending if is the front attribute carrier
    if (additionalElement == myViewNet->getFrontAttributeCarrier()) {
        // insert at front
        container.additionals.insert(container.additionals.begin(), additionalElement);
    } else {
        // insert at back
        container.additionals.push_back(additionalElement);
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateShapeElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // cast specific shape
    if (AC->getGUIGlObject()->getType() == GLO_POI) {
        // cast POI
        GNEPOI* POI = dynamic_cast<GNEPOI*>(AC);
        if (POI) {
            // check front element
            if (AC == myViewNet->getFrontAttributeCarrier()) {
                // insert at front
                container.POIs.insert(container.POIs.begin(), POI);
            } else {
                // insert at back
                container.POIs.push_back(POI);
            }
        }
    } else if (AC->getGUIGlObject()->getType() == GLO_POLYGON) {
        // cast poly
        GNEPoly* poly = dynamic_cast<GNEPoly*>(AC);
        if (poly) {
            // check front element
            if (AC == myViewNet->getFrontAttributeCarrier()) {
                // insert at front
                container.polys.insert(container.polys.begin(), poly);
            } else {
                // insert at back
                container.polys.push_back(poly);
            }
        }
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateTAZElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // cast specific TAZ
    if (AC->getGUIGlObject()->getType() == GLO_TAZ) {
        // cast TAZ
        GNETAZ* TAZ = dynamic_cast<GNETAZ*>(AC);
        if (TAZ) {
            // check front element
            if (AC == myViewNet->getFrontAttributeCarrier()) {
                // insert at front
                container.TAZs.insert(container.TAZs.begin(), TAZ);
            } else {
                // insert at back
                container.TAZs.push_back(TAZ);
            }
        }
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateDemandElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get demandElement
    GNEDemandElement* demandElement = myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(AC);
    // insert depending if is the front attribute carrier
    if (demandElement == myViewNet->getFrontAttributeCarrier()) {
        // insert at front
        container.demandElements.insert(container.demandElements.begin(), demandElement);
    } else {
        // insert at back
        container.demandElements.push_back(demandElement);
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateGenericDataElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // cast generic data from attribute carrier
    if (AC == frontAC) {
        // insert at front
        container.genericDatas.insert(container.genericDatas.begin(), dynamic_cast<GNEGenericData*>(AC));
    } else {
        // insert at back
        container.genericDatas.push_back(dynamic_cast<GNEGenericData*>(AC));
    }
    // cast specific generic data
    switch (AC->getGUIGlObject()->getType()) {
        case GLO_EDGEDATA:
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.edgeDatas.insert(container.edgeDatas.begin(), dynamic_cast<GNEEdgeData*>(AC));
            } else {
                // insert at back
                container.edgeDatas.push_back(dynamic_cast<GNEEdgeData*>(AC));
            }
            break;
        case GLO_EDGERELDATA:
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.edgeRelDatas.insert(container.edgeRelDatas.begin(), dynamic_cast<GNEEdgeRelData*>(AC));
            } else {
                // insert at back
                container.edgeRelDatas.push_back(dynamic_cast<GNEEdgeRelData*>(AC));
            }
            break;
        default:
            break;
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateGUIGlObjects(ObjectsContainer& container) {
    // first clear GUIGlObjects
    container.GUIGlObjects.clear();
    // reserve
    container.GUIGlObjects.reserve(container.attributeCarriers.size());
    // iterate over attribute carriers
    for (const auto& attributeCarrrier : container.attributeCarriers) {
        // add GUIGlObject in GUIGlObjects container
        container.GUIGlObjects.push_back(attributeCarrrier->getGUIGlObject());
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::processGUIGlObjects() {
    // iterate over myGUIGlObjectLanes
    for (const auto& glObject : myEdgeObjects.GUIGlObjects) {
        // cast attribute carrier from glObject
        GNEAttributeCarrier* AC = dynamic_cast<GNEAttributeCarrier*>(glObject);
        // only continue if attributeCarrier isn't nullptr;
        if (AC) {
            // update attribute carrier
            updateAttributeCarriers(myEdgeObjects, AC);
            // cast specific network elemetns
            if (AC->getTagProperty().isNetworkElement()) {
                // update network elements
                updateNetworkElements(myEdgeObjects, AC);
            } else if (AC->getTagProperty().isAdditionalElement()) {
                // update additional elements
                updateAdditionalElements(myEdgeObjects, AC);
                // update shapes and TAZs
                if (AC->getTagProperty().isShapeElement()) {
                    // update shape elements
                    updateShapeElements(myEdgeObjects, AC);
                }
                if (AC->getTagProperty().isTAZElement()) {
                    // update TAZ elements
                    updateTAZElements(myEdgeObjects, AC);
                }
            } else if (AC->getTagProperty().isDemandElement()) {
                // update demand elements
                updateDemandElements(myEdgeObjects, AC);
            } else if (AC->getTagProperty().isGenericData()) {
                // update generic datas
                updateGenericDataElements(myEdgeObjects, AC);
            }
        }
    }
    // update GUIGlObjects (due front element)
    updateGUIGlObjects(myEdgeObjects);
    // iterate over myGUIGlObjectLanes
    for (const auto& glObject : myLaneObjects.GUIGlObjects) {
        // cast attribute carrier from glObject
        GNEAttributeCarrier* AC = dynamic_cast<GNEAttributeCarrier*>(glObject);
        // only continue if attributeCarrier isn't nullptr;
        if (AC) {
            // update attribute carrier
            updateAttributeCarriers(myLaneObjects, AC);
            // cast specific network elemetns
            if (AC->getTagProperty().isNetworkElement()) {
                // update network elements
                updateNetworkElements(myLaneObjects, AC);
            } else if (AC->getTagProperty().isAdditionalElement()) {
                // update additional elements
                updateAdditionalElements(myLaneObjects, AC);
                // update shapes and TAZs
                if (AC->getTagProperty().isShapeElement()) {
                    // update shape elements
                    updateShapeElements(myLaneObjects, AC);
                }
                if (AC->getTagProperty().isTAZElement()) {
                    // update TAZ elements
                    updateTAZElements(myLaneObjects, AC);
                }
            } else if (AC->getTagProperty().isDemandElement()) {
                // update demand elements
                updateDemandElements(myLaneObjects, AC);
            } else if (AC->getTagProperty().isGenericData()) {
                // update generic datas
                updateGenericDataElements(myLaneObjects, AC);
            }
        }
    }
    // update GUIGlObjects (due front element)
    updateGUIGlObjects(myLaneObjects);
}


GNEViewNetHelper::ObjectsUnderCursor::ObjectsUnderCursor() :
    myViewNet(nullptr),
    mySwapLane2edge(false) {
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::MouseButtonKeyPressed - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::MouseButtonKeyPressed::MouseButtonKeyPressed() :
    myEventInfo(nullptr) {
}


void
GNEViewNetHelper::MouseButtonKeyPressed::update(void* eventData) {
    myEventInfo = (FXEvent*) eventData;
}


bool
GNEViewNetHelper::MouseButtonKeyPressed::shiftKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & SHIFTMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::MouseButtonKeyPressed::controlKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & CONTROLMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::MouseButtonKeyPressed::altKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & ALTMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::MouseButtonKeyPressed::mouseLeftButtonPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & LEFTBUTTONMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::MouseButtonKeyPressed::mouseRightButtonPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & RIGHTBUTTONMASK) != 0;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::MoveSingleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::MoveSingleElementValues::MoveSingleElementValues(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


bool
GNEViewNetHelper::MoveSingleElementValues::beginMoveNetworkElementShape() {
    // first obtain moving reference (common for all)
    myRelativeClickedPosition = myViewNet->getPositionInformation();
    // get edited element
    const GNENetworkElement* editedElement = myViewNet->myEditNetworkElementShapes.getEditedNetworkElement();
    // check what type of AC will be moved
    if (myViewNet->myObjectsUnderCursor.getJunctionFront() && (myViewNet->myObjectsUnderCursor.getJunctionFront() == editedElement)) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getJunctionFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getLaneFront() && (myViewNet->myObjectsUnderCursor.getLaneFront() == editedElement)) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getLaneFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getCrossingFront() && (myViewNet->myObjectsUnderCursor.getCrossingFront() == editedElement)) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getCrossingFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getConnectionFront() && (myViewNet->myObjectsUnderCursor.getConnectionFront() == editedElement)) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getConnectionFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getWalkingAreaFront() && (myViewNet->myObjectsUnderCursor.getWalkingAreaFront() == editedElement)) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getWalkingAreaFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else {
        // there isn't moved items, then return false
        return false;
    }
}


bool
GNEViewNetHelper::MoveSingleElementValues::beginMoveSingleElementNetworkMode() {
    // first obtain moving reference (common for all)
    myRelativeClickedPosition = myViewNet->getPositionInformation();
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->myObjectsUnderCursor.getAttributeCarrierFront();
    // check what type of AC will be moved
    if (myViewNet->myObjectsUnderCursor.getPolyFront() && (frontAC == myViewNet->myObjectsUnderCursor.getPolyFront())) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getPolyFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getPOIFront() && (frontAC == myViewNet->myObjectsUnderCursor.getPOIFront())) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getPOIFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getAdditionalFront() && (frontAC == myViewNet->myObjectsUnderCursor.getAdditionalFront())) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getAdditionalFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getJunctionFront() && (frontAC == myViewNet->myObjectsUnderCursor.getJunctionFront())) {
        // check if over junction there is a geometry point
        if (myViewNet->myObjectsUnderCursor.getEdgeFront() && (myViewNet->myObjectsUnderCursor.getEdgeFront()->clickedOverGeometryPoint(myRelativeClickedPosition))) {
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getEdgeFront()->getMoveOperation();
            // continue if move operation is valid
            if (moveOperation) {
                myMoveOperations.push_back(moveOperation);
                return true;
            } else {
                return false;
            }
        } else {
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getJunctionFront()->getMoveOperation();
            // continue if move operation is valid
            if (moveOperation) {
                myMoveOperations.push_back(moveOperation);
                return true;
            } else {
                return false;
            }
        }
    } else if ((myViewNet->myObjectsUnderCursor.getEdgeFront() && (frontAC == myViewNet->myObjectsUnderCursor.getEdgeFront())) ||
               (myViewNet->myObjectsUnderCursor.getLaneFront() && (frontAC == myViewNet->myObjectsUnderCursor.getLaneFront()))) {
        // calculate Edge movement values (can be entire shape, single geometry points, altitude, etc.)
        if (myViewNet->myMouseButtonKeyPressed.shiftKeyPressed()) {
            // edit end point
            myViewNet->myObjectsUnderCursor.getEdgeFront()->editEndpoint(myViewNet->getPositionInformation(), myViewNet->myUndoList);
            // edge values wasn't calculated, then return false
            return false;
        } else {
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getEdgeFront()->getMoveOperation();
            // continue if move operation is valid
            if (moveOperation) {
                myMoveOperations.push_back(moveOperation);
                return true;
            } else {
                return false;
            }
        }
    } else {
        // there isn't moved items, then return false
        return false;
    }
}


bool
GNEViewNetHelper::MoveSingleElementValues::beginMoveSingleElementDemandMode() {
    // first obtain moving reference (common for all)
    myRelativeClickedPosition = myViewNet->getPositionInformation();
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->myObjectsUnderCursor.getAttributeCarrierFront();
    // check demand element
    if (myViewNet->myObjectsUnderCursor.getDemandElementFront() && (frontAC == myViewNet->myObjectsUnderCursor.getDemandElementFront())) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getDemandElementFront()->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }

    } else {
        // there isn't moved items, then return false
        return false;
    }
}


void
GNEViewNetHelper::MoveSingleElementValues::moveSingleElement(const bool mouseLeftButtonPressed) {
    // calculate moveOffset
    const GNEMoveOffset moveOffset = calculateMoveOffset();
    // check if mouse button is pressed
    if (mouseLeftButtonPressed) {
        // iterate over all operations
        for (const auto& moveOperation : myMoveOperations) {
            // move elements
            GNEMoveElement::moveElement(myViewNet, moveOperation, moveOffset);
        }
    } else {
        // iterate over all operations
        for (const auto& moveOperation : myMoveOperations) {
            // commit move
            GNEMoveElement::commitMove(myViewNet, moveOperation, moveOffset, myViewNet->getUndoList());
            // don't forget delete move operation
            delete moveOperation;
        }
        // clear move operations
        myMoveOperations.clear();
    }
}


void
GNEViewNetHelper::MoveSingleElementValues::finishMoveSingleElement() {
    // calculate moveOffset
    const GNEMoveOffset moveOffset = calculateMoveOffset();
    // finish all move operations
    for (const auto& moveOperation : myMoveOperations) {
        GNEMoveElement::commitMove(myViewNet, moveOperation, moveOffset, myViewNet->getUndoList());
        // don't forget delete move operation
        delete moveOperation;
    }
    // clear move operations
    myMoveOperations.clear();
}


const GNEMoveOffset
GNEViewNetHelper::MoveSingleElementValues::calculateMoveOffset() const {
    // calculate moveOffset depending of current mouse position and relative clicked position
    // @note  #3521: Add checkBox to allow moving elements... has to be implemented and used here
    Position moveOffset = (myViewNet->getPositionInformation() - myViewNet->myMoveSingleElementValues.myRelativeClickedPosition);
    // calculate Z depending of moveElevation
    if (myViewNet->myNetworkViewOptions.menuCheckMoveElevation->shown() && myViewNet->myNetworkViewOptions.menuCheckMoveElevation->amChecked() == TRUE) {
        // use Y as Z value and return Z move offset
        return GNEMoveOffset(moveOffset.y());
    } else {
        // return X-Y move offset
        return GNEMoveOffset(moveOffset.x(), moveOffset.y());
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::MoveMultipleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::MoveMultipleElementValues::MoveMultipleElementValues(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myMovingSelectedEdge(false),
    myEdgeOffset(0) {
}


void
GNEViewNetHelper::MoveMultipleElementValues::beginMoveSelection() {
    // save clicked position (to calculate offset)
    myClickedPosition = myViewNet->getPositionInformation();
    // continue depending of clicked element
    if (myViewNet->myObjectsUnderCursor.getJunctionFront()) {
        calculateJunctionSelection();
    } else if (myViewNet->myObjectsUnderCursor.getEdgeFront()) {
        calculateEdgeSelection(myViewNet->myObjectsUnderCursor.getEdgeFront());
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::moveSelection(const bool mouseLeftButtonPressed) {
    // calculate moveOffset
    const GNEMoveOffset moveOffset = calculateMoveOffset();
    // check if mouse button is pressed
    if (mouseLeftButtonPressed) {
        // iterate over all operations
        for (const auto& moveOperation : myMoveOperations) {
            // move elements
            GNEMoveElement::moveElement(myViewNet, moveOperation, moveOffset);
        }
    } else if (myMoveOperations.size() > 0) {
        // begin undo list
        myViewNet->getUndoList()->begin(GUIIcon::MODEMOVE, TL("moving selection"));
        // iterate over all operations
        for (const auto& moveOperation : myMoveOperations) {
            // commit move
            GNEMoveElement::commitMove(myViewNet, moveOperation, moveOffset, myViewNet->getUndoList());
            // don't forget delete move operation
            delete moveOperation;
        }
        // end undo list
        myViewNet->getUndoList()->end();
        // clear move operations
        myMoveOperations.clear();
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::finishMoveSelection() {
    // calculate moveOffset
    const GNEMoveOffset moveOffset = calculateMoveOffset();
    // begin undo list
    myViewNet->getUndoList()->begin(GUIIcon::MODEMOVE, TL("moving selection"));
    // finish all move operations
    for (const auto& moveOperation : myMoveOperations) {
        GNEMoveElement::commitMove(myViewNet, moveOperation, moveOffset, myViewNet->getUndoList());
        // don't forget delete move operation
        delete moveOperation;
    }
    // end undo list
    myViewNet->getUndoList()->end();
    // clear move operations
    myMoveOperations.clear();
}


bool
GNEViewNetHelper::MoveMultipleElementValues::isMovingSelection() const {
    return (myMoveOperations.size() > 0);
}


bool
GNEViewNetHelper::MoveMultipleElementValues::isMovingSelectedEdge() const {
    return myMovingSelectedEdge;
}


void
GNEViewNetHelper::MoveMultipleElementValues::resetMovingSelectedEdge() {
    myMovingSelectedEdge = false;
}


double
GNEViewNetHelper::MoveMultipleElementValues::getEdgeOffset() const {
    return myEdgeOffset;
}


const GNEMoveOffset
GNEViewNetHelper::MoveMultipleElementValues::calculateMoveOffset() const {
    // calculate moveOffset depending of current mouse position and relative clicked position
    // @note  #3521: Add checkBox to allow moving elements... has to be implemented and used here
    Position moveOffset = (myViewNet->getPositionInformation() - myClickedPosition);
    // calculate Z depending of moveElevation
    if (myViewNet->myNetworkViewOptions.menuCheckMoveElevation->shown() && myViewNet->myNetworkViewOptions.menuCheckMoveElevation->amChecked() == TRUE) {
        // use Y for Z and return X move offset
        return GNEMoveOffset(moveOffset.y());
    } else {
        // return X-Y move offset
        return GNEMoveOffset(moveOffset.x(), moveOffset.y());
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::calculateJunctionSelection() {
    // declare move operation
    GNEMoveOperation* moveOperation = nullptr;
    // first move all selected junctions
    const auto selectedJunctions = myViewNet->getNet()->getAttributeCarriers()->getSelectedJunctions();
    // iterate over selected junctions
    for (const auto& junction : selectedJunctions) {
        moveOperation = junction->getMoveOperation();
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
    // now move all selected edges
    const auto selectedEdges = myViewNet->getNet()->getAttributeCarriers()->getSelectedEdges();
    // iterate over selected edges
    for (const auto& edge : selectedEdges) {
        moveOperation = edge->getMoveOperation();
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::calculateEdgeSelection(const GNEEdge* clickedEdge) {
    // first move all selected junctions
    const auto selectedJunctions = myViewNet->getNet()->getAttributeCarriers()->getSelectedJunctions();
    // iterate over selected junctions
    for (const auto& junction : selectedJunctions) {
        GNEMoveOperation* moveOperation = junction->getMoveOperation();
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
    // enable moving selected edge flag
    myMovingSelectedEdge = true;
    // get edge shape
    const auto& shape = clickedEdge->getNBEdge()->getGeometry();
    // calculate offset based on the clicked edge shape and convex angle
    if (clickedEdge->isConvexAngle()) {
        myEdgeOffset = shape.nearest_offset_to_point2D(myViewNet->getPositionInformation());
    } else {
        myEdgeOffset = shape.length2D() - shape.nearest_offset_to_point2D(myViewNet->getPositionInformation());
    }
    // now move all selected edges
    const auto selectedEdges = myViewNet->getNet()->getAttributeCarriers()->getSelectedEdges();
    // iterate over edges betwen 0 and 180 degrees
    for (const auto& edge : selectedEdges) {
        GNEMoveOperation* moveOperation = edge->getMoveOperation();
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::VehicleOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::VehicleOptions::VehicleOptions(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::VehicleOptions::buildVehicleOptionsMenuChecks() {
    UNUSED_PARAMETER(myViewNet);
    // currently unused
}


void
GNEViewNetHelper::VehicleOptions::hideVehicleOptionsMenuChecks() {
    // currently unused
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::VehicleTypeOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::VehicleTypeOptions::VehicleTypeOptions(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::VehicleTypeOptions::buildVehicleTypeOptionsMenuChecks() {
    UNUSED_PARAMETER(myViewNet);
    // currently unused
}


void
GNEViewNetHelper::VehicleTypeOptions::hideVehicleTypeOptionsMenuChecks() {
    // currently unused
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::SelectingArea - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::SelectingArea::SelectingArea(GNEViewNet* viewNet) :
    selectingUsingRectangle(false),
    startDrawing(false),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::SelectingArea::beginRectangleSelection() {
    selectingUsingRectangle = true;
    selectionCorner1 = myViewNet->getPositionInformation();
    selectionCorner2 = selectionCorner1;
}


void
GNEViewNetHelper::SelectingArea::moveRectangleSelection() {
    // start drawing
    startDrawing = true;
    // only update selection corner 2
    selectionCorner2 = myViewNet->getPositionInformation();
    // update status bar
    myViewNet->setStatusBarText(TL("Selection width:") + toString(fabs(selectionCorner1.x() - selectionCorner2.x()))
                                + TL(" height:") + toString(fabs(selectionCorner1.y() - selectionCorner2.y()))
                                + TL(" diagonal:") + toString(selectionCorner1.distanceTo2D(selectionCorner2)));
}


void
GNEViewNetHelper::SelectingArea::finishRectangleSelection() {
    // finish rectangle selection
    selectingUsingRectangle = false;
    startDrawing = false;
    // clear status bar
    myViewNet->setStatusBarText("");
}


void
GNEViewNetHelper::SelectingArea::processRectangleSelection() {
    // shift held down on mouse-down and mouse-up and check that rectangle exist
    if ((abs(selectionCorner1.x() - selectionCorner2.x()) > 0.01) &&
            (abs(selectionCorner1.y() - selectionCorner2.y()) > 0.01) &&
            myViewNet->myMouseButtonKeyPressed.shiftKeyPressed()) {
        // create boundary between two corners
        Boundary rectangleBoundary;
        rectangleBoundary.add(selectionCorner1);
        rectangleBoundary.add(selectionCorner2);
        // process selection within boundary
        processBoundarySelection(rectangleBoundary);
    }
}


std::vector<GNEEdge*>
GNEViewNetHelper::SelectingArea::processEdgeRectangleSelection() {
    // declare vector for selection
    std::vector<GNEEdge*> result;
    // shift held down on mouse-down and mouse-up and check that rectangle exist
    if ((abs(selectionCorner1.x() - selectionCorner2.x()) > 0.01) &&
            (abs(selectionCorner1.y() - selectionCorner2.y()) > 0.01) &&
            myViewNet->myMouseButtonKeyPressed.shiftKeyPressed()) {
        // create boundary between two corners
        Boundary rectangleBoundary;
        rectangleBoundary.add(selectionCorner1);
        rectangleBoundary.add(selectionCorner2);
        if (myViewNet->makeCurrent()) {
            // obtain all ACs in Rectangle BOundary
            std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(rectangleBoundary);
            // Filter ACs in Boundary and get only edges
            for (const auto& AC : ACsInBoundary) {
                if (AC.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    result.push_back(dynamic_cast<GNEEdge*>(AC.second));
                }
            }
            myViewNet->makeNonCurrent();
        }
    }
    return result;
}


void
GNEViewNetHelper::SelectingArea::processShapeSelection(const PositionVector& shape) {
    processBoundarySelection(shape.getBoxBoundary());
}


void
GNEViewNetHelper::SelectingArea::drawRectangleSelection(const RGBColor& color) const {
    if (selectingUsingRectangle) {
        GLHelper::pushMatrix();
        glTranslated(0, 0, GLO_RECTANGLESELECTION);
        GLHelper::setColor(color);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2d(selectionCorner1.x(), selectionCorner1.y());
        glVertex2d(selectionCorner1.x(), selectionCorner2.y());
        glVertex2d(selectionCorner2.x(), selectionCorner2.y());
        glVertex2d(selectionCorner2.x(), selectionCorner1.y());
        glEnd();
        GLHelper::popMatrix();
    }
}


void
GNEViewNetHelper::SelectingArea::processBoundarySelection(const Boundary& boundary) {
    if (myViewNet->makeCurrent()) {
        std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(boundary);
        // filter ACsInBoundary depending of current supermode
        std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundaryFiltered;
        for (const auto& AC : ACsInBoundary) {
            if (myViewNet->myEditModes.isCurrentSupermodeNetwork()) {
                if (AC.second->getTagProperty().isNetworkElement() || AC.second->getTagProperty().isAdditionalElement()) {
                    ACsInBoundaryFiltered.insert(AC);
                }
            } else if (myViewNet->myEditModes.isCurrentSupermodeDemand() && AC.second->getTagProperty().isDemandElement()) {
                ACsInBoundaryFiltered.insert(AC);
            } else if (myViewNet->myEditModes.isCurrentSupermodeData() && AC.second->getTagProperty().isGenericData()) {
                ACsInBoundaryFiltered.insert(AC);
            }
        }
        // declare two sets of attribute carriers, one for select and another for unselect
        std::vector<GNEAttributeCarrier*> ACToSelect;
        std::vector<GNEAttributeCarrier*> ACToUnselect;
        // reserve memory (we assume that in the worst case we're going to insert all elements of ACsInBoundaryFiltered
        ACToSelect.reserve(ACsInBoundaryFiltered.size());
        ACToUnselect.reserve(ACsInBoundaryFiltered.size());
        // in restrict AND replace mode all current selected attribute carriers will be unselected
        if ((myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::Operation::RESTRICT) ||
                (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::Operation::REPLACE)) {
            // obtain selected ACs depending of current supermode
            const auto selectedAC = myViewNet->getNet()->getAttributeCarriers()->getSelectedAttributeCarriers(false);
            // add id into ACs to unselect
            for (const auto& AC : selectedAC) {
                ACToUnselect.push_back(AC);
            }
        }
        // iterate over AttributeCarriers obtained of boundary an place it in ACToSelect or ACToUnselect
        for (const auto& AC : ACsInBoundaryFiltered) {
            switch (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode()) {
                case GNESelectorFrame::ModificationMode::Operation::SUB:
                    ACToUnselect.push_back(AC.second);
                    break;
                case GNESelectorFrame::ModificationMode::Operation::RESTRICT:
                    if (std::find(ACToUnselect.begin(), ACToUnselect.end(), AC.second) != ACToUnselect.end()) {
                        ACToSelect.push_back(AC.second);
                    }
                    break;
                default:
                    ACToSelect.push_back(AC.second);
                    break;
            }
        }
        // select junctions and their connections and crossings if Auto select junctions is enabled (note: only for "add mode")
        if (myViewNet->autoSelectNodes() && (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::Operation::ADD)) {
            std::vector<GNEEdge*> edgesToSelect;
            // iterate over ACToSelect and extract edges
            for (const auto& AC : ACToSelect) {
                if (AC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    edgesToSelect.push_back(dynamic_cast<GNEEdge*>(AC));
                }
            }
            // iterate over extracted edges
            for (const auto& edge : edgesToSelect) {
                // select junction source and all their connections and crossings
                ACToSelect.push_back(edge->getFromJunction());
                for (const auto& connection : edge->getFromJunction()->getGNEConnections()) {
                    ACToSelect.push_back(connection);
                }
                for (const auto& crossing : edge->getFromJunction()->getGNECrossings()) {
                    ACToSelect.push_back(crossing);
                }
                // select junction destiny and all their connections crossings
                ACToSelect.push_back(edge->getToJunction());
                for (const auto& connection : edge->getToJunction()->getGNEConnections()) {
                    ACToSelect.push_back(connection);
                }
                for (const auto& crossing : edge->getToJunction()->getGNECrossings()) {
                    ACToSelect.push_back(crossing);
                }
            }
        }
        // only continue if there is ACs to select or unselect
        if ((ACToSelect.size() + ACToUnselect.size()) > 0) {
            // first unselect AC of ACToUnselect and then selects AC of ACToSelect
            myViewNet->myUndoList->begin(GUIIcon::MODESELECT, TL("selection using rectangle"));
            for (const auto& AC : ACToUnselect) {
                AC->setAttribute(GNE_ATTR_SELECTED, "0", myViewNet->myUndoList);
            }
            for (const auto& AC : ACToSelect) {
                if (AC->getTagProperty().isSelectable()) {
                    AC->setAttribute(GNE_ATTR_SELECTED, "1", myViewNet->myUndoList);
                }
            }
            myViewNet->myUndoList->end();
        }
        myViewNet->makeNonCurrent();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::TestingMode - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::TestingMode::TestingMode(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::TestingMode::initTestingMode() {
    // first check if testing mode is enabled and window size is correct
    if (OptionsCont::getOptions().getBool("gui-testing") && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        // make sure that given windows size has exactly two valid int values
        if ((windowSize.size() == 2) && GNEAttributeCarrier::canParse<int>(windowSize[0]) && GNEAttributeCarrier::canParse<int>(windowSize[1])) {
            myTestingWidth = GNEAttributeCarrier::parse<int>(windowSize[0]);
            myTestingHeight = GNEAttributeCarrier::parse<int>(windowSize[1]);
        } else {
            WRITE_ERRORF(TL("Invalid windows size-format: %for option 'window-size'"), toString(windowSize));
        }
    }
}


void
GNEViewNetHelper::TestingMode::drawTestingElements(GUIMainWindow* mainWindow) {
    // first check if testing mode is enabled
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        // check if main windows has to be resized
        if (myTestingWidth > 0) {
            mainWindow->resize(myTestingWidth, myTestingHeight);
        }
        //std::cout << " fixed: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
        // draw pink square in the upper left corner on top of everything
        const double size = myViewNet->p2m(32);
        Position center = myViewNet->screenPos2NetPos(8, 8);
        // magenta
        GLHelper::pushMatrix();
        GLHelper::setColor(RGBColor::MAGENTA);
        glTranslated(center.x(), center.y(), GLO_TESTELEMENT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glVertex2d(0, 0);
        glVertex2d(0, -size);
        glVertex2d(size, -size);
        glVertex2d(size, 0);
        glEnd();
        GLHelper::popMatrix();
        // blue
        GLHelper::pushMatrix();
        GLHelper::setColor(RGBColor::BLUE);
        glTranslated(center.x(), center.y(), GLO_TESTELEMENT + 1);
        glScaled(0.7, 0.7, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glVertex2d(0, 0);
        glVertex2d(0, -size);
        glVertex2d(size, -size);
        glVertex2d(size, 0);
        glEnd();
        GLHelper::popMatrix();
        // yellow
        GLHelper::pushMatrix();
        GLHelper::setColor(RGBColor::YELLOW);
        glTranslated(center.x(), center.y(), GLO_TESTELEMENT + 2);
        glScaled(0.4, 0.4, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glVertex2d(0, 0);
        glVertex2d(0, -size);
        glVertex2d(size, -size);
        glVertex2d(size, 0);
        glEnd();
        GLHelper::popMatrix();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::SaveElements - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::SaveElements::SaveElements(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


GNEViewNetHelper::SaveElements::~SaveElements() {
    delete mySaveIndividualFilesPopup;
}


void
GNEViewNetHelper::SaveElements::buildSaveElementsButtons() {
    // create save sumo config button
    mySaveNeteditConfig = new MFXButtonTooltip(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Save NETEDITConfig") + std::string("\t") + TL("Save NETEDITConfig. (Ctrl+Shift+E)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_NETEDITCONFIG),
            myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG, GUIDesignButtonToolbar);
    mySaveNeteditConfig->create();
    // create save sumo config button
    mySaveSumoConfig = new MFXButtonTooltip(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                                            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                            std::string("\t") + TL("Save SumoConfig") + std::string("\t") + TL("Save SumoConfig. (Ctrl+Shift+S)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_SUMOCONFIG),
                                            myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG, GUIDesignButtonToolbar);
    mySaveSumoConfig->create();
    // create save network button
    mySaveNetwork = new MFXButtonTooltip(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                                         myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                         std::string("\t") + TL("Save network") + std::string("\t") + TL("Save network. (Ctrl+S)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_NETWORKELEMENTS),
                                         myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK, GUIDesignButtonToolbar);
    mySaveNetwork->create();
    // create popup for save individual files
    mySaveIndividualFilesPopup = new FXPopup(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements, POPUP_VERTICAL);
    mySaveIndividualFilesPopup->create();
    // create save individual files button
    mySaveIndividualFiles = new MFXMenuButtonTooltip(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(), std::string("\t") + TL("Save individual files") + std::string("\t") + TL("Save individual files."),
            GUIIconSubSys::getIcon(GUIIcon::SAVE_MULTIPLE), mySaveIndividualFilesPopup, nullptr, GUIDesignButtonToolbarLocator);
    mySaveIndividualFiles->create();
    // create save additional elements button
    mySaveAdditionalElements = new MFXButtonTooltip(mySaveIndividualFilesPopup,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Save additional elements") + std::string("\t") + TL("Save additional elements. (Ctrl+Shift+A)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_ADDITIONALELEMENTS),
            myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS, GUIDesignButtonPopup);
    mySaveAdditionalElements->create();
    // create save demand elements button
    mySaveDemandElements = new MFXButtonTooltip(mySaveIndividualFilesPopup,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Save demand elements") + std::string("\t") + TL("Save demand elements. (Ctrl+Shift+D)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_DEMANDELEMENTS),
            myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS, GUIDesignButtonPopup);
    mySaveDemandElements->create();
    // create save data elements button
    mySaveDataElements = new MFXButtonTooltip(mySaveIndividualFilesPopup,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Save data elements") + std::string("\t") + TL("Save data elements. (Ctrl+Shift+B)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_DATAELEMENTS),
            myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS, GUIDesignButtonPopup);
    mySaveDataElements->create();
    // create save mean datas elements button
    mySaveMeanDataElements = new MFXButtonTooltip(mySaveIndividualFilesPopup,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Save mean data elements") + std::string("\t") + TL("Save mean data elements. (Ctrl+Shift+M)"), GUIIconSubSys::getIcon(GUIIcon::SAVE_MEANDATAELEMENTS),
            myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_M_SAVEMEANDATAS, GUIDesignButtonPopup);
    mySaveMeanDataElements->create();
    // recalc menu bar because there is new elements
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements->recalc();
    // show menu bar modes
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements->show();
}


void
GNEViewNetHelper::SaveElements::setSaveIndividualFiles(bool value) {
    if (value) {
        mySaveIndividualFiles->enable();
    } else {
        mySaveIndividualFiles->disable();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::EditModes - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::EditModes::EditModes(GNEViewNet* viewNet) :
    currentSupermode(Supermode::NETWORK),
    networkEditMode(NetworkEditMode::NETWORK_INSPECT),
    demandEditMode(DemandEditMode::DEMAND_INSPECT),
    dataEditMode(DataEditMode::DATA_INSPECT),
    networkButton(nullptr),
    demandButton(nullptr),
    dataButton(nullptr),
    myViewNet(viewNet) {
    auto& neteditOptions = OptionsCont::getOptions();
    // if new option is enabled, start in create edge mode
    if (neteditOptions.getBool("new")) {
        networkEditMode = NetworkEditMode::NETWORK_CREATE_EDGE;
        neteditOptions.resetWritable();
        neteditOptions.set("new", "false");
    }
}


void
GNEViewNetHelper::EditModes::buildSuperModeButtons() {
    // create network button
    networkButton = new MFXCheckableButton(false,
                                           myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes,
                                           myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                           TL("Network") + std::string("\t") + TL("Edit network elements") + std::string("\t") + TL("Set mode for edit network elements. (F2)"),
                                           GUIIconSubSys::getIcon(GUIIcon::SUPERMODENETWORK), myViewNet, MID_HOTKEY_F2_SUPERMODE_NETWORK, GUIDesignMFXCheckableButtonSupermode);
    networkButton->create();
    // create demand button
    demandButton = new MFXCheckableButton(false,
                                          myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes,
                                          myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                          TL("Demand") + std::string("\t") + TL("Edit traffic demand elements") + std::string("\t") + TL("Set mode for edit traffic demand. (F3)"),
                                          GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), myViewNet, MID_HOTKEY_F3_SUPERMODE_DEMAND, GUIDesignMFXCheckableButtonSupermode);
    demandButton->create();
    // create data button
    dataButton = new MFXCheckableButton(false,
                                        myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes,
                                        myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                        TL("Data") + std::string("\t") + TL("Edit data elements") + std::string("\t") + TL("Set mode for edit data demand. (F4)"),
                                        GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myViewNet, MID_HOTKEY_F4_SUPERMODE_DATA, GUIDesignMFXCheckableButtonSupermode);
    dataButton->create();
    // recalc menu bar because there is new elements
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
    // show menu bar modes
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::EditModes::setSupermode(Supermode supermode, const bool force) {
    if (!force && (supermode == currentSupermode)) {
        myViewNet->setStatusBarText(TL("Mode already selected"));
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else {
        myViewNet->setStatusBarText("");
        // abort current operation
        myViewNet->abortOperation(false);
        // set super mode
        currentSupermode = supermode;
        // set supermodes
        if (supermode == Supermode::NETWORK) {
            // change buttons
            networkButton->setChecked(true);
            demandButton->setChecked(false);
            dataButton->setChecked(false);
            // show network buttons
            myViewNet->myNetworkCheckableButtons.showNetworkCheckableButtons();
            // hide demand buttons
            myViewNet->myDemandCheckableButtons.hideDemandCheckableButtons();
            // hide data buttons
            myViewNet->myDataCheckableButtons.hideDataCheckableButtons();
            // force update network mode
            setNetworkEditMode(networkEditMode, true);
        } else if (supermode == Supermode::DEMAND) {
            // change buttons
            networkButton->setChecked(false);
            demandButton->setChecked(true);
            dataButton->setChecked(false);
            // hide network buttons
            myViewNet->myNetworkCheckableButtons.hideNetworkCheckableButtons();
            // show demand buttons
            myViewNet->myDemandCheckableButtons.showDemandCheckableButtons();
            // hide data buttons
            myViewNet->myDataCheckableButtons.hideDataCheckableButtons();
            // force update demand mode
            setDemandEditMode(demandEditMode, true);
            // demand modes require ALWAYS a recomputing
            myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
            // check if update path calculator
            if (!myViewNet->myNet->getPathManager()->getPathCalculator()->isPathCalculatorUpdated()) {
                // update DijkstraRouter of RouteCalculatorInstance
                myViewNet->myNet->getPathManager()->getPathCalculator()->updatePathCalculator();
                // compute all demand elements
                myViewNet->myNet->computeDemandElements(myViewNet->myViewParent->getGNEAppWindows());
            }
        } else if (supermode == Supermode::DATA) {
            // change buttons
            networkButton->setChecked(false);
            demandButton->setChecked(false);
            dataButton->setChecked(true);
            // hide network buttons
            myViewNet->myNetworkCheckableButtons.hideNetworkCheckableButtons();
            // hide demand buttons
            myViewNet->myDemandCheckableButtons.hideDemandCheckableButtons();
            // show data buttons
            myViewNet->myDataCheckableButtons.showDataCheckableButtons();
            // force update data mode
            setDataEditMode(dataEditMode, true);
            // check if recompute
            if (myViewNet->getViewParent()->getGNEAppWindows()->getProcessingMenuCommands().menuCheckRecomputeDataMode->getCheck() == TRUE) {
                // demand modes require ALWAYS a recomputing
                myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
            }
            // check if update path calculator
            if (!myViewNet->myNet->getPathManager()->getPathCalculator()->isPathCalculatorUpdated()) {
                // update DijkstraRouter of RouteCalculatorInstance
                myViewNet->myNet->getPathManager()->getPathCalculator()->updatePathCalculator();
                // compute all demand elements
                myViewNet->myNet->computeDemandElements(myViewNet->myViewParent->getGNEAppWindows());
            }
        }
        // update buttons
        networkButton->update();
        demandButton->update();
        dataButton->update();
        // update Supermode CommandButtons in GNEAppWindows
        myViewNet->myViewParent->getGNEAppWindows()->updateSuperModeMenuCommands(currentSupermode);
    }
}


void
GNEViewNetHelper::EditModes::setNetworkEditMode(NetworkEditMode mode, const bool force) {
    if ((mode == networkEditMode) && !force) {
        myViewNet->setStatusBarText(TL("Network mode already selected"));
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else if (networkEditMode == NetworkEditMode::NETWORK_TLS && !myViewNet->myViewParent->getTLSEditorFrame()->isTLSSaved()) {
        myViewNet->setStatusBarText(TL("Save modifications in TLS before change mode"));
        myViewNet->myCurrentFrame->focusUpperElement();
    } else {
        myViewNet->setStatusBarText("");
        myViewNet->abortOperation(false);
        // stop editing of custom shapes
        myViewNet->myEditNetworkElementShapes.stopEditCustomShape();
        // set new Network mode
        networkEditMode = mode;
        // for common modes (Inspect/Delete/Select/move) change also the other supermode
        if (networkEditMode == NetworkEditMode::NETWORK_INSPECT) {
            demandEditMode = DemandEditMode::DEMAND_INSPECT;
            dataEditMode = DataEditMode::DATA_INSPECT;
        } else if (networkEditMode == NetworkEditMode::NETWORK_DELETE) {
            demandEditMode = DemandEditMode::DEMAND_DELETE;
            dataEditMode = DataEditMode::DATA_DELETE;
        } else if (networkEditMode == NetworkEditMode::NETWORK_SELECT) {
            demandEditMode = DemandEditMode::DEMAND_SELECT;
            dataEditMode = DataEditMode::DATA_SELECT;
        } else if (networkEditMode == NetworkEditMode::NETWORK_MOVE) {
            demandEditMode = DemandEditMode::DEMAND_MOVE;
        }
        // certain modes require a recomputing
        switch (mode) {
            case NetworkEditMode::NETWORK_CONNECT:
            case NetworkEditMode::NETWORK_PROHIBITION:
            case NetworkEditMode::NETWORK_TLS:
            case NetworkEditMode::NETWORK_WIRE:
                // modes which depend on computed data
                myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
                break;
            default:
                break;
        }
        // update cursors
        myViewNet->updateCursor();
        // update network mode specific controls
        myViewNet->updateNetworkModeSpecificControls();
    }
}


void
GNEViewNetHelper::EditModes::setDemandEditMode(DemandEditMode mode, const bool force) {
    if ((mode == demandEditMode) && !force) {
        myViewNet->setStatusBarText(TL("Demand mode already selected"));
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else {
        myViewNet->setStatusBarText("");
        myViewNet->abortOperation(false);
        // stop editing of custom shapes
        myViewNet->myEditNetworkElementShapes.stopEditCustomShape();
        // set new Demand mode
        demandEditMode = mode;
        // for common modes (Inspect/Delete/Select/Move) change also the other supermode
        if (demandEditMode == DemandEditMode::DEMAND_INSPECT) {
            networkEditMode = NetworkEditMode::NETWORK_INSPECT;
            dataEditMode = DataEditMode::DATA_INSPECT;
        } else if (demandEditMode == DemandEditMode::DEMAND_DELETE) {
            networkEditMode = NetworkEditMode::NETWORK_DELETE;
            dataEditMode = DataEditMode::DATA_DELETE;
        } else if (demandEditMode == DemandEditMode::DEMAND_SELECT) {
            networkEditMode = NetworkEditMode::NETWORK_SELECT;
            dataEditMode = DataEditMode::DATA_SELECT;
        } else if (demandEditMode == DemandEditMode::DEMAND_MOVE) {
            networkEditMode = NetworkEditMode::NETWORK_MOVE;
        }
        // update cursors
        myViewNet->updateCursor();
        // update network mode specific controls
        myViewNet->updateDemandModeSpecificControls();
    }
}


void
GNEViewNetHelper::EditModes::setDataEditMode(DataEditMode mode, const bool force) {
    if ((mode == dataEditMode) && !force) {
        myViewNet->setStatusBarText(TL("Data mode already selected"));
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else {
        myViewNet->setStatusBarText("");
        myViewNet->abortOperation(false);
        // stop editing of custom shapes
        myViewNet->myEditNetworkElementShapes.stopEditCustomShape();
        // set new Data mode
        dataEditMode = mode;
        // for common modes (Inspect/Delete/Select/Move) change also the other supermode
        if (dataEditMode == DataEditMode::DATA_INSPECT) {
            networkEditMode = NetworkEditMode::NETWORK_INSPECT;
            demandEditMode = DemandEditMode::DEMAND_INSPECT;
        } else if (dataEditMode == DataEditMode::DATA_DELETE) {
            networkEditMode = NetworkEditMode::NETWORK_DELETE;
            demandEditMode = DemandEditMode::DEMAND_DELETE;
        } else if (dataEditMode == DataEditMode::DATA_SELECT) {
            networkEditMode = NetworkEditMode::NETWORK_SELECT;
            demandEditMode = DemandEditMode::DEMAND_SELECT;
        }
        // update all datasets
        for (const auto& dataSet : myViewNet->getNet()->getAttributeCarriers()->getDataSets()) {
            dataSet->updateAttributeColors();
        }
        // update cursors
        myViewNet->updateCursor();
        // update network mode specific controls
        myViewNet->updateDataModeSpecificControls();
    }
}


bool
GNEViewNetHelper::EditModes::isCurrentSupermodeNetwork() const {
    return (currentSupermode == Supermode::NETWORK);
}


bool
GNEViewNetHelper::EditModes::isCurrentSupermodeDemand() const {
    return (currentSupermode == Supermode::DEMAND);
}


bool
GNEViewNetHelper::EditModes::isCurrentSupermodeData() const {
    return (currentSupermode == Supermode::DATA);
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::NetworkViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::NetworkViewOptions::NetworkViewOptions(GNEViewNet* viewNet) :
    menuCheckToggleGrid(nullptr),
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckDrawSpreadVehicles(nullptr),
    menuCheckShowDemandElements(nullptr),
    menuCheckSelectEdges(nullptr),
    menuCheckShowConnections(nullptr),
    menuCheckHideConnections(nullptr),
    menuCheckShowAdditionalSubElements(nullptr),
    menuCheckShowTAZElements(nullptr),
    menuCheckExtendSelection(nullptr),
    menuCheckChangeAllPhases(nullptr),
    menuCheckWarnAboutMerge(nullptr),
    menuCheckShowJunctionBubble(nullptr),
    menuCheckMoveElevation(nullptr),
    menuCheckChainEdges(nullptr),
    menuCheckAutoOppositeEdge(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::NetworkViewOptions::buildNetworkViewOptionsMenuChecks() {
    // create menu checks
    menuCheckToggleGrid = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show grid") + std::string("\t") + TL("Show grid and restrict movement to the grid - define grid size in visualization options. (Ctrl+G)")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleGrid->setChecked(false);
    menuCheckToggleGrid->create();

    menuCheckToggleDrawJunctionShape = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Hide junction shape") + std::string("\t") + TL("Toggle hiding junction shape. (Ctrl+J)")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleDrawJunctionShape->setChecked(false);
    menuCheckToggleDrawJunctionShape->create();

    menuCheckDrawSpreadVehicles = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Draw vehicles spread in lane or depart position") + std::string("\t") + TL("Draw vehicles spread in lane or in depart position.")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES, GUIDesignMFXCheckableButtonSquare);
    menuCheckDrawSpreadVehicles->setChecked(false);
    menuCheckDrawSpreadVehicles->create();

    menuCheckShowDemandElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show demand elements") + std::string("\t") + TL("Toggle show demand elements.")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowDemandElements->setChecked(false);
    menuCheckShowDemandElements->create();

    menuCheckSelectEdges = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Clicks target lanes") + std::string("\t") + TL("Toggle whether clicking should inspect/select/delete lanes instead of edges.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES, GUIDesignMFXCheckableButtonSquare);
    menuCheckSelectEdges->create();

    menuCheckShowConnections = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show connections over junctions") + std::string("\t") + TL("Toggle show connections over junctions.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowConnections->setChecked(myViewNet->getVisualisationSettings().showLane2Lane);
    menuCheckShowConnections->create();

    menuCheckHideConnections = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Hide connections") + std::string("\t") + TL("Toggle hide connections.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_HIDECONNECTIONS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS, GUIDesignMFXCheckableButtonSquare);
    menuCheckHideConnections->setChecked(false);
    menuCheckHideConnections->create();

    menuCheckShowAdditionalSubElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show additional sub-elements") + std::string("\t") + TL("Toggle show additional sub-elements.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWSUBADDITIONALS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowAdditionalSubElements->setChecked(false);
    menuCheckShowAdditionalSubElements->create();

    menuCheckShowTAZElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show TAZ elements") + std::string("\t") + TL("Toggle show TAZ elements.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWTAZELEMENTS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowTAZElements->setChecked(false);
    menuCheckShowTAZElements->create();

    menuCheckExtendSelection = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Automatic select junctions") + std::string("\t") + TL("Toggle whether selecting multiple edges should automatically select their junctions.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION, GUIDesignMFXCheckableButtonSquare);
    menuCheckExtendSelection->setChecked(true);
    menuCheckExtendSelection->create();

    menuCheckChangeAllPhases = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Apply state to all phases") + std::string("\t") + TL("Toggle whether clicking should apply state changes to all phases of the current TLS plan.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_APPLYTOALLPHASES),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES, GUIDesignMFXCheckableButtonSquare);
    menuCheckChangeAllPhases->setChecked(false);
    menuCheckChangeAllPhases->create();

    menuCheckWarnAboutMerge = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Automatic merging junction") + std::string("\t") + TL("Toggle ask for confirmation before merging junction.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ASKFORMERGE),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE, GUIDesignMFXCheckableButtonSquare);
    menuCheckWarnAboutMerge->create();

    menuCheckShowJunctionBubble = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show bubbles") + std::string("\t") + TL("Toggle show bubbles over junctions shapes.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_BUBBLES),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowJunctionBubble->setChecked(false);
    menuCheckShowJunctionBubble->create();

    menuCheckMoveElevation = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Move elevation") + std::string("\t") + TL("Apply mouse movement to elevation instead of x,y position.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ELEVATION),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION, GUIDesignMFXCheckableButtonSquare);
    menuCheckMoveElevation->setChecked(false);
    menuCheckMoveElevation->create();

    menuCheckChainEdges = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Edge chain mode") + std::string("\t") + TL("Create consecutive edges with a single click (hit ESC to cancel chain).")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_CHAIN),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES, GUIDesignMFXCheckableButtonSquare);
    menuCheckChainEdges->setChecked(false);
    menuCheckChainEdges->create();

    menuCheckAutoOppositeEdge = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Edge opposite direction") + std::string("\t") + TL("Automatically create an edge in the opposite direction.")),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_TWOWAY),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES, GUIDesignMFXCheckableButtonSquare);
    menuCheckAutoOppositeEdge->setChecked(false);
    menuCheckAutoOppositeEdge->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::NetworkViewOptions::hideNetworkViewOptionsMenuChecks() {
    menuCheckToggleGrid->hide();
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckDrawSpreadVehicles->hide();
    menuCheckShowDemandElements->hide();
    menuCheckSelectEdges->hide();
    menuCheckShowConnections->hide();
    menuCheckHideConnections->hide();
    menuCheckShowAdditionalSubElements->hide();
    menuCheckShowTAZElements->hide();
    menuCheckExtendSelection->hide();
    menuCheckChangeAllPhases->hide();
    menuCheckWarnAboutMerge->hide();
    menuCheckShowJunctionBubble->hide();
    menuCheckMoveElevation->hide();
    menuCheckChainEdges->hide();
    menuCheckAutoOppositeEdge->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::NetworkViewOptions::getVisibleNetworkMenuCommands(std::vector<MFXCheckableButton*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckToggleGrid->shown()) {
        commands.push_back(menuCheckToggleGrid);
    }
    if (menuCheckToggleDrawJunctionShape->shown()) {
        commands.push_back(menuCheckToggleDrawJunctionShape);
    }
    if (menuCheckDrawSpreadVehicles->shown()) {
        commands.push_back(menuCheckDrawSpreadVehicles);
    }
    if (menuCheckShowDemandElements->shown()) {
        commands.push_back(menuCheckShowDemandElements);
    }
    if (menuCheckSelectEdges->shown()) {
        commands.push_back(menuCheckSelectEdges);
    }
    if (menuCheckShowConnections->shown()) {
        commands.push_back(menuCheckShowConnections);
    }
    if (menuCheckHideConnections->shown()) {
        commands.push_back(menuCheckHideConnections);
    }
    if (menuCheckShowAdditionalSubElements->shown()) {
        commands.push_back(menuCheckShowAdditionalSubElements);
    }
    if (menuCheckShowTAZElements->shown()) {
        commands.push_back(menuCheckShowTAZElements);
    }
    if (menuCheckExtendSelection->shown()) {
        commands.push_back(menuCheckExtendSelection);
    }
    if (menuCheckChangeAllPhases->shown()) {
        commands.push_back(menuCheckChangeAllPhases);
    }
    if (menuCheckWarnAboutMerge->shown()) {
        commands.push_back(menuCheckWarnAboutMerge);
    }
    if (menuCheckShowJunctionBubble->shown()) {
        commands.push_back(menuCheckShowJunctionBubble);
    }
    if (menuCheckMoveElevation->shown()) {
        commands.push_back(menuCheckMoveElevation);
    }
    if (menuCheckChainEdges->shown()) {
        commands.push_back(menuCheckChainEdges);
    }
    if (menuCheckAutoOppositeEdge->shown()) {
        commands.push_back(menuCheckAutoOppositeEdge);
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::drawSpreadVehicles() const {
    return (menuCheckDrawSpreadVehicles->amChecked() == TRUE);
}


bool
GNEViewNetHelper::NetworkViewOptions::showDemandElements() const {
    if (menuCheckShowDemandElements->shown()) {
        return (menuCheckShowDemandElements->amChecked() == TRUE);
    } else {
        // by default, if menuCheckShowDemandElements isn't shown, always show demand elements
        return true;
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::selectEdges() const {
    if (menuCheckSelectEdges->shown()) {
        return (menuCheckSelectEdges->amChecked() != TRUE);
    } else {
        // by default, if menuCheckSelectEdges isn't shown, always select edges
        return true;
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::showConnections() const {
    if (myViewNet->myEditModes.isCurrentSupermodeData()) {
        return false;
    } else if (myViewNet->myEditModes.networkEditMode == NetworkEditMode::NETWORK_CONNECT) {
        // check if menu check hide connections ins shown
        return (menuCheckHideConnections->amChecked() == FALSE);
    } else if (myViewNet->myEditModes.networkEditMode == NetworkEditMode::NETWORK_PROHIBITION) {
        return true;
    } else if (myViewNet->myEditModes.isCurrentSupermodeNetwork() && menuCheckShowConnections->shown() == false) {
        return false;
    } else {
        return (myViewNet->getVisualisationSettings().showLane2Lane);
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::showSubAdditionals() const {
    if (!myViewNet->myEditModes.isCurrentSupermodeNetwork()) {
        return false;
    } else if (menuCheckShowAdditionalSubElements->shown() == false) {
        return false;
    } else {
        return menuCheckShowAdditionalSubElements->amChecked();
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::showTAZElements() const {
    if (!myViewNet->myEditModes.isCurrentSupermodeNetwork()) {
        return false;
    } else if (menuCheckShowTAZElements->shown() == false) {
        return false;
    } else {
        return menuCheckShowTAZElements->amChecked();
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::editingElevation() const {
    if (menuCheckMoveElevation->shown()) {
        return (menuCheckMoveElevation->amChecked() == TRUE);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DemandViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DemandViewOptions::DemandViewOptions(GNEViewNet* viewNet) :
    menuCheckToggleGrid(nullptr),
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckDrawSpreadVehicles(nullptr),
    menuCheckHideShapes(nullptr),
    menuCheckShowAllTrips(nullptr),
    menuCheckShowAllPersonPlans(nullptr),
    menuCheckLockPerson(nullptr),
    menuCheckShowAllContainerPlans(nullptr),
    menuCheckLockContainer(nullptr),
    menuCheckHideNonInspectedDemandElements(nullptr),
    menuCheckShowOverlappedRoutes(nullptr),
    myViewNet(viewNet),
    myLockedPerson(nullptr),
    myLockedContainer(nullptr) {
}


void
GNEViewNetHelper::DemandViewOptions::buildDemandViewOptionsMenuChecks() {
    // create menu checks
    menuCheckToggleGrid = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show grid") + std::string("\t") + TL("Show grid and restrict movement to the grid - define grid size in visualization options. (Ctrl+G)")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleGrid->setChecked(false);
    menuCheckToggleGrid->create();

    menuCheckToggleDrawJunctionShape = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Hide junction shape") + std::string("\t") + TL("Toggle hiding junction shape. (Ctrl+J)")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleDrawJunctionShape->setChecked(false);
    menuCheckToggleDrawJunctionShape->create();

    menuCheckDrawSpreadVehicles = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Draw vehicles spread/depart position") + std::string("\t") + TL("Draw vehicles spread in lane or in depart position.")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES, GUIDesignMFXCheckableButtonSquare);
    menuCheckDrawSpreadVehicles->setChecked(false);
    menuCheckDrawSpreadVehicles->create();

    menuCheckHideShapes = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show shapes") + std::string("\t") + TL("Toggle show shapes (Polygons and POIs).")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDESHAPES),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES, GUIDesignMFXCheckableButtonSquare);
    menuCheckHideShapes->setChecked(false);
    menuCheckHideShapes->create();

    menuCheckShowAllTrips = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show all trips") + std::string("\t") + TL("Toggle show all trips (requires updated demand - F5).")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWTRIPS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowAllTrips->setChecked(false);
    menuCheckShowAllTrips->create();

    menuCheckShowAllPersonPlans = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show all person plans") + std::string("\t") + TL("Toggle show all person plans.")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWPERSONPLANS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowAllPersonPlans->setChecked(false);
    menuCheckShowAllPersonPlans->create();

    menuCheckLockPerson = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Lock selected person") + std::string("\t") + TL("Toggle lock selected person.")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKPERSON),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON, GUIDesignMFXCheckableButtonSquare);
    menuCheckLockPerson->setChecked(false);
    menuCheckLockPerson->create();

    menuCheckShowAllContainerPlans = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show all containers plans") + std::string("\t") + TL("Toggle show all container plans.")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowAllContainerPlans->setChecked(false);
    menuCheckShowAllContainerPlans->create();

    menuCheckLockContainer = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Lock selected container") + std::string("\t") + TL("Toggle lock selected container.")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKCONTAINER),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER, GUIDesignMFXCheckableButtonSquare);
    menuCheckLockContainer->setChecked(false);
    menuCheckLockContainer->create();

    menuCheckHideNonInspectedDemandElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show non-inspected demand elements") + std::string("\t") + TL("Toggle show non-inspected demand elements.")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED, GUIDesignMFXCheckableButtonSquare);
    menuCheckHideNonInspectedDemandElements->setChecked(false);
    menuCheckHideNonInspectedDemandElements->create();

    menuCheckShowOverlappedRoutes = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show number of overlapped routes") + std::string("\t") + TL("Toggle show number of overlapped routes.")),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWOVERLAPPEDROUTES),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowOverlappedRoutes->setChecked(false);
    menuCheckShowOverlappedRoutes->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DemandViewOptions::hideDemandViewOptionsMenuChecks() {
    menuCheckToggleGrid->hide();
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckDrawSpreadVehicles->hide();
    menuCheckHideShapes->hide();
    menuCheckShowAllTrips->hide();
    menuCheckShowAllPersonPlans->hide();
    menuCheckLockPerson->hide();
    menuCheckShowAllContainerPlans->hide();
    menuCheckLockContainer->hide();
    menuCheckHideNonInspectedDemandElements->hide();
    menuCheckShowOverlappedRoutes->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::DemandViewOptions::getVisibleDemandMenuCommands(std::vector<MFXCheckableButton*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckToggleGrid->shown()) {
        commands.push_back(menuCheckToggleGrid);
    }
    if (menuCheckToggleDrawJunctionShape->shown()) {
        commands.push_back(menuCheckToggleDrawJunctionShape);
    }
    if (menuCheckDrawSpreadVehicles->shown()) {
        commands.push_back(menuCheckDrawSpreadVehicles);
    }
    if (menuCheckHideShapes->shown()) {
        commands.push_back(menuCheckHideShapes);
    }
    if (menuCheckShowAllTrips->shown()) {
        commands.push_back(menuCheckShowAllTrips);
    }
    if (menuCheckShowAllPersonPlans->shown() && menuCheckShowAllPersonPlans->isEnabled()) {
        commands.push_back(menuCheckShowAllPersonPlans);
    }
    if (menuCheckLockPerson->shown() && menuCheckLockPerson->isEnabled()) {
        commands.push_back(menuCheckLockPerson);
    }
    if (menuCheckShowAllContainerPlans->shown() && menuCheckShowAllContainerPlans->isEnabled()) {
        commands.push_back(menuCheckShowAllContainerPlans);
    }
    if (menuCheckLockContainer->shown() && menuCheckLockContainer->isEnabled()) {
        commands.push_back(menuCheckLockContainer);
    }
    if (menuCheckHideNonInspectedDemandElements->shown()) {
        commands.push_back(menuCheckHideNonInspectedDemandElements);
    }
    if (menuCheckShowOverlappedRoutes->shown()) {
        commands.push_back(menuCheckShowOverlappedRoutes);
    }
}


bool
GNEViewNetHelper::DemandViewOptions::drawSpreadVehicles() const {
    return (menuCheckDrawSpreadVehicles->amChecked() == TRUE);
}


bool
GNEViewNetHelper::DemandViewOptions::showNonInspectedDemandElements(const GNEDemandElement* demandElement) const {
    if (menuCheckHideNonInspectedDemandElements->shown()) {
        // check conditions
        if ((menuCheckHideNonInspectedDemandElements->amChecked() == FALSE) || (myViewNet->getInspectedAttributeCarriers().empty())) {
            // if checkbox is disabled or there isn't insepected element, then return true
            return true;
        } else if (myViewNet->getInspectedAttributeCarriers().front()->getTagProperty().isDemandElement()) {
            if (myViewNet->isAttributeCarrierInspected(demandElement)) {
                // if inspected element correspond to demandElement, return true
                return true;
            } else {
                // if demandElement is a route, check if dottedAC is one of their children (Vehicle or Stop)
                for (const auto& i : demandElement->getChildDemandElements()) {
                    if (myViewNet->isAttributeCarrierInspected(i)) {
                        return true;
                    }
                }
                // if demandElement is a vehicle, check if dottedAC is one of his route Parent
                for (const auto& i : demandElement->getParentDemandElements()) {
                    if (myViewNet->isAttributeCarrierInspected(i)) {
                        return true;
                    }
                }
                // dottedAC isn't one of their parent, then return false
                return false;
            }
        } else {
            // we're inspecting a demand element, then return true
            return true;
        }
    } else {
        // we're inspecting a demand element, then return true
        return true;
    }
}


bool
GNEViewNetHelper::DemandViewOptions::showShapes() const {
    if (menuCheckHideShapes->shown()) {
        return (menuCheckHideShapes->amChecked() == FALSE);
    } else {
        return true;
    }
}


bool
GNEViewNetHelper::DemandViewOptions::showAllTrips() const {
    return (menuCheckShowAllTrips->amChecked() == TRUE);
}


bool
GNEViewNetHelper::DemandViewOptions::showAllPersonPlans() const {
    if (menuCheckShowAllPersonPlans->isEnabled()) {
        return (menuCheckShowAllPersonPlans->amChecked() == TRUE);
    } else {
        return false;
    }
}


void
GNEViewNetHelper::DemandViewOptions::lockPerson(const GNEDemandElement* person) {
    myLockedPerson = person;
}


void
GNEViewNetHelper::DemandViewOptions::unlockPerson() {
    myLockedPerson = nullptr;
}


const GNEDemandElement*
GNEViewNetHelper::DemandViewOptions::getLockedPerson() const {
    return myLockedPerson;
}


bool
GNEViewNetHelper::DemandViewOptions::showAllContainerPlans() const {
    if (menuCheckShowAllContainerPlans->isEnabled()) {
        return (menuCheckShowAllContainerPlans->amChecked() == TRUE);
    } else {
        return false;
    }
}


void
GNEViewNetHelper::DemandViewOptions::lockContainer(const GNEDemandElement* container) {
    myLockedContainer = container;
}


void
GNEViewNetHelper::DemandViewOptions::unlockContainer() {
    myLockedContainer = nullptr;
}


bool
GNEViewNetHelper::DemandViewOptions::showOverlappedRoutes() const {
    if (menuCheckShowOverlappedRoutes->isEnabled()) {
        return (menuCheckShowOverlappedRoutes->amChecked() == TRUE);
    } else {
        return false;
    }
}


const GNEDemandElement*
GNEViewNetHelper::DemandViewOptions::getLockedContainer() const {
    return myLockedContainer;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DataViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DataViewOptions::DataViewOptions(GNEViewNet* viewNet) :
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckShowAdditionals(nullptr),
    menuCheckShowShapes(nullptr),
    menuCheckShowDemandElements(nullptr),
    menuCheckToggleTAZRelDrawing(nullptr),
    menuCheckToggleTAZDrawFill(nullptr),
    menuCheckToggleTAZRelOnlyFrom(nullptr),
    menuCheckToggleTAZRelOnlyTo(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::DataViewOptions::buildDataViewOptionsMenuChecks() {
    // create menu checks
    menuCheckToggleDrawJunctionShape = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Hide junction shape") + std::string("\t") + TL("Toggle hiding junction shape. (Ctrl+J)")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleDrawJunctionShape->setChecked(false);
    menuCheckToggleDrawJunctionShape->create();

    menuCheckShowAdditionals = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show additionals") + std::string("\t") + TL("Toggle show additionals.")),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWADDITIONALS),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowAdditionals->create();

    menuCheckShowShapes = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show shapes") + std::string("\t") + TL("Toggle show shapes (Polygons and POIs).")),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWSHAPES),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowShapes->create();

    menuCheckShowDemandElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Show demand elements") + std::string("\t") + TL("Toggle show demand elements.")),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS, GUIDesignMFXCheckableButtonSquare);
    menuCheckShowDemandElements->create();

    menuCheckToggleTAZRelDrawing = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Draw TAZREL drawing mode") + std::string("\t") + TL("Toggle draw TAZREL drawing mode.")),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELDRAWING),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleTAZRelDrawing->create();

    menuCheckToggleTAZDrawFill = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Toggle draw TAZ fill") + std::string("\t") + TL("Toggle draw TAZ fill")),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZDRAWFILL),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleTAZDrawFill->create();

    menuCheckToggleTAZRelOnlyFrom = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Toggle draw TAZRel only from") + std::string("\t") + TL("Toggle draw TAZRel only from")),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYFROM),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleTAZRelOnlyFrom->create();

    menuCheckToggleTAZRelOnlyTo = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            (std::string("\t") + TL("Toggle draw TAZRel only to") + std::string("\t") + TL("Toggle draw TAZRel only to")),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYTO),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO, GUIDesignMFXCheckableButtonSquare);
    menuCheckToggleTAZRelOnlyTo->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DataViewOptions::hideDataViewOptionsMenuChecks() {
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckShowAdditionals->hide();
    menuCheckShowShapes->hide();
    menuCheckShowDemandElements->hide();
    menuCheckToggleTAZRelDrawing->hide();
    menuCheckToggleTAZDrawFill->hide();
    menuCheckToggleTAZRelOnlyFrom->hide();
    menuCheckToggleTAZRelOnlyTo->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::DataViewOptions::getVisibleDataMenuCommands(std::vector<MFXCheckableButton*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckToggleDrawJunctionShape->shown()) {
        commands.push_back(menuCheckToggleDrawJunctionShape);
    }
    if (menuCheckShowAdditionals->shown()) {
        commands.push_back(menuCheckShowAdditionals);
    }
    if (menuCheckShowShapes->shown()) {
        commands.push_back(menuCheckShowShapes);
    }
    if (menuCheckShowDemandElements->shown()) {
        commands.push_back(menuCheckShowDemandElements);
    }
    if (menuCheckToggleTAZRelDrawing->shown()) {
        commands.push_back(menuCheckToggleTAZRelDrawing);
    }
    if (menuCheckToggleTAZDrawFill->shown()) {
        commands.push_back(menuCheckToggleTAZDrawFill);
    }
    if (menuCheckToggleTAZRelOnlyFrom->shown()) {
        commands.push_back(menuCheckToggleTAZRelOnlyFrom);
    }
    if (menuCheckToggleTAZRelOnlyTo->shown()) {
        commands.push_back(menuCheckToggleTAZRelOnlyTo);
    }
}


bool
GNEViewNetHelper::DataViewOptions::showAdditionals() const {
    if (menuCheckShowAdditionals->shown()) {
        return (menuCheckShowAdditionals->amChecked() == TRUE);
    } else {
        return true;
    }
}


bool
GNEViewNetHelper::DataViewOptions::showShapes() const {
    if (menuCheckShowShapes->shown()) {
        return (menuCheckShowShapes->amChecked() == TRUE);
    } else {
        return true;
    }
}


bool
GNEViewNetHelper::DataViewOptions::showDemandElements() const {
    if (menuCheckShowDemandElements->shown()) {
        return (menuCheckShowDemandElements->amChecked() == TRUE);
    } else {
        return true;
    }
}


bool
GNEViewNetHelper::DataViewOptions::TAZRelDrawing() const {
    return (menuCheckToggleTAZRelDrawing->amChecked() != TRUE);
}


bool
GNEViewNetHelper::DataViewOptions::TAZDrawFill() const {
    if (menuCheckToggleTAZDrawFill->shown()) {
        return (menuCheckToggleTAZDrawFill->amChecked() != TRUE);
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::DataViewOptions::TAZRelOnlyFrom() const {
    if (menuCheckToggleTAZRelOnlyFrom->shown()) {
        return (menuCheckToggleTAZRelOnlyFrom->amChecked() != TRUE);
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::DataViewOptions::TAZRelOnlyTo() const {
    if (menuCheckToggleTAZRelOnlyTo->shown()) {
        return (menuCheckToggleTAZRelOnlyTo->amChecked() != TRUE);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::IntervalBar - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::IntervalBar::IntervalBar(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myUpdateInterval(true),
    myGenericDataTypesComboBox(nullptr),
    myDataSetsComboBox(nullptr),
    myIntervalCheckBox(nullptr),
    myBeginTextField(nullptr),
    myEndTextField(nullptr),
    myParametersComboBox(nullptr) {
}


void
GNEViewNetHelper::IntervalBar::buildIntervalBarElements() {
    // create interval label
    FXLabel* genericDataLabel = new FXLabel(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                            "Data type", 0, GUIDesignLabelThickedFixed(100));
    genericDataLabel->create();
    // create combo box for generic datas
    myGenericDataTypesComboBox = new FXComboBox(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
            GUIDesignComboBoxNCol, myViewNet, MID_GNE_INTERVALBAR_GENERICDATATYPE, GUIDesignComboBoxWidth180);
    myGenericDataTypesComboBox->create();
    // fill combo box
    myGenericDataTypesComboBox->appendItem("<all>");
    myGenericDataTypesComboBox->appendItem(toString(GNE_TAG_EDGEREL_SINGLE).c_str());
    myGenericDataTypesComboBox->appendItem(toString(SUMO_TAG_EDGEREL).c_str());
    myGenericDataTypesComboBox->appendItem(toString(SUMO_TAG_TAZREL).c_str());
    myGenericDataTypesComboBox->setNumVisible(myGenericDataTypesComboBox->getNumItems());
    // create dataSet label
    FXLabel* dataSetLabel = new FXLabel(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                        "Data sets", 0, GUIDesignLabelThickedFixed(100));
    dataSetLabel->create();
    // create combo box for sets
    myDataSetsComboBox = new FXComboBox(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                        GUIDesignComboBoxNCol, myViewNet, MID_GNE_INTERVALBAR_DATASET, GUIDesignComboBoxWidth180);
    myDataSetsComboBox->create();
    // create checkbutton for myLimitByInterval
    myIntervalCheckBox = new FXCheckButton(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                           "Interval", myViewNet, MID_GNE_INTERVALBAR_LIMITED, GUIDesignCheckButtonAttribute);
    myIntervalCheckBox->create();
    // create textfield for begin
    myBeginTextField = new FXTextField(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                       GUIDesignTextFieldNCol, myViewNet, MID_GNE_INTERVALBAR_BEGIN, GUIDesignTextFieldFixedRestricted(50, TEXTFIELD_REAL));
    myBeginTextField->create();
    // create text field for end
    myEndTextField = new FXTextField(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                     GUIDesignTextFieldNCol, myViewNet, MID_GNE_INTERVALBAR_END, GUIDesignTextFieldFixedRestricted(50, TEXTFIELD_REAL));
    myEndTextField->create();
    // create parameter label
    FXLabel* parameterLabel = new FXLabel(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                          "Parameter", 0, GUIDesignLabelThickedFixed(100));
    parameterLabel->create();
    // create combo box for attributes
    myParametersComboBox = new FXComboBox(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                          GUIDesignComboBoxNCol, myViewNet, MID_GNE_INTERVALBAR_PARAMETER, GUIDesignComboBoxWidth180);
    myParametersComboBox->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar->recalc();
}


void
GNEViewNetHelper::IntervalBar::showIntervalBar() {
    // first update interval bar
    updateIntervalBar();
    // show toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar->show();
}


void
GNEViewNetHelper::IntervalBar::hideIntervalBar() {
    // hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar->hide();
}


void
GNEViewNetHelper::IntervalBar::updateIntervalBar() {
    // check if intervalBar has to be updated
    if (myUpdateInterval && myViewNet->getNet()) {
        // clear data sets
        myDataSets.clear();
        // declare intervals
        double begin = INVALID_DOUBLE;
        double end = INVALID_DOUBLE;
        // clear parameters
        myParameters.clear();
        // iterate over all data elements
        for (const auto& dataSet : myViewNet->getNet()->getAttributeCarriers()->getDataSets()) {
            // add data set ID
            myDataSets.push_back(dataSet->getID());
            // iterate over all intervals
            for (const auto& interval : dataSet->getDataIntervalChildren()) {
                // set intervals
                if ((begin == INVALID_DOUBLE) || (interval.first < begin)) {
                    begin = interval.first;
                }
                if ((end == INVALID_DOUBLE) || (interval.first > end)) {
                    end = interval.first;
                }
                // iterate over all generic datas
                for (const auto& genericData : interval.second->getGenericDataChildren()) {
                    // iterate over parameters
                    for (const auto& parameter : genericData->getParametersMap()) {
                        myParameters.insert(parameter.first);
                    }
                }
            }
        }
        // get previous dataSet
        const std::string previousDataSet = myDataSetsComboBox->getText().text();
        // get previous interval
        const std::string previousBegin = (myIntervalCheckBox->getCheck() == TRUE) ? myBeginTextField->getText().text() : "";
        const std::string previousEnd = (myIntervalCheckBox->getCheck() == TRUE) ? myEndTextField->getText().text() : "";
        // get previous parameter
        const std::string previousParameter = myParametersComboBox->getText().text();
        // clear comboBoxes
        myDataSetsComboBox->clearItems();
        myParametersComboBox->clearItems();
        // add first item (all)
        myDataSetsComboBox->appendItem("<all>");
        myParametersComboBox->appendItem("<all>");
        // fill dataSet comboBox
        for (const auto& dataSet : myDataSets) {
            myDataSetsComboBox->appendItem(dataSet.c_str());
        }
        // set begin/end
        myBeginTextField->setText(toString(begin).c_str());
        myEndTextField->setText(toString(end).c_str());
        // fill parameter comboBox
        for (const auto& parameter : myParameters) {
            myParametersComboBox->appendItem(parameter.c_str());
        }
        // check previous dataSet
        myDataSetsComboBox->setCurrentItem(0, FALSE);
        for (int i = 0; i < myDataSetsComboBox->getNumItems(); i++) {
            if (myDataSetsComboBox->getItem(i).text() == previousDataSet) {
                myDataSetsComboBox->setCurrentItem(i);
            }
        }
        // set previous interval
        if (myIntervalCheckBox->getCheck() == TRUE) {
            myBeginTextField->setText(previousBegin.c_str());
            myEndTextField->setText(previousEnd.c_str());
        }
        // check previous parameter
        myParametersComboBox->setCurrentItem(0, FALSE);
        for (int i = 0; i < myParametersComboBox->getNumItems(); i++) {
            if (myParametersComboBox->getItem(i).text() == previousParameter) {
                myParametersComboBox->setCurrentItem(i);
            }
        }
        // set visible elements
        if (myDataSetsComboBox->getNumItems() < 10) {
            myDataSetsComboBox->setNumVisible(myDataSetsComboBox->getNumItems());
        } else {
            myDataSetsComboBox->setNumVisible(10);
        }
        if (myParametersComboBox->getNumItems() < 10) {
            myParametersComboBox->setNumVisible(myParametersComboBox->getNumItems());
        } else {
            myParametersComboBox->setNumVisible(10);
        }
        // check if enable or disable
        if ((myViewNet->getEditModes().dataEditMode == DataEditMode::DATA_INSPECT) ||
                (myViewNet->getEditModes().dataEditMode == DataEditMode::DATA_SELECT) ||
                (myViewNet->getEditModes().dataEditMode == DataEditMode::DATA_DELETE)) {
            enableIntervalBar();
        } else {
            disableIntervalBar();
        }
        // intervalBar updated, then change flag
        myUpdateInterval = false;
    }
}


void
GNEViewNetHelper::IntervalBar::markForUpdate() {
    myUpdateInterval = true;
}


SumoXMLTag
GNEViewNetHelper::IntervalBar::getGenericDataType() const {
    if (myGenericDataTypesComboBox->isEnabled() && (myGenericDataTypesComboBox->getTextColor() == FXRGB(0, 0, 0))) {
        if (myGenericDataTypesComboBox->getText() == toString(GNE_TAG_EDGEREL_SINGLE).c_str()) {
            return GNE_TAG_EDGEREL_SINGLE;
        } else if (myGenericDataTypesComboBox->getText() == toString(SUMO_TAG_EDGEREL).c_str()) {
            return SUMO_TAG_EDGEREL;
        } else if (myGenericDataTypesComboBox->getText() == toString(SUMO_TAG_TAZREL).c_str()) {
            return SUMO_TAG_TAZREL;
        }
    }
    return SUMO_TAG_NOTHING;
}


GNEDataSet*
GNEViewNetHelper::IntervalBar::getDataSet() const {
    if (!myDataSetsComboBox->isEnabled() ||
            (myDataSetsComboBox->getCurrentItem() == 0) ||
            (myDataSetsComboBox->getTextColor() == FXRGB(255, 0, 0))) {
        return nullptr;
    } else {
        return myViewNet->getNet()->getAttributeCarriers()->retrieveDataSet(myDataSetsComboBox->getText().text());
    }
}


double
GNEViewNetHelper::IntervalBar::getBegin() const {
    if (!myIntervalCheckBox->isEnabled() || (myIntervalCheckBox->getCheck() == FALSE)) {
        return INVALID_DOUBLE;
    } else {
        return GNEAttributeCarrier::parse<double>(myBeginTextField->getText().text());
    }
}


double
GNEViewNetHelper::IntervalBar::getEnd() const {
    if (!myIntervalCheckBox->isEnabled() || (myIntervalCheckBox->getCheck() == FALSE)) {
        return INVALID_DOUBLE;
    } else {
        return GNEAttributeCarrier::parse<double>(myEndTextField->getText().text());
    }
}


std::string
GNEViewNetHelper::IntervalBar::getParameter() const {
    if (!myParametersComboBox->isEnabled() ||
            (myParametersComboBox->getCurrentItem() == 0) ||
            (myParametersComboBox->getTextColor() == FXRGB(255, 0, 0))) {
        return "";
    } else {
        return myParametersComboBox->getText().text();
    }
}


void
GNEViewNetHelper::IntervalBar::setGenericDataType() {
    if ((myGenericDataTypesComboBox->getText() == "<all>") ||
            (myGenericDataTypesComboBox->getText() == toString(GNE_TAG_EDGEREL_SINGLE).c_str()) ||
            (myGenericDataTypesComboBox->getText() == toString(SUMO_TAG_EDGEREL).c_str()) ||
            (myGenericDataTypesComboBox->getText() == toString(SUMO_TAG_TAZREL).c_str())) {
        myGenericDataTypesComboBox->setTextColor(FXRGB(0, 0, 0));
    } else {
        myGenericDataTypesComboBox->setTextColor(FXRGB(255, 0, 0));
    }
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setDataSet() {
    // check if exist
    if (std::find(myDataSets.begin(), myDataSets.end(), myDataSetsComboBox->getText().text()) != myDataSets.end()) {
        myDataSetsComboBox->setTextColor(FXRGB(0, 0, 0));
    } else {
        myDataSetsComboBox->setTextColor(FXRGB(255, 0, 0));
    }
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setInterval() {
    // enable or disable text fields
    if (myIntervalCheckBox->isEnabled() && (myIntervalCheckBox->getCheck() == TRUE)) {
        myBeginTextField->enable();
        myEndTextField->enable();
    } else {
        myBeginTextField->disable();
        myEndTextField->disable();
    }
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setBegin() {
    if (myBeginTextField->getText().empty()) {
        myBeginTextField->setText(toString(myViewNet->getNet()->getDataSetIntervalMinimumBegin()).c_str());
        myBeginTextField->setTextColor(FXRGB(0, 0, 0));
    } else if (GNEAttributeCarrier::canParse<double>(myBeginTextField->getText().text())) {
        myBeginTextField->setTextColor(FXRGB(0, 0, 0));
    } else {
        myBeginTextField->setTextColor(FXRGB(255, 0, 0));
    }
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setEnd() {
    if (myEndTextField->getText().empty()) {
        myEndTextField->setText(toString(myViewNet->getNet()->getDataSetIntervalMaximumEnd()).c_str());
        myEndTextField->setTextColor(FXRGB(0, 0, 0));
    } else if (GNEAttributeCarrier::canParse<double>(myEndTextField->getText().text())) {
        myEndTextField->setTextColor(FXRGB(0, 0, 0));
    } else {
        myEndTextField->setTextColor(FXRGB(255, 0, 0));
    }
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setParameter() {
    // check if exist
    if (myParameters.count(myParametersComboBox->getText().text()) > 0) {
        myParametersComboBox->setTextColor(FXRGB(0, 0, 0));
    } else {
        myParametersComboBox->setTextColor(FXRGB(255, 0, 0));
    }
    // update view net
    myViewNet->updateViewNet();
}



void
GNEViewNetHelper::IntervalBar::enableIntervalBar() {
    // enable elements
    myGenericDataTypesComboBox->enable();
    myDataSetsComboBox->enable();
    myIntervalCheckBox->enable();
    if (myIntervalCheckBox->getCheck() == TRUE) {
        myBeginTextField->enable();
        myEndTextField->enable();
    } else {
        myBeginTextField->disable();
        myEndTextField->disable();
    }
    myParametersComboBox->enable();
}


void
GNEViewNetHelper::IntervalBar::disableIntervalBar() {
    // disable all elements
    myGenericDataTypesComboBox->disable();
    myDataSetsComboBox->disable();
    myIntervalCheckBox->disable();
    myBeginTextField->disable();
    myEndTextField->disable();
    myParametersComboBox->disable();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::CommonCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::CommonCheckableButtons::CommonCheckableButtons(GNEViewNet* viewNet) :
    inspectButton(nullptr),
    deleteButton(nullptr),
    selectButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::CommonCheckableButtons::buildCommonCheckableButtons() {
    // inspect button
    inspectButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                           myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                           std::string("\t") + TL("Set inspect mode") + std::string("\t") + TL("Mode for inspect elements and change their attributes. (I)"),
                                           GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), myViewNet, MID_HOTKEY_I_MODE_INSPECT, GUIDesignMFXCheckableButtonSquare);
    inspectButton->create();
    // delete button
    deleteButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                          std::string("\t") + TL("Set delete mode") + std::string("\t") + TL("Mode for delete elements. (D)"),
                                          GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), myViewNet, MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE, GUIDesignMFXCheckableButtonSquare);
    deleteButton->create();
    // select button
    selectButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                          std::string("\t") + ("Set select mode") + std::string("\t") + ("Mode for select elements. (S)"),
                                          GUIIconSubSys::getIcon(GUIIcon::MODESELECT), myViewNet, MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT, GUIDesignMFXCheckableButtonSquare);
    selectButton->create();
    // always recalc menu bar after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::CommonCheckableButtons::showCommonCheckableButtons() {
    inspectButton->show();
    deleteButton->show();
    selectButton->show();
}


void
GNEViewNetHelper::CommonCheckableButtons::hideCommonCheckableButtons() {
    inspectButton->hide();
    deleteButton->hide();
    selectButton->hide();
}


void
GNEViewNetHelper::CommonCheckableButtons::disableCommonCheckableButtons() {
    inspectButton->setChecked(false);
    deleteButton->setChecked(false);
    selectButton->setChecked(false);
}


void
GNEViewNetHelper::CommonCheckableButtons::updateCommonCheckableButtons() {
    inspectButton->update();
    deleteButton->update();
    selectButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::NetworkCheckableButtons::NetworkCheckableButtons(GNEViewNet* viewNet) :
    moveNetworkElementsButton(nullptr),
    createEdgeButton(nullptr),
    connectionButton(nullptr),
    trafficLightButton(nullptr),
    additionalButton(nullptr),
    crossingButton(nullptr),
    TAZButton(nullptr),
    shapeButton(nullptr),
    prohibitionButton(nullptr),
    wireButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::NetworkCheckableButtons::buildNetworkCheckableButtons() {
    // move button
    moveNetworkElementsButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set move mode") + std::string("\t") + TL("Mode for move elements. (M)"),
            GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myViewNet, MID_HOTKEY_M_MODE_MOVE_MEANDATA, GUIDesignMFXCheckableButtonSquare);
    moveNetworkElementsButton->create();
    // create edge
    createEdgeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set create edge mode") + std::string("\t") + TL("Mode for creating junction and edges. (E)"),
            GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), myViewNet, MID_HOTKEY_E_MODE_EDGE_EDGEDATA, GUIDesignMFXCheckableButtonSquare);
    createEdgeButton->create();
    // connection mode
    connectionButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set connection mode") + std::string("\t") + TL("Mode for edit connections between lanes. (C)"),
            GUIIconSubSys::getIcon(GUIIcon::MODECONNECTION), myViewNet, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN, GUIDesignMFXCheckableButtonSquare);
    connectionButton->create();
    // prohibition mode
    prohibitionButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set prohibition mode") + std::string("\t") + TL("Mode for editing connection prohibitions. (H)"),
            GUIIconSubSys::getIcon(GUIIcon::MODEPROHIBITION), myViewNet, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN, GUIDesignMFXCheckableButtonSquare);
    prohibitionButton->create();
    // traffic light mode
    trafficLightButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set traffic light mode") + std::string("\t") + TL("Mode for edit traffic lights over junctions. (T)"),
            GUIIconSubSys::getIcon(GUIIcon::MODETLS), myViewNet, MID_HOTKEY_T_MODE_TLS_TYPE, GUIDesignMFXCheckableButtonSquare);
    trafficLightButton->create();
    // additional mode
    additionalButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set additional mode") + std::string("\t") + TL("Mode for adding additional elements. (A)"),
            GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), myViewNet, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALSTOP, GUIDesignMFXCheckableButtonSquare);
    additionalButton->create();
    // crossing mode
    crossingButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                            std::string("\t") + TL("Set crossing mode") + std::string("\t") + TL("Mode for creating crossings between edges. (R)"),
                                            GUIIconSubSys::getIcon(GUIIcon::MODECROSSING), myViewNet, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA, GUIDesignMFXCheckableButtonSquare);
    crossingButton->create();
    // TAZ Mode
    TAZButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                       myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                       std::string("\t") + TL("Set TAZ mode") + std::string("\t") + TL("Mode for creating Traffic Assignment Zones. (Z)"),
                                       GUIIconSubSys::getIcon(GUIIcon::MODETAZ), myViewNet, MID_HOTKEY_Z_MODE_TAZ_TAZREL, GUIDesignMFXCheckableButtonSquare);
    TAZButton->create();
    // shape mode
    shapeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                         myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                         std::string("\t") + TL("Set polygon mode") + std::string("\t") + TL("Mode for creating polygons and POIs. (P)"),
                                         GUIIconSubSys::getIcon(GUIIcon::MODESHAPE), myViewNet, MID_HOTKEY_P_MODE_POLYGON_PERSON, GUIDesignMFXCheckableButtonSquare);
    shapeButton->create();
    // wire mode
    wireButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                        myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                        std::string("\t") + TL("Set wire mode") + std::string("\t") + TL("Mode for editing wires. (W)"),
                                        GUIIconSubSys::getIcon(GUIIcon::MODEWIRE), myViewNet, MID_HOTKEY_W_MODE_WIRE, GUIDesignMFXCheckableButtonSquare);
    wireButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::NetworkCheckableButtons::showNetworkCheckableButtons() {
    moveNetworkElementsButton->show();
    createEdgeButton->show();
    connectionButton->show();
    trafficLightButton->show();
    additionalButton->show();
    crossingButton->show();
    TAZButton->show();
    shapeButton->show();
    prohibitionButton->show();
    wireButton->show();
}


void
GNEViewNetHelper::NetworkCheckableButtons::hideNetworkCheckableButtons() {
    moveNetworkElementsButton->hide();
    createEdgeButton->hide();
    connectionButton->hide();
    trafficLightButton->hide();
    additionalButton->hide();
    crossingButton->hide();
    TAZButton->hide();
    shapeButton->hide();
    prohibitionButton->hide();
    wireButton->hide();
}


void
GNEViewNetHelper::NetworkCheckableButtons::disableNetworkCheckableButtons() {
    moveNetworkElementsButton->setChecked(false);
    createEdgeButton->setChecked(false);
    connectionButton->setChecked(false);
    trafficLightButton->setChecked(false);
    additionalButton->setChecked(false);
    crossingButton->setChecked(false);
    TAZButton->setChecked(false);
    shapeButton->setChecked(false);
    prohibitionButton->setChecked(false);
    wireButton->setChecked(false);
}


void
GNEViewNetHelper::NetworkCheckableButtons::updateNetworkCheckableButtons() {
    moveNetworkElementsButton->update();
    createEdgeButton->update();
    connectionButton->update();
    trafficLightButton->update();
    additionalButton->update();
    crossingButton->update();
    TAZButton->update();
    shapeButton->update();
    prohibitionButton->update();
    wireButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DemandCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DemandCheckableButtons::DemandCheckableButtons(GNEViewNet* viewNet) :
    moveDemandElementsButton(nullptr),
    routeButton(nullptr),
    vehicleButton(nullptr),
    typeButton(nullptr),
    stopButton(nullptr),
    personButton(nullptr),
    personPlanButton(nullptr),
    containerButton(nullptr),
    containerPlanButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::DemandCheckableButtons::buildDemandCheckableButtons() {
    // move button
    moveDemandElementsButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Set move mode") + std::string("\t") + TL("Mode for move elements. (M)"),
            GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myViewNet, MID_HOTKEY_M_MODE_MOVE_MEANDATA, GUIDesignMFXCheckableButtonSquare);
    moveDemandElementsButton->create();
    // route mode
    routeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                         myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                         std::string("\t") + TL("Create route mode") + std::string("\t") + TL("Mode for creating routes. (R)"),
                                         GUIIconSubSys::getIcon(GUIIcon::MODEROUTE), myViewNet, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA, GUIDesignMFXCheckableButtonSquare);
    routeButton->create();
    // vehicle mode
    vehicleButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                           myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                           std::string("\t") + TL("Create vehicle mode") + std::string("\t") + TL("Mode for creating vehicles. (V)"),
                                           GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), myViewNet, MID_HOTKEY_V_MODE_VEHICLE, GUIDesignMFXCheckableButtonSquare);
    vehicleButton->create();
    // type mode
    typeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                        myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                        std::string("\t") + TL("Create type mode") + std::string("\t") + TL("Mode for creating types (vehicles, person and containers). (T)"),
                                        GUIIconSubSys::getIcon(GUIIcon::MODETYPE), myViewNet, MID_HOTKEY_T_MODE_TLS_TYPE, GUIDesignMFXCheckableButtonSquare);
    typeButton->create();
    // stop mode
    stopButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                        myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                        std::string("\t") + TL("Create stop mode") + std::string("\t") + TL("Mode for creating stops. (A)"),
                                        GUIIconSubSys::getIcon(GUIIcon::MODESTOP), myViewNet, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALSTOP, GUIDesignMFXCheckableButtonSquare);
    stopButton->create();
    // person mode
    personButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                          std::string("\t") + TL("Create person mode") + std::string("\t") + TL("Mode for creating persons. (P)"),
                                          GUIIconSubSys::getIcon(GUIIcon::MODEPERSON), myViewNet, MID_HOTKEY_P_MODE_POLYGON_PERSON, GUIDesignMFXCheckableButtonSquare);
    personButton->create();
    // person plan mode
    personPlanButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Create person plan mode") + std::string("\t") + TL("Mode for creating person plans. (C)"),
            GUIIconSubSys::getIcon(GUIIcon::MODEPERSONPLAN), myViewNet, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN, GUIDesignMFXCheckableButtonSquare);
    personPlanButton->create();
    // container mode
    containerButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Create container mode") + std::string("\t") + TL("Mode for creating containers. (P)"),
            GUIIconSubSys::getIcon(GUIIcon::MODECONTAINER), myViewNet, MID_HOTKEY_G_MODE_CONTAINER, GUIDesignMFXCheckableButtonSquare);
    containerButton->create();
    // container plan mode
    containerPlanButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Create container plan mode") + std::string("\t") + TL("Mode for creating container plans. (H)"),
            GUIIconSubSys::getIcon(GUIIcon::MODECONTAINERPLAN), myViewNet, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN, GUIDesignMFXCheckableButtonSquare);
    containerPlanButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DemandCheckableButtons::showDemandCheckableButtons() {
    moveDemandElementsButton->show();
    routeButton->show();
    vehicleButton->show();
    typeButton->show();
    stopButton->show();
    personButton->show();
    personPlanButton->show();
    containerButton->show();
    containerPlanButton->show();
}


void
GNEViewNetHelper::DemandCheckableButtons::hideDemandCheckableButtons() {
    moveDemandElementsButton->hide();
    routeButton->hide();
    vehicleButton->hide();
    typeButton->hide();
    stopButton->hide();
    personButton->hide();
    personPlanButton->hide();
    containerButton->hide();
    containerPlanButton->hide();
}


void
GNEViewNetHelper::DemandCheckableButtons::disableDemandCheckableButtons() {
    moveDemandElementsButton->setChecked(false);
    routeButton->setChecked(false);
    vehicleButton->setChecked(false);
    typeButton->setChecked(false);
    stopButton->setChecked(false);
    personButton->setChecked(false);
    personPlanButton->setChecked(false);
    containerButton->setChecked(false);
    containerPlanButton->setChecked(false);
}


void
GNEViewNetHelper::DemandCheckableButtons::updateDemandCheckableButtons() {
    moveDemandElementsButton->update();
    routeButton->update();
    vehicleButton->update();
    typeButton->update();
    stopButton->update();
    personButton->update();
    personPlanButton->update();
    containerButton->update();
    containerPlanButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DataCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DataCheckableButtons::DataCheckableButtons(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::DataCheckableButtons::buildDataCheckableButtons() {
    // edgeData mode
    edgeDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                            std::string("\t") + TL("Create edge data mode") + std::string("\t") + TL("Mode for creating edge datas. (E)"),
                                            GUIIconSubSys::getIcon(GUIIcon::MODEEDGEDATA), myViewNet, MID_HOTKEY_E_MODE_EDGE_EDGEDATA, GUIDesignMFXCheckableButtonSquare);
    edgeDataButton->create();
    // edgeRelData mode
    edgeRelDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Create edge relation data mode") + std::string("\t") + TL("Mode for creating edge relation datas. (R)"),
            GUIIconSubSys::getIcon(GUIIcon::MODEEDGERELDATA), myViewNet, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA, GUIDesignMFXCheckableButtonSquare);
    edgeRelDataButton->create();
    // TAZRelData mode
    TAZRelDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
            std::string("\t") + TL("Create TAZ relation data mode") + std::string("\t") + TL("Mode for creating TAZ relation datas. (Z)"),
            GUIIconSubSys::getIcon(GUIIcon::MODETAZRELDATA), myViewNet, MID_HOTKEY_Z_MODE_TAZ_TAZREL, GUIDesignMFXCheckableButtonSquare);
    TAZRelDataButton->create();
    // meanData button
    meanDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                            myViewNet->myViewParent->getGNEAppWindows()->getStaticTooltipMenu(),
                                            std::string("\t") + TL("MeanData mode") + std::string("\t") + TL("Mode for MeanData elements. (M)"),
                                            GUIIconSubSys::getIcon(GUIIcon::MODEMEANDATA), myViewNet, MID_HOTKEY_M_MODE_MOVE_MEANDATA, GUIDesignMFXCheckableButtonSquare);
    meanDataButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DataCheckableButtons::showDataCheckableButtons() {
    edgeDataButton->show();
    edgeRelDataButton->show();
    TAZRelDataButton->show();
    meanDataButton->show();
}


void
GNEViewNetHelper::DataCheckableButtons::hideDataCheckableButtons() {
    edgeDataButton->hide();
    edgeRelDataButton->hide();
    TAZRelDataButton->hide();
    meanDataButton->hide();
}


void
GNEViewNetHelper::DataCheckableButtons::disableDataCheckableButtons() {
    edgeDataButton->setChecked(false);
    edgeRelDataButton->setChecked(false);
    TAZRelDataButton->setChecked(false);
    meanDataButton->setChecked(false);
}


void
GNEViewNetHelper::DataCheckableButtons::updateDataCheckableButtons() {
    edgeDataButton->update();
    edgeRelDataButton->update();
    TAZRelDataButton->update();
    meanDataButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::EditNetworkElementShapes - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::EditNetworkElementShapes::EditNetworkElementShapes(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myEditedNetworkElement(nullptr),
    myPreviousNetworkEditMode(NetworkEditMode::NETWORK_NONE) {
}


void
GNEViewNetHelper::EditNetworkElementShapes::startEditCustomShape(GNENetworkElement* element) {
    if (element && (myEditedNetworkElement == nullptr)) {
        // save current edit mode before starting
        myPreviousNetworkEditMode = myViewNet->myEditModes.networkEditMode;
        // set move mode
        myViewNet->myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_MOVE);
        //set editedNetworkElement
        myEditedNetworkElement = element;
        // enable shape edited flag
        myEditedNetworkElement->setShapeEdited(true);
        // update view net to show the new editedShapePoly
        myViewNet->updateViewNet();
    }
}


void
GNEViewNetHelper::EditNetworkElementShapes::stopEditCustomShape() {
    // stop edit shape junction deleting editedShapePoly
    if (myEditedNetworkElement != nullptr) {
        // disable shape edited flag
        myEditedNetworkElement->setShapeEdited(false);
        // reset editedNetworkElement
        myEditedNetworkElement = nullptr;
        // restore previous edit mode
        if (myViewNet->myEditModes.networkEditMode != myPreviousNetworkEditMode) {
            myViewNet->myEditModes.setNetworkEditMode(myPreviousNetworkEditMode);
        }
    }
}


void
GNEViewNetHelper::EditNetworkElementShapes::commitEditedShape() {
    // save edited junction's shape
    if (myEditedNetworkElement != nullptr) {

        /* */

        // stop edit custom shape
        stopEditCustomShape();
    }
}


GNENetworkElement*
GNEViewNetHelper::EditNetworkElementShapes::getEditedNetworkElement() const {
    return myEditedNetworkElement;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::BlockIcon - methods
// ---------------------------------------------------------------------------

void
GNEViewNetHelper::LockIcon::drawLockIcon(const GNEAttributeCarrier* AC, GUIGlObjectType type,
        const Position viewPosition, const double exaggeration, const double size,
        const double offsetx, const double offsety) {
    // first check if icon can be drawn
    if (checkDrawing(AC, type, exaggeration)) {
        // Start pushing matrix
        GLHelper::pushMatrix();
        // Traslate to position
        glTranslated(viewPosition.x(), viewPosition.y(), GLO_LOCKICON);
        // Traslate depending of the offset
        glTranslated(offsetx, offsety, 0);
        // rotate to avoid draw invert
        glRotated(180, 0, 0, 1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Draw lock icon
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::LOCK), size);
        // Pop matrix
        GLHelper::popMatrix();
    }
}


GNEViewNetHelper::LockIcon::LockIcon() {}


bool
GNEViewNetHelper::LockIcon::checkDrawing(const GNEAttributeCarrier* AC, GUIGlObjectType type, const double exaggeration) {
    // get view net
    const auto viewNet = AC->getNet()->getViewNet();
    // get visualization settings
    const auto& s = viewNet->getVisualisationSettings();
    // check exaggeration
    if (exaggeration == 0) {
        return false;
    }
    // check supermodes
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() &&
            !(AC->getTagProperty().isNetworkElement() || AC->getTagProperty().isAdditionalElement())) {
        return false;
    }
    if (viewNet->getEditModes().isCurrentSupermodeDemand() && (!AC->getTagProperty().isDemandElement())) {
        return false;
    }
    if (viewNet->getEditModes().isCurrentSupermodeData() && (!AC->getTagProperty().isDataElement())) {
        return false;
    }
    // check if is locked
    if (!viewNet->getLockManager().isObjectLocked(type, AC->isAttributeCarrierSelected())) {
        return false;
    }
    // check visualizationSettings
    if (s.drawForPositionSelection || s.drawForRectangleSelection) {
        return false;
    }
    // check detail
    if (!s.drawDetail(s.detailSettings.lockIcon, exaggeration)) {
        return false;
    }
    // all ok, then draw
    return true;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper - methods
// ---------------------------------------------------------------------------

const std::vector<RGBColor>&
GNEViewNetHelper::getRainbowScaledColors() {
    // if is empty, fill it
    if (myRainbowScaledColors.empty()) {
        // fill scale colors (10)
        myRainbowScaledColors.push_back(RGBColor(232, 35,  0,   255));
        myRainbowScaledColors.push_back(RGBColor(255, 165, 0,   255));
        myRainbowScaledColors.push_back(RGBColor(255, 255, 0,   255));
        myRainbowScaledColors.push_back(RGBColor(28,  215, 0,   255));
        myRainbowScaledColors.push_back(RGBColor(0,   181, 100, 255));
        myRainbowScaledColors.push_back(RGBColor(0,   255, 191, 255));
        myRainbowScaledColors.push_back(RGBColor(178, 255, 255, 255));
        myRainbowScaledColors.push_back(RGBColor(0,   112, 184, 255));
        myRainbowScaledColors.push_back(RGBColor(56,  41,  131, 255));
        myRainbowScaledColors.push_back(RGBColor(127, 0,   255, 255));
    }
    return myRainbowScaledColors;
}


const RGBColor&
GNEViewNetHelper::getRainbowScaledColor(const double min, const double max, const double value) {
    // check extremes
    if (value <= min) {
        return getRainbowScaledColors().front();
    } else if (value >= max) {
        return getRainbowScaledColors().back();
    } else {
        // calculate value procent between [min, max]
        const double procent = ((value - min) * 100) / (max - min);
        // check if is valid
        if (procent <= 0) {
            return getRainbowScaledColors().front();
        } else if (procent >= 100) {
            return getRainbowScaledColors().back();
        } else {
            // return scaled color
            return getRainbowScaledColors().at((int)(procent / 10.0));
        }
    }
}


std::vector<GUIGlObject*>
GNEViewNetHelper::filterElementsByLayer(const std::vector<GUIGlObject*>& GLObjects) {
    std::vector<GUIGlObject*> filteredGLObjects;
    if (GLObjects.size() > 0) {
        const auto firstLayer = GLObjects.front()->getType();
        for (const auto& GLObject : GLObjects) {
            if ((GLO_RIDE <= firstLayer) && (firstLayer <= GLO_TRANSHIP) &&
                    (GLO_RIDE <= GLObject->getType()) && (GLObject->getType() <= GLO_TRANSHIP)) {
                filteredGLObjects.push_back(GLObject);
            } else if ((GLO_STOP <= firstLayer) && (firstLayer <= GLO_STOP_CONTAINER) &&
                       (GLO_STOP <= GLObject->getType()) && (GLObject->getType() <= GLO_STOP_CONTAINER)) {
                filteredGLObjects.push_back(GLObject);
            } else if ((GLO_VEHICLE <= firstLayer) && (firstLayer <= GLO_ROUTEFLOW) &&
                       (GLO_VEHICLE <= GLObject->getType()) && (GLObject->getType() <= GLO_ROUTEFLOW)) {
                filteredGLObjects.push_back(GLObject);
            } else if ((GLO_PERSON <= firstLayer) && (firstLayer <= GLO_PERSONFLOW) &&
                       (GLO_PERSON <= GLObject->getType()) && (GLObject->getType() <= GLO_PERSONFLOW)) {
                filteredGLObjects.push_back(GLObject);
            } else if ((GLO_CONTAINER <= firstLayer) && (firstLayer <= GLO_CONTAINERFLOW) &&
                       (GLO_CONTAINER <= GLObject->getType()) && (GLObject->getType() <= GLO_CONTAINERFLOW)) {
                filteredGLObjects.push_back(GLObject);
            } else if (GLObject->getType() == firstLayer) {
                filteredGLObjects.push_back(GLObject);
            }
        }
    }
    return filteredGLObjects;
}

/****************************************************************************/
