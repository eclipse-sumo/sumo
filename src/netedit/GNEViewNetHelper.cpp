/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/data/GNEDataSet.h>
#include <netedit/elements/data/GNEEdgeRelData.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEInternalLane.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
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
    // set GUIGlObject in myGUIGlObjectLanes
    sortGUIGlObjects(GUIGlObjects);
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
            } else if (AC->getTagProperty().isTAZElement()) {
                // update TAZ elements
                updateTAZElements(myEdgeObjects, AC);
            } else if (AC->getTagProperty().isShape()) {
                // update shape elements
                updateShapeElements(myEdgeObjects, AC);
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
            } else if (AC->getTagProperty().isTAZElement()) {
                // update TAZ elements
                updateTAZElements(myLaneObjects, AC);
            } else if (AC->getTagProperty().isShape()) {
                // update shape elements
                updateShapeElements(myLaneObjects, AC);
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


void
GNEViewNetHelper::ObjectsUnderCursor::swapLane2Edge() {
    // enable flag
    mySwapLane2edge = true;
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


GNEShape*
GNEViewNetHelper::ObjectsUnderCursor::getShapeFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.shapes.size() > 0) {
            return myEdgeObjects.shapes.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.shapes.size() > 0) {
            return myLaneObjects.shapes.front();
        } else {
            return nullptr;
        }
    }
}


GNETAZElement*
GNEViewNetHelper::ObjectsUnderCursor::getTAZElementFront() const {
    if (mySwapLane2edge) {
        if (myEdgeObjects.TAZElements.size() > 0) {
            return myEdgeObjects.TAZElements.front();
        } else {
            return nullptr;
        }
    } else {
        if (myLaneObjects.TAZElements.size() > 0) {
            return myLaneObjects.TAZElements.front();
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


const std::vector<GNEAttributeCarrier*>&
GNEViewNetHelper::ObjectsUnderCursor::getClickedAttributeCarriers() const {
    if (mySwapLane2edge) {
        return myEdgeObjects.attributeCarriers;
    } else {
        return myLaneObjects.attributeCarriers;
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
    shapes.clear();
    TAZElements.clear();
    demandElements.clear();
    junctions.clear();
    edges.clear();
    lanes.clear();
    crossings.clear();
    connections.clear();
    internalLanes.clear();
    TAZs.clear();
    POIs.clear();
    polys.clear();
    genericDatas.clear();
    edgeDatas.clear();
    edgeRelDatas.clear();
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
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // cast additional element from attribute carrier
    if (AC == frontAC) {
        // insert at front
        container.additionals.insert(container.additionals.begin(), dynamic_cast<GNEAdditional*>(AC));
    } else {
        // insert at back
        container.additionals.push_back(dynamic_cast<GNEAdditional*>(AC));
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateTAZElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // cast TAZ element from attribute carrier
    if (AC == frontAC) {
        // insert at front
        container.TAZElements.insert(container.TAZElements.begin(), dynamic_cast<GNETAZElement*>(AC));
    } else {
        // insert at back
        container.TAZElements.push_back(dynamic_cast<GNETAZElement*>(AC));
    }
    // cast specific TAZ
    switch (AC->getGUIGlObject()->getType()) {
        case GLO_TAZ:
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.TAZs.insert(container.TAZs.begin(), dynamic_cast<GNETAZ*>(AC));
            } else {
                // insert at back
                container.TAZs.push_back(dynamic_cast<GNETAZ*>(AC));
            }
            break;
        default:
            break;
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateShapeElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // cast shape element from attribute carrier
    if (AC == frontAC) {
        // insert at front
        container.shapes.insert(container.shapes.begin(), dynamic_cast<GNEShape*>(AC));
    } else {
        // insert at back
        container.shapes.push_back(dynamic_cast<GNEShape*>(AC));
    }
    // cast specific shape
    switch (AC->getGUIGlObject()->getType()) {
        case GLO_POI:
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.POIs.insert(container.POIs.begin(), dynamic_cast<GNEPOI*>(AC));
            } else {
                // insert at back
                container.POIs.push_back(dynamic_cast<GNEPOI*>(AC));
            }
            break;
        case GLO_POLYGON:
            // check front element
            if (AC == frontAC) {
                // insert at front
                container.polys.insert(container.polys.begin(), dynamic_cast<GNEPoly*>(AC));
            } else {
                // insert at back
                container.polys.push_back(dynamic_cast<GNEPoly*>(AC));
            }
            break;
        default:
            break;
    }
}


void
GNEViewNetHelper::ObjectsUnderCursor::updateDemandElements(ObjectsContainer& container, GNEAttributeCarrier* AC) {
    // get front AC
    const GNEAttributeCarrier* frontAC = myViewNet->getFrontAttributeCarrier();
    // cast demand element from attribute carrier
    if (AC == frontAC) {
        // insert at front
        container.demandElements.insert(container.demandElements.begin(), dynamic_cast<GNEDemandElement*>(AC));
    } else {
        // insert at back
        container.demandElements.push_back(dynamic_cast<GNEDemandElement*>(AC));
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
    // iterate over atribute carriers
    for (const auto& attributeCarrrier : container.attributeCarriers) {
        // add GUIGlObject in GUIGlObjects container
        container.GUIGlObjects.push_back(attributeCarrrier->getGUIGlObject());
    }
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
        return calculateMoveOperationShape(myViewNet->myObjectsUnderCursor.getJunctionFront(),
                                           myViewNet->myObjectsUnderCursor.getJunctionFront()->getNBNode()->getShape(),
                                           myViewNet->getVisualisationSettings().neteditSizeSettings.junctionGeometryPointRadius);
    } else if (myViewNet->myObjectsUnderCursor.getCrossingFront() && (myViewNet->myObjectsUnderCursor.getCrossingFront() == editedElement)) {
        return calculateMoveOperationShape(myViewNet->myObjectsUnderCursor.getCrossingFront(),
                                           myViewNet->myObjectsUnderCursor.getCrossingFront()->getCrossingShape(),
                                           myViewNet->getVisualisationSettings().neteditSizeSettings.crossingGeometryPointRadius);
    } else if (myViewNet->myObjectsUnderCursor.getConnectionFront() && (myViewNet->myObjectsUnderCursor.getConnectionFront() == editedElement)) {
        return calculateMoveOperationShape(myViewNet->myObjectsUnderCursor.getConnectionFront(),
                                           myViewNet->myObjectsUnderCursor.getConnectionFront()->getConnectionShape(),
                                           myViewNet->getVisualisationSettings().neteditSizeSettings.connectionGeometryPointRadius);
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
        // calculate polygonShapeOffset
        const double polygonShapeOffset = myViewNet->myObjectsUnderCursor.getPolyFront()->getShape().nearest_offset_to_point2D(myViewNet->getPositionInformation(), false);
        // calculate distance to shape
        const double distanceToShape = myViewNet->myObjectsUnderCursor.getPolyFront()->getShape().distance2D(myViewNet->getPositionInformation());
        // get snap radius
        const double snap_radius = myViewNet->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
        // check if we clicked over shape
        if ((distanceToShape <= snap_radius) || myViewNet->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getPolyFront()->getMoveOperation(polygonShapeOffset);
            // continue if move operation is valid
            if (moveOperation) {
                myMoveOperations.push_back(moveOperation);
                return true;
            }
        }
        // shape operation value wasn't calculated, then return false
        return false;
    } else if (myViewNet->myObjectsUnderCursor.getPOIFront() && (frontAC == myViewNet->myObjectsUnderCursor.getPOIFront())) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getPOIFront()->getMoveOperation(0);
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getAdditionalFront() && (frontAC == myViewNet->myObjectsUnderCursor.getAdditionalFront())) {
        // get move operation
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getAdditionalFront()->getMoveOperation(0);
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        } else {
            return false;
        }
    } else if (myViewNet->myObjectsUnderCursor.getTAZFront() && (frontAC == myViewNet->myObjectsUnderCursor.getTAZFront())) {
        // calculate TAZShapeOffset
        const double TAZShapeOffset = myViewNet->myObjectsUnderCursor.getTAZFront()->getTAZElementShape().nearest_offset_to_point2D(myViewNet->getPositionInformation(), false);
        // calculate distance to TAZ
        const double distanceToShape = myViewNet->myObjectsUnderCursor.getTAZFront()->getTAZElementShape().distance2D(myViewNet->getPositionInformation());
        // get snap radius
        const double snap_radius = myViewNet->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
        // check if we clicked over TAZ
        if (distanceToShape <= snap_radius) {
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getTAZFront()->getMoveOperation(TAZShapeOffset);
            // continue if move operation is valid
            if (moveOperation) {
                myMoveOperations.push_back(moveOperation);
                return true;
            }
        }
        // TAZ operation value wasn't calculated, then return false
        return false;
    } else if (myViewNet->myObjectsUnderCursor.getJunctionFront() && (frontAC == myViewNet->myObjectsUnderCursor.getJunctionFront())) {
        if (myViewNet->myObjectsUnderCursor.getJunctionFront()->isShapeEdited()) {
            return false;
        } else {
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getJunctionFront()->getMoveOperation(0);
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
            double shapeOffset = -1;
            if (myViewNet->myObjectsUnderCursor.getEdgeFront()->clickedOverShapeStart(myViewNet->getPositionInformation())) {
                // move shape start
                shapeOffset = 0;
            } else if (myViewNet->myObjectsUnderCursor.getEdgeFront()->clickedOverShapeEnd(myViewNet->getPositionInformation())) {
                // move shape end
                shapeOffset = myViewNet->myObjectsUnderCursor.getEdgeFront()->getNBEdge()->getGeometry().length2D();
            } else {
                // calculate shape offset
                shapeOffset = myViewNet->myObjectsUnderCursor.getEdgeFront()->getNBEdge()->getGeometry().nearest_offset_to_point2D(myViewNet->getPositionInformation());
            }
            // get move operation
            GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getEdgeFront()->getMoveOperation(shapeOffset);
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
        GNEMoveOperation* moveOperation = myViewNet->myObjectsUnderCursor.getDemandElementFront()->getMoveOperation(0);
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


bool
GNEViewNetHelper::MoveSingleElementValues::calculateMoveOperationShape(GNEMoveElement* moveElement, const PositionVector& shape, const double radius) {
    // calculate junctionShapeOffset
    const double junctionShapeOffset = shape.nearest_offset_to_point2D(myViewNet->getPositionInformation(), false);
    // calculate distance to shape
    const double distanceToShape = shape.distance2D(myViewNet->getPositionInformation());
    // check if we clicked over shape
    if (distanceToShape <= radius) {
        // get move operation
        GNEMoveOperation* moveOperation = moveElement->getMoveOperation(junctionShapeOffset);
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
            return true;
        }
    }
    // shape operation value wasn't calculated, then return false
    return false;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::MoveMultipleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::MoveMultipleElementValues::MoveMultipleElementValues(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::MoveMultipleElementValues::beginMoveSelection() {
    // save clicked position (to calculate offset)
    myClickedPosition = myViewNet->getPositionInformation();
    // obtain Junctions and edges selected
    const auto movedJunctions = myViewNet->getNet()->retrieveJunctions(true);
    const auto movedEdges = myViewNet->getNet()->retrieveEdges(true);
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
        myViewNet->getUndoList()->p_begin("moving selection");
        // iterate over all operations
        for (const auto& moveOperation : myMoveOperations) {
            // commit move
            GNEMoveElement::commitMove(myViewNet, moveOperation, moveOffset, myViewNet->getUndoList());
            // don't forget delete move operation
            delete moveOperation;
        }
        // end undo list
        myViewNet->getUndoList()->p_end();
        // clear move operations
        myMoveOperations.clear();
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::finishMoveSelection() {
    // calculate moveOffset
    const GNEMoveOffset moveOffset = calculateMoveOffset();
    // begin undo list
    myViewNet->getUndoList()->p_begin("moving selection");
    // finish all move operations
    for (const auto& moveOperation : myMoveOperations) {
        GNEMoveElement::commitMove(myViewNet, moveOperation, moveOffset, myViewNet->getUndoList());
        // don't forget delete move operation
        delete moveOperation;
    }
    // end undo list
    myViewNet->getUndoList()->p_end();
    // clear move operations
    myMoveOperations.clear();
}


bool
GNEViewNetHelper::MoveMultipleElementValues::isMovingSelection() const {
    return (myMoveOperations.size() > 0);
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
    const auto selectedJunctions = myViewNet->getNet()->retrieveJunctions(true);
    // iterate over selected junctions
    for (const auto& junction : selectedJunctions) {
        moveOperation = junction->getMoveOperation(0);
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
    // now move all selected edges
    const auto selectedEdges = myViewNet->getNet()->retrieveEdges(true);
    // iterate over selected edges
    for (const auto& edge : selectedEdges) {
        moveOperation = edge->getMoveOperation(0);
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::calculateEdgeSelection(const GNEEdge* clickedEdge) {
    // declare move operation
    GNEMoveOperation* moveOperation = nullptr;
    // first move all selected junctions
    const auto selectedJunctions = myViewNet->getNet()->retrieveJunctions(true);
    // iterate over selected junctions
    for (const auto& junction : selectedJunctions) {
        moveOperation = junction->getMoveOperation(0);
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
    // obtain selected edges in two groups (depending of angle)
    const auto selectedEdges000180 = myViewNet->getNet()->retrieve000180AngleEdges(true);
    const auto selectedEdges180360 = myViewNet->getNet()->retrieve180360AngleEdges(true);
    // calculate shape offset for clicked edge
    const double shapeOffset = clickedEdge->getNBEdge()->getGeometry().nearest_offset_to_point2D(myViewNet->getPositionInformation());
    // get flag for inverse offset
    const bool useInverseOffset = (std::find(selectedEdges000180.begin(), selectedEdges000180.end(), clickedEdge) != selectedEdges000180.end());
    // iterate over edges betwen 0 and 180 degrees
    for (const auto& edge : selectedEdges000180) {
        // get move operation depending of useInverseOffset
        if (useInverseOffset) {
            moveOperation = edge->getMoveOperation(shapeOffset);
        } else {
            moveOperation = edge->getMoveOperation(edge->getNBEdge()->getGeometry().length2D() - shapeOffset);
        }
        // continue if move operation is valid
        if (moveOperation) {
            myMoveOperations.push_back(moveOperation);
        }
    }
    // iterate over edges betwen 180 and 360 degrees
    for (const auto& edge : selectedEdges180360) {
        // get move operation depending of useInverseOffset
        if (useInverseOffset) {
            moveOperation = edge->getMoveOperation(edge->getNBEdge()->getGeometry().length2D() - shapeOffset);
        } else {
            moveOperation = edge->getMoveOperation(shapeOffset);
        }
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
    myViewNet->setStatusBarText("Selection width:" + toString(fabs(selectionCorner1.x() - selectionCorner2.x()))
                                + " height:" + toString(fabs(selectionCorner1.y() - selectionCorner2.y()))
                                + " diagonal:" + toString(selectionCorner1.distanceTo2D(selectionCorner2)));
}


void
GNEViewNetHelper::SelectingArea::finishRectangleSelection() {
    // finish rectangle selection
    selectingUsingRectangle = false;
    startDrawing = false;
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
            for (auto i : ACsInBoundary) {
                if (i.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    result.push_back(dynamic_cast<GNEEdge*>(i.second));
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
                if (AC.second->getTagProperty().isNetworkElement() || AC.second->getTagProperty().isAdditionalElement() ||
                        AC.second->getTagProperty().isTAZElement() || AC.second->getTagProperty().isShape()) {
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
            std::vector<GNEAttributeCarrier*> selectedAC = myViewNet->getNet()->getSelectedAttributeCarriers(false);
            // add id into ACs to unselect
            for (auto i : selectedAC) {
                ACToUnselect.push_back(i);
            }
        }
        // iterate over AtributeCarriers obtained of boundary an place it in ACToSelect or ACToUnselect
        for (auto i : ACsInBoundaryFiltered) {
            switch (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode()) {
                case GNESelectorFrame::ModificationMode::Operation::SUB:
                    ACToUnselect.push_back(i.second);
                    break;
                case GNESelectorFrame::ModificationMode::Operation::RESTRICT:
                    if (std::find(ACToUnselect.begin(), ACToUnselect.end(), i.second) != ACToUnselect.end()) {
                        ACToSelect.push_back(i.second);
                    }
                    break;
                default:
                    ACToSelect.push_back(i.second);
                    break;
            }
        }
        // select junctions and their connections and crossings if Auto select junctions is enabled (note: only for "add mode")
        if (myViewNet->autoSelectNodes() && (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::Operation::ADD)) {
            std::vector<GNEEdge*> edgesToSelect;
            // iterate over ACToSelect and extract edges
            for (auto i : ACToSelect) {
                if (i->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    edgesToSelect.push_back(dynamic_cast<GNEEdge*>(i));
                }
            }
            // iterate over extracted edges
            for (auto i : edgesToSelect) {
                // select junction source and all their connections and crossings
                ACToSelect.push_back(i->getParentJunctions().front());
                for (auto j : i->getParentJunctions().front()->getGNEConnections()) {
                    ACToSelect.push_back(j);
                }
                for (auto j : i->getParentJunctions().front()->getGNECrossings()) {
                    ACToSelect.push_back(j);
                }
                // select junction destiny and all their connections crossings
                ACToSelect.push_back(i->getParentJunctions().back());
                for (auto j : i->getParentJunctions().back()->getGNEConnections()) {
                    ACToSelect.push_back(j);
                }
                for (auto j : i->getParentJunctions().back()->getGNECrossings()) {
                    ACToSelect.push_back(j);
                }
            }
        }
        // only continue if there is ACs to select or unselect
        if ((ACToSelect.size() + ACToUnselect.size()) > 0) {
            // first unselect AC of ACToUnselect and then selects AC of ACToSelect
            myViewNet->myUndoList->p_begin("selection using rectangle");
            for (auto i : ACToUnselect) {
                i->setAttribute(GNE_ATTR_SELECTED, "0", myViewNet->myUndoList);
            }
            for (auto i : ACToSelect) {
                if (i->getTagProperty().isSelectable()) {
                    i->setAttribute(GNE_ATTR_SELECTED, "1", myViewNet->myUndoList);
                }
            }
            myViewNet->myUndoList->p_end();
        }
        myViewNet->makeNonCurrent();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::TestingMode - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::TestingMode::TestingMode(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myTestingEnabled(OptionsCont::getOptions().getBool("gui-testing")),
    myTestingWidth(0),
    myTestingHeight(0) {
}


void
GNEViewNetHelper::TestingMode::initTestingMode() {
    // first check if testing mode is enabled and window size is correct
    if (myTestingEnabled && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        // make sure that given windows size has exactly two valid int values
        if ((windowSize.size() == 2) && GNEAttributeCarrier::canParse<int>(windowSize[0]) && GNEAttributeCarrier::canParse<int>(windowSize[1])) {
            myTestingWidth = GNEAttributeCarrier::parse<int>(windowSize[0]);
            myTestingHeight = GNEAttributeCarrier::parse<int>(windowSize[1]);
        } else {
            WRITE_ERROR("Invalid windows size-format: " + toString(windowSize) + "for option 'window-size'");
        }
    }
}


void
GNEViewNetHelper::TestingMode::drawTestingElements(GUIMainWindow* mainWindow) {
    // first check if testing mode is neabled
    if (myTestingEnabled) {
        // check if main windows has to be resized
        if (myTestingWidth > 0) {
            mainWindow->resize(myTestingWidth, myTestingHeight);
        }
        //std::cout << " fixed: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
        // draw pink square in the upper left corner on top of everything
        GLHelper::pushMatrix();
        const double size = myViewNet->p2m(32);
        Position center = myViewNet->screenPos2NetPos(8, 8);
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
        GLHelper::pushMatrix();
        // show box with the current position relative to pink square
        Position posRelative = myViewNet->screenPos2NetPos(myViewNet->getWidth() - 40, myViewNet->getHeight() - 20);
        // adjust cursor position (24,25) to show exactly the same position as in function netedit.leftClick(match, X, Y)
        GLHelper::drawTextBox(toString(myViewNet->getWindowCursorPosition().x() - 24) + " " + toString(myViewNet->getWindowCursorPosition().y() - 25), posRelative, GLO_TESTELEMENT, myViewNet->p2m(20), RGBColor::BLACK, RGBColor::WHITE);
        GLHelper::popMatrix();
    }
}


bool
GNEViewNetHelper::TestingMode::isTestingEnabled() const {
    return myTestingEnabled;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::SaveElements - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::SaveElements::SaveElements(GNEViewNet* viewNet) :
    saveAll(nullptr),
    saveNetwork(nullptr),
    saveAdditionalElements(nullptr),
    saveDemandElements(nullptr),
    saveDataElements(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::SaveElements::buildSaveElementsButtons() {
    // create save network button
    saveAll = new FXButton(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                           "\tSave all\tSave all elements.", GUIIconSubSys::getIcon(GUIIcon::SAVEALLELEMENTS),
                           myViewNet->getViewParent()->getGNEAppWindows(), MID_GNE_SAVEALLELEMENTS, GUIDesignButtonToolbar);
    saveAll->create();
    // create save network button
    saveNetwork = new FXButton(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                               "\tSave network\tSave network. (Ctrl+S)", GUIIconSubSys::getIcon(GUIIcon::SAVENETWORKELEMENTS),
                               myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK, GUIDesignButtonToolbar);
    saveNetwork->create();
    // create save additional elements button
    saveAdditionalElements = new FXButton(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                                          "\tSave additional elements\tSave additional elements. (Ctrl+Shift+A)", GUIIconSubSys::getIcon(GUIIcon::SAVEADDITIONALELEMENTS),
                                          myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS, GUIDesignButtonToolbar);
    saveAdditionalElements->create();
    // create save demand elements button
    saveDemandElements = new FXButton(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                                      "\tSave demand elements\tSave demand elements. (Ctrl+Shift+D)", GUIIconSubSys::getIcon(GUIIcon::SAVEDEMANDELEMENTS),
                                      myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS, GUIDesignButtonToolbar);
    saveDemandElements->create();
    // create save data elements button
    saveDataElements = new FXButton(myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements,
                                    "\tSave data elements\tSave data elements. (Ctrl+Shift+B)", GUIIconSubSys::getIcon(GUIIcon::SAVEDATAELEMENTS),
                                    myViewNet->getViewParent()->getGNEAppWindows(), MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS, GUIDesignButtonToolbar);
    saveDataElements->create();
    // recalc menu bar because there is new elements
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements->recalc();
    // show menu bar modes
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().saveElements->show();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::EditModes - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::EditModes::EditModes(GNEViewNet* viewNet) :
    networkEditMode(NetworkEditMode::NETWORK_INSPECT),
    demandEditMode(DemandEditMode::DEMAND_INSPECT),
    dataEditMode(DataEditMode::DATA_INSPECT),
    networkButton(nullptr),
    demandButton(nullptr),
    dataButton(nullptr),
    myViewNet(viewNet),
    myCurrentSupermode(Supermode::NETWORK) {
}


void
GNEViewNetHelper::EditModes::buildSuperModeButtons() {
    // create network button
    networkButton = new MFXCheckableButton(false,
                                           myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes, "Network\t\tSet mode for edit network elements. (F2)",
                                           GUIIconSubSys::getIcon(GUIIcon::SUPERMODENETWORK), myViewNet, MID_HOTKEY_F2_SUPERMODE_NETWORK, GUIDesignMFXCheckableButtonSupermode);
    networkButton->create();
    // create demand button
    demandButton = new MFXCheckableButton(false,
                                          myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes, "Demand\t\tSet mode for edit traffic demand. (F3)",
                                          GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), myViewNet, MID_HOTKEY_F3_SUPERMODE_DEMAND, GUIDesignMFXCheckableButtonSupermode);
    demandButton->create();
    // create data button
    dataButton = new MFXCheckableButton(false,
                                        myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes, "Data\t\tSet mode for edit data demand. (F4)",
                                        GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myViewNet, MID_HOTKEY_F4_SUPERMODE_DATA, GUIDesignMFXCheckableButtonSupermode);
    dataButton->create();
    // recalc menu bar because there is new elements
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
    // show menu bar modes
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::EditModes::setSupermode(Supermode supermode, const bool force) {
    if (!force && (supermode == myCurrentSupermode)) {
        myViewNet->setStatusBarText("Mode already selected");
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else {
        myViewNet->setStatusBarText("");
        // abort current operation
        myViewNet->abortOperation(false);
        // set super mode
        myCurrentSupermode = supermode;
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
        }
        // update buttons
        networkButton->update();
        demandButton->update();
        dataButton->update();
        // update Supermode CommandButtons in GNEAppWindows
        myViewNet->myViewParent->getGNEAppWindows()->updateSuperModeMenuCommands(myCurrentSupermode);
    }
}


void
GNEViewNetHelper::EditModes::setNetworkEditMode(NetworkEditMode mode, const bool force) {
    if ((mode == networkEditMode) && !force) {
        myViewNet->setStatusBarText("Network mode already selected");
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else if (networkEditMode == NetworkEditMode::NETWORK_TLS && !myViewNet->myViewParent->getTLSEditorFrame()->isTLSSaved()) {
        myViewNet->setStatusBarText("Save modifications in TLS before change mode");
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
        myViewNet->setStatusBarText("Demand mode already selected");
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
        // demand modes require ALWAYS a recomputing
        myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
        // update DijkstraRouter of RouteCalculatorInstance
        myViewNet->myNet->getPathManager()->getPathCalculator()->updatePathCalculator();
        // compute all demand elements
        myViewNet->myNet->computeDemandElements(myViewNet->myViewParent->getGNEAppWindows());
        // update cursors
        myViewNet->updateCursor();
        // update network mode specific controls
        myViewNet->updateDemandModeSpecificControls();
    }
}


void
GNEViewNetHelper::EditModes::setDataEditMode(DataEditMode mode, const bool force) {
    if ((mode == dataEditMode) && !force) {
        myViewNet->setStatusBarText("Data mode already selected");
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
        // data modes require ALWAYS a recomputing
        myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
        // update DijkstraRouter of RouteCalculatorInstance
        myViewNet->myNet->getPathManager()->getPathCalculator()->updatePathCalculator();
        // compute all data elements
        myViewNet->myNet->computeDataElements(myViewNet->myViewParent->getGNEAppWindows());
        // update all datasets
        for (const auto& dataSet : myViewNet->getNet()->getAttributeCarriers()->getDataSets()) {
            dataSet.second->updateAttributeColors();
        }
        // update cursors
        myViewNet->updateCursor();
        // update network mode specific controls
        myViewNet->updateDataModeSpecificControls();
    }
}


bool
GNEViewNetHelper::EditModes::isCurrentSupermodeNetwork() const {
    return (myCurrentSupermode == Supermode::NETWORK);
}


bool
GNEViewNetHelper::EditModes::isCurrentSupermodeDemand() const {
    return (myCurrentSupermode == Supermode::DEMAND);
}


bool
GNEViewNetHelper::EditModes::isCurrentSupermodeData() const {
    return (myCurrentSupermode == Supermode::DATA);
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::NetworkViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::NetworkViewOptions::NetworkViewOptions(GNEViewNet* viewNet) :
    menuCheckToggleGrid(nullptr),
    menuCheckDrawSpreadVehicles(nullptr),
    menuCheckShowDemandElements(nullptr),
    menuCheckSelectEdges(nullptr),
    menuCheckShowConnections(nullptr),
    menuCheckHideConnections(nullptr),
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
            ("\t\tShow grid and restrict movement to the grid - define grid size in visualization options. (Ctrl+G)"),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID, GUIDesignMFXCheckableButton);
    menuCheckToggleGrid->setChecked(false);
    menuCheckToggleGrid->create();

    menuCheckDrawSpreadVehicles = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tDraw vehicles spread in lane or in depart position."),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES, GUIDesignMFXCheckableButton);
    menuCheckDrawSpreadVehicles->setChecked(false);
    menuCheckDrawSpreadVehicles->create();

    menuCheckShowDemandElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show demand elements."),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS, GUIDesignMFXCheckableButton);
    menuCheckShowDemandElements->setChecked(false);
    menuCheckShowDemandElements->create();

    menuCheckSelectEdges = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle whether clicking should select edges or lanes."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES, GUIDesignMFXCheckableButton);
    menuCheckSelectEdges->setChecked(true);
    menuCheckSelectEdges->create();

    menuCheckShowConnections = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show connections over junctions."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS, GUIDesignMFXCheckableButton);
    menuCheckShowConnections->setChecked(myViewNet->getVisualisationSettings().showLane2Lane);
    menuCheckShowConnections->create();

    menuCheckHideConnections = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tHide connections."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_HIDECONNECTIONS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS, GUIDesignMFXCheckableButton);
    menuCheckHideConnections->setChecked(false);
    menuCheckHideConnections->create();

    menuCheckExtendSelection = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle whether selecting multiple edges should automatically select their junctions."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION, GUIDesignMFXCheckableButton);
    menuCheckExtendSelection->setChecked(true);
    menuCheckExtendSelection->create();

    menuCheckChangeAllPhases = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle whether clicking should apply state changes to all phases of the current TLS plan."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_APPLYTOALLPHASES),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES, GUIDesignMFXCheckableButton);
    menuCheckChangeAllPhases->setChecked(false);
    menuCheckChangeAllPhases->create();

    menuCheckWarnAboutMerge = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tAsk for confirmation before merging junction."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ASKFORMERGE),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE, GUIDesignMFXCheckableButton);
    menuCheckWarnAboutMerge->setChecked(true);
    menuCheckWarnAboutMerge->create();

    menuCheckShowJunctionBubble = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tShow bubbles over junctions shapes."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_BUBBLES),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES, GUIDesignMFXCheckableButton);
    menuCheckShowJunctionBubble->setChecked(false);
    menuCheckShowJunctionBubble->create();

    menuCheckMoveElevation = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tApply mouse movement to elevation instead of x,y position."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ELEVATION),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION, GUIDesignMFXCheckableButton);
    menuCheckMoveElevation->setChecked(false);
    menuCheckMoveElevation->create();

    menuCheckChainEdges = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tCreate consecutive edges with a single click (hit ESC to cancel chain)."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_CHAIN),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES, GUIDesignMFXCheckableButton);
    menuCheckChainEdges->setChecked(false);
    menuCheckChainEdges->create();

    menuCheckAutoOppositeEdge = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tAutomatically create an edge in the opposite direction."),
            GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_TWOWAY),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES, GUIDesignMFXCheckableButton);
    menuCheckAutoOppositeEdge->setChecked(false);
    menuCheckAutoOppositeEdge->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::NetworkViewOptions::hideNetworkViewOptionsMenuChecks() {
    menuCheckToggleGrid->hide();
    menuCheckDrawSpreadVehicles->hide();
    menuCheckShowDemandElements->hide();
    menuCheckSelectEdges->hide();
    menuCheckShowConnections->hide();
    menuCheckHideConnections->hide();
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
        return (menuCheckSelectEdges->amChecked() == TRUE);
    } else {
        // by default, if menuCheckSelectEdges isn't shown, always select edges
        return true;
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::showConnections() const {
    if (myViewNet->myEditModes.networkEditMode == NetworkEditMode::NETWORK_CONNECT) {
        // check if menu check hide connections ins shown
        return (menuCheckHideConnections->amChecked() == FALSE);
    } else if (myViewNet->myEditModes.networkEditMode == NetworkEditMode::NETWORK_PROHIBITION) {
        return true;
    } else if (menuCheckShowConnections->shown() == false) {
        return false;
    } else {
        return (myViewNet->getVisualisationSettings().showLane2Lane);
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
    menuCheckDrawSpreadVehicles(nullptr),
    menuCheckHideShapes(nullptr),
    menuCheckShowAllTrips(nullptr),
    menuCheckShowAllPersonPlans(nullptr),
    menuCheckLockPerson(nullptr),
    menuCheckShowAllContainerPlans(nullptr),
    menuCheckLockContainer(nullptr),
    menuCheckHideNonInspectedDemandElements(nullptr),
    myViewNet(viewNet),
    myLockedPerson(nullptr),
    myLockedContainer(nullptr) {
}


void
GNEViewNetHelper::DemandViewOptions::buildDemandViewOptionsMenuChecks() {
    // create menu checks
    menuCheckToggleGrid = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tShow grid and restrict movement to the grid - define grid size in visualization options. (Ctrl+G)"),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID, GUIDesignMFXCheckableButton);
    menuCheckToggleGrid->setChecked(false);
    menuCheckToggleGrid->create();

    menuCheckDrawSpreadVehicles = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tDraw vehicles spread in lane or in depart position."),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES, GUIDesignMFXCheckableButton);
    menuCheckDrawSpreadVehicles->setChecked(false);
    menuCheckDrawSpreadVehicles->create();

    menuCheckHideShapes = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show shapes (Polygons and POIs)."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDESHAPES),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES, GUIDesignMFXCheckableButton);
    menuCheckHideShapes->setChecked(false);
    menuCheckHideShapes->create();

    menuCheckShowAllTrips = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show all trips (requires updated demand - F5)."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWTRIPS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS, GUIDesignMFXCheckableButton);
    menuCheckShowAllTrips->setChecked(false);
    menuCheckShowAllTrips->create();

    menuCheckShowAllPersonPlans = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tShow all person plans."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWPERSONPLANS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS, GUIDesignMFXCheckableButton);
    menuCheckShowAllPersonPlans->setChecked(false);
    menuCheckShowAllPersonPlans->create();

    menuCheckLockPerson = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tLock selected person."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKPERSON),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON, GUIDesignMFXCheckableButton);
    menuCheckLockPerson->setChecked(false);
    menuCheckLockPerson->create();

    menuCheckShowAllContainerPlans = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tShow all container plans."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS, GUIDesignMFXCheckableButton);
    menuCheckShowAllContainerPlans->setChecked(false);
    menuCheckShowAllContainerPlans->create();

    menuCheckLockContainer = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tLock selected container."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKCONTAINER),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER, GUIDesignMFXCheckableButton);
    menuCheckLockContainer->setChecked(false);
    menuCheckLockContainer->create();

    menuCheckHideNonInspectedDemandElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show non-inspected demand elements."),
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED, GUIDesignMFXCheckableButton);
    menuCheckHideNonInspectedDemandElements->setChecked(false);
    menuCheckHideNonInspectedDemandElements->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DemandViewOptions::hideDemandViewOptionsMenuChecks() {
    menuCheckToggleGrid->hide();
    menuCheckDrawSpreadVehicles->hide();
    menuCheckHideShapes->hide();
    menuCheckShowAllTrips->hide();
    menuCheckShowAllPersonPlans->hide();
    menuCheckLockPerson->hide();
    menuCheckShowAllContainerPlans->hide();
    menuCheckLockContainer->hide();
    menuCheckHideNonInspectedDemandElements->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::DemandViewOptions::getVisibleDemandMenuCommands(std::vector<MFXCheckableButton*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckToggleGrid->shown()) {
        commands.push_back(menuCheckToggleGrid);
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


const GNEDemandElement*
GNEViewNetHelper::DemandViewOptions::getLockedContainer() const {
    return myLockedContainer;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DataViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DataViewOptions::DataViewOptions(GNEViewNet* viewNet) :
    menuCheckShowAdditionals(nullptr),
    menuCheckShowShapes(nullptr),
    menuCheckShowDemandElements(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::DataViewOptions::buildDataViewOptionsMenuChecks() {
    // create menu checks
    menuCheckShowAdditionals = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show additionals."),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWADDITIONALS),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS, GUIDesignMFXCheckableButton);
    menuCheckShowAdditionals->setChecked(false);
    menuCheckShowAdditionals->create();

    menuCheckShowShapes = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show shapes (Polygons and POIs)."),
            GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWSHAPES),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES, GUIDesignMFXCheckableButton);
    menuCheckShowShapes->setChecked(false);
    menuCheckShowShapes->create();

    menuCheckShowDemandElements = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            ("\t\tToggle show demand elements."),
            GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
            myViewNet, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS, GUIDesignMFXCheckableButton);
    menuCheckShowDemandElements->setChecked(false);
    menuCheckShowDemandElements->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DataViewOptions::hideDataViewOptionsMenuChecks() {
    menuCheckShowAdditionals->hide();
    menuCheckShowShapes->hide();
    menuCheckShowDemandElements->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::DataViewOptions::getVisibleDataMenuCommands(std::vector<MFXCheckableButton*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckShowAdditionals->shown()) {
        commands.push_back(menuCheckShowAdditionals);
    }
    if (menuCheckShowShapes->shown()) {
        commands.push_back(menuCheckShowShapes);
    }
    if (menuCheckShowDemandElements->shown()) {
        commands.push_back(menuCheckShowDemandElements);
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

// ---------------------------------------------------------------------------
// GNEViewNetHelper::IntervalBar - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::IntervalBar::IntervalBar(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myIntervalBarUpdate(true),
    myGenericDataTypesComboBox(nullptr),
    myDataSetsComboBox(nullptr),
    myLimitByIntervalCheckBox(nullptr),
    myBeginTextField(nullptr),
    myEndTextField(nullptr),
    myFilteredAttributesComboBox(nullptr),
    myNoGenericDatas("<no types>"),
    myAllGenericDatas("<all types>"),
    myNoDataSets("<no dataSets>"),
    myAllDataSets("<all dataSets>"),
    myAllAttributes("<all attributes>") {
}


void
GNEViewNetHelper::IntervalBar::buildIntervalBarElements() {
    // create interval label
    FXLabel* genericDataLabel = new FXLabel(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                            "Data type", 0, GUIDesignLabelAttribute);
    genericDataLabel->create();
    // create combo box for generic datas
    myGenericDataTypesComboBox = new FXComboBox(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
            GUIDesignComboBoxNCol, myViewNet, MID_GNE_INTERVALBAR_GENERICDATATYPE, GUIDesignComboBoxWidth120);
    myGenericDataTypesComboBox->create();
    FXLabel* dataSetLabel = new FXLabel(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                        "Data sets", 0, GUIDesignLabelAttribute);
    dataSetLabel->create();
    // create combo box for sets
    myDataSetsComboBox = new FXComboBox(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                        GUIDesignComboBoxNCol, myViewNet, MID_GNE_INTERVALBAR_DATASET, GUIDesignComboBoxWidth120);
    myDataSetsComboBox->create();
    // create checkbutton for myLimitByInterval
    myLimitByIntervalCheckBox = new FXCheckButton(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
            "Interval", myViewNet, MID_GNE_INTERVALBAR_LIMITED, GUIDesignCheckButtonAttribute);
    myLimitByIntervalCheckBox->create();
    // create textfield for begin
    myBeginTextField = new FXTextField(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                       GUIDesignTextFieldNCol, myViewNet, MID_GNE_INTERVALBAR_BEGIN, GUIDesignTextFielWidth50Real);
    myBeginTextField->create();
    // create text field for end
    myEndTextField = new FXTextField(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                     GUIDesignTextFieldNCol, myViewNet, MID_GNE_INTERVALBAR_END, GUIDesignTextFielWidth50Real);
    myEndTextField->create();
    // create attribute label
    FXLabel* attributeLabel = new FXLabel(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
                                          "Attribute", 0, GUIDesignLabelAttribute);
    attributeLabel->create();
    // create combo box for attributes
    myFilteredAttributesComboBox = new FXComboBox(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar,
            GUIDesignComboBoxNCol, myViewNet, MID_GNE_INTERVALBAR_ATTRIBUTE, GUIDesignComboBoxWidth180);
    myFilteredAttributesComboBox->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().intervalBar->recalc();
}


void
GNEViewNetHelper::IntervalBar::enableIntervalBar() {
    // enable elements
    myGenericDataTypesComboBox->enable();
    myDataSetsComboBox->enable();
    myLimitByIntervalCheckBox->enable();
    if (myLimitByIntervalCheckBox->getCheck() == TRUE) {
        myBeginTextField->enable();
        myEndTextField->enable();
    } else {
        myBeginTextField->disable();
        myEndTextField->disable();
    }
    myFilteredAttributesComboBox->enable();
}


void
GNEViewNetHelper::IntervalBar::disableIntervalBar() {
    // disable all elements
    myGenericDataTypesComboBox->disable();
    myDataSetsComboBox->disable();
    myLimitByIntervalCheckBox->disable();
    myBeginTextField->disable();
    myEndTextField->disable();
    myFilteredAttributesComboBox->disable();
}


void
GNEViewNetHelper::IntervalBar::enableIntervalBarUpdate() {
    myIntervalBarUpdate = true;
    // now update interval bar
    updateIntervalBar();
}


void
GNEViewNetHelper::IntervalBar::disableIntervalBarUpdate() {
    myIntervalBarUpdate = false;
}


void
GNEViewNetHelper::IntervalBar::showIntervalBar() {
    // check if begin and end textFields has to be updated (only once)
    if (myBeginTextField->getText().empty()) {
        setBegin();
    }
    if (myEndTextField->getText().empty()) {
        setEnd();
    }
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
    if (myIntervalBarUpdate) {
        // first save current data set
        const std::string previousDataSet = myDataSetsComboBox->getNumItems() > 0 ? myDataSetsComboBox->getItem(myDataSetsComboBox->getCurrentItem()).text() : "";
        // first clear items
        myDataSetsComboBox->clearItems();
        myGenericDataTypesComboBox->clearItems();
        if (myViewNet->getNet()) {
            // retrieve data sets
            const auto dataSets = myViewNet->getNet()->retrieveDataSets();
            if (dataSets.empty()) {
                myGenericDataTypesComboBox->appendItem(myNoGenericDatas);
                myDataSetsComboBox->appendItem(myNoDataSets);
                // disable elements
                disableIntervalBar();
            } else {
                // declare integer to save previous data set index
                int previousDataSetIndex = 0;
                // enable elements
                enableIntervalBar();
                // add "<all>" item
                myGenericDataTypesComboBox->appendItem(myAllGenericDatas);
                myDataSetsComboBox->appendItem(myAllDataSets);
                // get all generic data types
                const auto genericDataTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::GENERICDATA, false);
                // add all generic data types
                for (const auto& dataTag : genericDataTags) {
                    myGenericDataTypesComboBox->appendItem(dataTag.second.c_str());
                }
                myGenericDataTypesComboBox->setNumVisible(myGenericDataTypesComboBox->getNumItems());
                // add data sets
                for (const auto& dataSet : dataSets) {
                    // check if current data set is the previous data set
                    if (dataSet->getID() == previousDataSet) {
                        previousDataSetIndex = myDataSetsComboBox->getNumItems();
                    }
                    myDataSetsComboBox->appendItem(dataSet->getID().c_str());
                }
                // set visible elements
                if (myDataSetsComboBox->getNumItems() < 10) {
                    myDataSetsComboBox->setNumVisible(myDataSetsComboBox->getNumItems());
                } else {
                    myDataSetsComboBox->setNumVisible(10);
                }
                // set current data set
                myDataSetsComboBox->setCurrentItem(previousDataSetIndex);
            }
            // update limit by interval
            setInterval();
        }
    }
}


std::string
GNEViewNetHelper::IntervalBar::getGenericDataTypeStr() const {
    if (myGenericDataTypesComboBox->isEnabled() && (myGenericDataTypesComboBox->getText() == myAllGenericDatas)) {
        return "";
    } else {
        return myGenericDataTypesComboBox->getText().text();
    }
}


std::string
GNEViewNetHelper::IntervalBar::getDataSetStr() const {
    if (myDataSetsComboBox->isEnabled() && (myDataSetsComboBox->getText() == myAllDataSets)) {
        return "";
    } else {
        return myDataSetsComboBox->getText().text();
    }
}


std::string
GNEViewNetHelper::IntervalBar::getBeginStr() const {
    if (myBeginTextField->isEnabled() && GNEAttributeCarrier::canParse<double>(myBeginTextField->getText().text())) {
        return myBeginTextField->getText().text();
    } else {
        return "";
    }
}


std::string
GNEViewNetHelper::IntervalBar::getEndStr() const {
    if (myEndTextField->isEnabled() && GNEAttributeCarrier::canParse<double>(myEndTextField->getText().text())) {
        return myEndTextField->getText().text();
    } else {
        return "";
    }
}


std::string
GNEViewNetHelper::IntervalBar::getAttributeStr() const {
    if (myFilteredAttributesComboBox->isEnabled() &&
            ((myFilteredAttributesComboBox->getText() == myAllAttributes) || (myFilteredAttributesComboBox->getTextColor() != FXRGB(0, 0, 0)))) {
        return "";
    } else {
        return myFilteredAttributesComboBox->getText().text();
    }
}


void
GNEViewNetHelper::IntervalBar::setGenericDataType() {
    // check if data set is correct
    if (myGenericDataTypesComboBox->getText() == myAllGenericDatas) {
        myGenericDataTypesComboBox->setTextColor(FXRGB(0, 0, 0));
    } else if (myGenericDataTypesComboBox->getText().empty()) {
        myGenericDataTypesComboBox->setTextColor(FXRGB(0, 0, 0));
        myGenericDataTypesComboBox->setText(myAllGenericDatas);
    } else {
        // get all generic data types
        const auto genericDataTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::GENERICDATA, false);
        // set invalid color
        myGenericDataTypesComboBox->setTextColor(FXRGB(255, 0, 0));
        // set valid color depending of myGenericDataTypesComboBox
        for (const auto& genericDataTag : genericDataTags) {
            if (genericDataTag.second == myGenericDataTypesComboBox->getText().text()) {
                myGenericDataTypesComboBox->setTextColor(FXRGB(0, 0, 0));
            }
        }
    }
    // update comboBox attributes
    updateComboBoxAttributes();
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setDataSet() {
    // check if data set is correct
    if (myDataSetsComboBox->getText() == myAllDataSets) {
        myDataSetsComboBox->setTextColor(FXRGB(0, 0, 0));
    } else if (myDataSetsComboBox->getText().empty()) {
        myDataSetsComboBox->setTextColor(FXRGB(0, 0, 0));
        myDataSetsComboBox->setText(myAllDataSets);
    } else if (myViewNet->getNet()->retrieveDataSet(myDataSetsComboBox->getText().text(), false)) {
        myDataSetsComboBox->setTextColor(FXRGB(0, 0, 0));
    } else {
        myDataSetsComboBox->setTextColor(FXRGB(255, 0, 0));
    }
    // update comboBox attributes
    updateComboBoxAttributes();
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setInterval() {
    // enable or disable text fields
    if (myLimitByIntervalCheckBox->isEnabled() && (myLimitByIntervalCheckBox->getCheck() == TRUE)) {
        myBeginTextField->enable();
        myEndTextField->enable();
    } else {
        myBeginTextField->disable();
        myEndTextField->disable();
    }
    // update comboBox attributes
    updateComboBoxAttributes();
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
    // update comboBox attributes
    updateComboBoxAttributes();
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
    // update comboBox attributes
    updateComboBoxAttributes();
    // update view net
    myViewNet->updateViewNet();
}


void
GNEViewNetHelper::IntervalBar::setAttribute() {
    //
}


void
GNEViewNetHelper::IntervalBar::updateComboBoxAttributes() {
    // update attributes
    myFilteredAttributes = myViewNet->getNet()->retrieveGenericDataParameters(getDataSetStr(),
                           getGenericDataTypeStr(), getBeginStr(), getEndStr());
    // clear combo box
    myFilteredAttributesComboBox->clearItems();
    // check if there is dataSets
    if (myDataSetsComboBox->isEnabled()) {
        // add wildcard for all attributes
        myFilteredAttributesComboBox->appendItem(myAllAttributes);
        // add all atributes in ComboBox
        for (const auto& attribute : myFilteredAttributes) {
            myFilteredAttributesComboBox->appendItem(attribute.c_str());
        }
        // set visible elements
        if (myFilteredAttributesComboBox->getNumItems() < 10) {
            myFilteredAttributesComboBox->setNumVisible(myFilteredAttributesComboBox->getNumItems());
        } else {
            myFilteredAttributesComboBox->setNumVisible(10);
        }
    } else {
        // add wildcard for all attributes
        myFilteredAttributesComboBox->appendItem(myNoDataSets);
    }
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
                                           "\tset inspect mode\tMode for inspect elements and change their attributes. (I)",
                                           GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), myViewNet, MID_HOTKEY_I_MODES_INSPECT, GUIDesignMFXCheckableButton);
    inspectButton->create();
    // delete button
    deleteButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          "\tset delete mode\tMode for delete elements. (D)",
                                          GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), myViewNet, MID_HOTKEY_D_MODES_DELETE, GUIDesignMFXCheckableButton);
    deleteButton->create();
    // select button
    selectButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          "\tset select mode\tMode for select elements. (S)",
                                          GUIIconSubSys::getIcon(GUIIcon::MODESELECT), myViewNet, MID_HOTKEY_S_MODES_SELECT, GUIDesignMFXCheckableButton);
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
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::NetworkCheckableButtons::buildNetworkCheckableButtons() {
    // move button
    moveNetworkElementsButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tset move mode\tMode for move elements. (M)",
            GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myViewNet, MID_HOTKEY_M_MODES_MOVE, GUIDesignMFXCheckableButton);
    moveNetworkElementsButton->create();
    // create edge
    createEdgeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tset create edge mode\tMode for creating junction and edges. (E)",
            GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), myViewNet, MID_HOTKEY_E_MODES_EDGE_EDGEDATA, GUIDesignMFXCheckableButton);
    createEdgeButton->create();
    // connection mode
    connectionButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tset connection mode\tMode for edit connections between lanes. (C)",
            GUIIconSubSys::getIcon(GUIIcon::MODECONNECTION), myViewNet, MID_HOTKEY_C_MODES_CONNECT_PERSONPLAN, GUIDesignMFXCheckableButton);
    connectionButton->create();
    // prohibition mode
    prohibitionButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tset prohibition mode\tMode for editing connection prohibitions. (W)",
            GUIIconSubSys::getIcon(GUIIcon::MODEPROHIBITION), myViewNet, MID_HOTKEY_W_MODES_PROHIBITION_PERSONTYPE, GUIDesignMFXCheckableButton);
    prohibitionButton->create();
    // traffic light mode
    trafficLightButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tset traffic light mode\tMode for edit traffic lights over junctions. (T)",
            GUIIconSubSys::getIcon(GUIIcon::MODETLS), myViewNet, MID_HOTKEY_T_MODES_TLS_VTYPE, GUIDesignMFXCheckableButton);
    trafficLightButton->create();
    // additional mode
    additionalButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tset additional mode\tMode for adding additional elements. (A)",
            GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), myViewNet, MID_HOTKEY_A_MODES_ADDITIONAL_STOP, GUIDesignMFXCheckableButton);
    additionalButton->create();
    // crossing mode
    crossingButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                            "\tset crossing mode\tMode for creating crossings between edges. (R)",
                                            GUIIconSubSys::getIcon(GUIIcon::MODECROSSING), myViewNet, MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA, GUIDesignMFXCheckableButton);
    crossingButton->create();
    // TAZ Mode
    TAZButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                       "\tset TAZ mode\tMode for creating Traffic Assignment Zones. (Z)",
                                       GUIIconSubSys::getIcon(GUIIcon::MODETAZ), myViewNet, MID_HOTKEY_Z_MODES_TAZ_TAZREL, GUIDesignMFXCheckableButton);
    TAZButton->create();
    // shape mode
    shapeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                         "\tset polygon mode\tMode for creating polygons and POIs. (P)",
                                         GUIIconSubSys::getIcon(GUIIcon::MODEPOLYGON), myViewNet, MID_HOTKEY_P_MODES_POLYGON_PERSON, GUIDesignMFXCheckableButton);
    shapeButton->create();
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
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DemandCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DemandCheckableButtons::DemandCheckableButtons(GNEViewNet* viewNet) :
    moveDemandElementsButton(nullptr),
    routeButton(nullptr),
    vehicleButton(nullptr),
    vehicleTypeButton(nullptr),
    stopButton(nullptr),
    personTypeButton(nullptr),
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
            "\tset move mode\tMode for move elements. (M)",
            GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myViewNet, MID_HOTKEY_M_MODES_MOVE, GUIDesignMFXCheckableButton);
    moveDemandElementsButton->create();
    // route mode
    routeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                         "\tcreate route mode\tMode for creating routes. (R)",
                                         GUIIconSubSys::getIcon(GUIIcon::MODEROUTE), myViewNet, MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA, GUIDesignMFXCheckableButton);
    routeButton->create();
    // vehicle mode
    vehicleButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                           "\tcreate vehicle mode\tMode for creating vehicles. (V)",
                                           GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), myViewNet, MID_HOTKEY_V_MODES_VEHICLE, GUIDesignMFXCheckableButton);
    vehicleButton->create();
    // vehicle type mode
    vehicleTypeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tcreate vehicle type mode\tMode for creating vehicle types. (T)",
            GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLETYPE), myViewNet, MID_HOTKEY_T_MODES_TLS_VTYPE, GUIDesignMFXCheckableButton);
    vehicleTypeButton->create();
    // stop mode
    stopButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                        "\tcreate stop mode\tMode for creating stops. (A)",
                                        GUIIconSubSys::getIcon(GUIIcon::MODESTOP), myViewNet, MID_HOTKEY_A_MODES_ADDITIONAL_STOP, GUIDesignMFXCheckableButton);
    stopButton->create();
    // person type mode
    personTypeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tcreate person type mode\tMode for creating person types. (W)",
            GUIIconSubSys::getIcon(GUIIcon::MODEPERSONTYPE), myViewNet, MID_HOTKEY_W_MODES_PROHIBITION_PERSONTYPE, GUIDesignMFXCheckableButton);
    personTypeButton->create();
    // person mode
    personButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          "\tcreate person mode\tMode for creating persons. (P)",
                                          GUIIconSubSys::getIcon(GUIIcon::MODEPERSON), myViewNet, MID_HOTKEY_P_MODES_POLYGON_PERSON, GUIDesignMFXCheckableButton);
    personButton->create();
    // person plan mode
    personPlanButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tcreate person plan mode\tMode for creating person plans. (C)",
            GUIIconSubSys::getIcon(GUIIcon::MODEPERSONPLAN), myViewNet, MID_HOTKEY_C_MODES_CONNECT_PERSONPLAN, GUIDesignMFXCheckableButton);
    personPlanButton->create();
    // container mode
    containerButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                          "\tcreate container mode\tMode for creating containers. (P)",
                                          GUIIconSubSys::getIcon(GUIIcon::MODECONTAINER), myViewNet, MID_HOTKEY_G_MODE_CONTAINER, GUIDesignMFXCheckableButton);
    containerButton->create();
    // container plan mode
    containerPlanButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tcreate container plan mode\tMode for creating container plans. (C)",
            GUIIconSubSys::getIcon(GUIIcon::MODECONTAINERPLAN), myViewNet, MID_HOTKEY_H_MODE_CONTAINERDATA, GUIDesignMFXCheckableButton);
    containerPlanButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DemandCheckableButtons::showDemandCheckableButtons() {
    moveDemandElementsButton->show();
    routeButton->show();
    vehicleButton->show();
    vehicleTypeButton->show();
    stopButton->show();
    personTypeButton->show();
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
    vehicleTypeButton->hide();
    stopButton->hide();
    personTypeButton->hide();
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
    vehicleTypeButton->setChecked(false);
    stopButton->setChecked(false);
    personTypeButton->setChecked(false);
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
    vehicleTypeButton->update();
    stopButton->update();
    personTypeButton->update();
    personButton->update();
    personPlanButton->update();
    containerButton->update();
    containerPlanButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DataCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DataCheckableButtons::DataCheckableButtons(GNEViewNet* viewNet) :
    edgeDataButton(nullptr),
    edgeRelDataButton(nullptr),
    TAZRelDataButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::DataCheckableButtons::buildDataCheckableButtons() {
    // edgeData mode
    edgeDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
                                            "\tcreate edge data mode\tMode for creating edge datas. (E)",
                                            GUIIconSubSys::getIcon(GUIIcon::MODEEDGEDATA), myViewNet, MID_HOTKEY_E_MODES_EDGE_EDGEDATA, GUIDesignMFXCheckableButton);
    edgeDataButton->create();
    // edgeRelData mode
    edgeRelDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tcreate edge relation data mode\tMode for creating edge relation datas. (R)",
            GUIIconSubSys::getIcon(GUIIcon::MODEEDGERELDATA), myViewNet, MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA, GUIDesignMFXCheckableButton);
    edgeRelDataButton->create();
    // TAZRelData mode
    TAZRelDataButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes,
            "\tcreate TAZ relation data mode\tMode for creating TAZ relation datas. (Z)",
            GUIIconSubSys::getIcon(GUIIcon::MODETAZRELDATA), myViewNet, MID_HOTKEY_Z_MODES_TAZ_TAZREL, GUIDesignMFXCheckableButton);

    TAZRelDataButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DataCheckableButtons::showDataCheckableButtons() {
    edgeDataButton->show();
    edgeRelDataButton->show();
    TAZRelDataButton->show();
}


void
GNEViewNetHelper::DataCheckableButtons::hideDataCheckableButtons() {
    edgeDataButton->hide();
    edgeRelDataButton->hide();
    TAZRelDataButton->hide();
}


void
GNEViewNetHelper::DataCheckableButtons::disableDataCheckableButtons() {
    edgeDataButton->setChecked(false);
    edgeRelDataButton->setChecked(false);
    TAZRelDataButton->setChecked(false);
}


void
GNEViewNetHelper::DataCheckableButtons::updateDataCheckableButtons() {
    edgeDataButton->update();
    edgeRelDataButton->update();
    TAZRelDataButton->update();
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
GNEViewNetHelper::LockIcon::drawLockIcon(const GNEAttributeCarrier* AC, const GNEGeometry::Geometry& geometry,
        const double exaggeration, const double offsetx, const double offsety, const bool overlane, const double size) {
    // first check if icon can be drawn
    if (checkDrawing(AC, exaggeration) && (geometry.getShape().size() > 0)) {
        // calculate middle point
        const double middlePoint = (geometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = (geometry.getShape().size() == 1) ? geometry.getShape().front() : geometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        double rot = 0;
        if ((geometry.getShape().size() == 1) && (geometry.getShapeRotations().size() > 0)) {
            rot = geometry.getShapeRotations().front();
        } else if (geometry.getShape().size() > 1) {
            rot = geometry.getShape().rotationDegreeAtOffset(middlePoint);
        }
        // get texture
        const GUIGlID lockTexture = getLockIcon(AC);
        // Start pushing matrix
        GLHelper::pushMatrix();
        // Traslate to position
        glTranslated(pos.x(), pos.y(), 0.1);
        // rotate depending of overlane
        if (overlane) {
            GNEGeometry::rotateOverLane(rot);
        } else {
            // avoid draw invert
            glRotated(180, 0, 0, 1);
        }
        // Set draw color
        glColor3d(1, 1, 1);
        // Traslate depending of the offset
        glTranslated(offsetx, offsety, 0);
        // Draw lock icon
        GUITexturesHelper::drawTexturedBox(lockTexture, size);
        // Pop matrix
        GLHelper::popMatrix();
    }
}


GNEViewNetHelper::LockIcon::LockIcon() {}


bool
GNEViewNetHelper::LockIcon::checkDrawing(const GNEAttributeCarrier* AC, const double exaggeration) {
    // get visualization settings
    const auto s = AC->getNet()->getViewNet()->getVisualisationSettings();
    // check exaggeration
    if (exaggeration == 0) {
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
    // check modes
    if (!AC->getNet()->getViewNet()->showLockIcon()) {
        return false;
    }
    return true;
}


GUIGlID
GNEViewNetHelper::LockIcon::getLockIcon(const GNEAttributeCarrier* AC) {
    // Draw icon depending of the state of additional
    if (AC->drawUsingSelectColor()) {
        if (!AC->getTagProperty().canBlockMovement()) {
            // Draw not movable texture if additional isn't movable and is selected
            return GUITextureSubSys::getTexture(GUITexture::NOTMOVING_SELECTED);
        } else if (AC->getAttribute(GNE_ATTR_BLOCK_MOVEMENT) == toString(true)) {
            // Draw lock texture if additional is movable, is blocked and is selected
            return GUITextureSubSys::getTexture(GUITexture::LOCK_SELECTED);
        } else {
            // Draw empty texture if additional is movable, isn't blocked and is selected
            return GUITextureSubSys::getTexture(GUITexture::EMPTY_SELECTED);
        }
    } else {
        if (!AC->getTagProperty().canBlockMovement()) {
            // Draw not movable texture if additional isn't movable
            return GUITextureSubSys::getTexture(GUITexture::NOTMOVING);
        } else if (AC->getAttribute(GNE_ATTR_BLOCK_MOVEMENT) == toString(true)) {
            // Draw lock texture if additional is movable and is blocked
            return GUITextureSubSys::getTexture(GUITexture::LOCK);
        } else {
            // Draw empty texture if additional is movable and isn't blocked
            return GUITextureSubSys::getTexture(GUITexture::EMPTY);
        }
    }
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

/****************************************************************************/
