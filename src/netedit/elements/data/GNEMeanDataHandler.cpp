/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEMeanDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 22
///
// Builds meanData objects for netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_MeanData.h>
#include <netedit/dialogs/basic/GNEOverwriteElement.h>
#include <netedit/elements/data/GNEMeanData.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEMeanDataHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMeanDataHandler::GNEMeanDataHandler(GNENet* net, const std::string& filename, const bool allowUndoRedo) :
    MeanDataHandler(filename),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEMeanDataHandler::~GNEMeanDataHandler() {}


bool
GNEMeanDataHandler::postParserTasks() {
    // nothing to do
    return true;
}


bool
GNEMeanDataHandler::buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id,
                                      const std::string& file, const std::string& type, const SUMOTime period, const SUMOTime begin,
                                      const SUMOTime end, const bool trackVehicles, const std::vector<std::string>& writtenAttributes,
                                      const bool aggregate, const std::vector<std::string>& edgeIDs, const std::string& edgeFile,
                                      const std::string& excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                                      const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes,
                                      const double speedThreshold) {
    // parse attributes
    const auto edges = parseEdges(SUMO_TAG_MEANDATA_EDGE, edgeIDs);
    // parse edges
    const auto attributes = parseAttributes(SUMO_TAG_MEANDATA_EDGE, writtenAttributes);
    // check if meanData edge exists
    if (!checkValidAdditionalID(SUMO_TAG_MEANDATA_EDGE, id)) {
        return false;
    } else if (!checkDuplicatedMeanDataElement(SUMO_TAG_MEANDATA_EDGE, id)) {
        return false;
    } else if ((period != TIME2STEPS(-1)) && !checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_PERIOD, period, true)) {
        return false;
    } else if ((begin != TIME2STEPS(-1)) && !checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_BEGIN, begin, true)) {
        return false;
    } else if ((end != TIME2STEPS(-1)) && !checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_END, end, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_MAX_TRAVELTIME, maxTravelTime, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_MIN_SAMPLES, minSamples, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold, true)) {
        return false;
    } else if (!checkExcludeEmpty(SUMO_TAG_MEANDATA_EDGE, id, excludeEmpty)) {
        return false;
    } else if ((edges.size() == edgeIDs.size()) && (attributes.size() == writtenAttributes.size())) {
        GNEMeanData* edgeMeanData = new GNEMeanData(SUMO_TAG_MEANDATA_EDGE, id, myNet, myFilename, file, type, period, begin, end,
                trackVehicles, attributes,  aggregate, edgeIDs, edgeFile, excludeEmpty,  withInternal,
                detectPersons, minSamples, maxTravelTime, vTypes, speedThreshold);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(edgeMeanData, TL("add meanDataEdge"));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_MeanData(edgeMeanData, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertMeanData(edgeMeanData);
            edgeMeanData->incRef("buildEdgeMeanData");
        }
        return true;
    } else {
        return false;
    }
}


bool
GNEMeanDataHandler::buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id,
                                      const std::string& file, const std::string& type, const SUMOTime period, const SUMOTime begin,
                                      const SUMOTime end, const bool trackVehicles, const std::vector<std::string>& writtenAttributes,
                                      const bool aggregate, const std::vector<std::string>& edgeIDs, const std::string& edgeFile,
                                      const std::string& excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                                      const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes,
                                      const double speedThreshold) {
    // parse attributes
    const auto edges = parseEdges(SUMO_TAG_MEANDATA_LANE, edgeIDs);
    // parse edges
    const auto attributes = parseAttributes(SUMO_TAG_MEANDATA_LANE, writtenAttributes);
    // check if meanData edge exists
    if (!checkValidAdditionalID(SUMO_TAG_MEANDATA_LANE, id)) {
        return false;
    } else if (!checkDuplicatedMeanDataElement(SUMO_TAG_MEANDATA_LANE, id)) {
        return false;
    } else if ((period != TIME2STEPS(-1)) && !checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_PERIOD, period, true)) {
        return false;
    } else if ((begin != TIME2STEPS(-1)) && !checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_BEGIN, begin, true)) {
        return false;
    } else if ((end != TIME2STEPS(-1)) && !checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_END, end, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_MAX_TRAVELTIME, maxTravelTime, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_MIN_SAMPLES, minSamples, true)) {
        return false;
    } else if (!checkNegative(SUMO_TAG_MEANDATA_EDGE, id, SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold, true)) {
        return false;
    } else if (!checkExcludeEmpty(SUMO_TAG_MEANDATA_EDGE, id, excludeEmpty)) {
        return false;
    } else if ((edges.size() == edgeIDs.size()) && (attributes.size() == writtenAttributes.size())) {
        GNEMeanData* edgeMeanData = new GNEMeanData(SUMO_TAG_MEANDATA_LANE, id, myNet, myFilename, file, type, period, begin, end,
                trackVehicles, attributes,  aggregate, edgeIDs, edgeFile, excludeEmpty,  withInternal,
                detectPersons, minSamples, maxTravelTime, vTypes, speedThreshold);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(edgeMeanData, TL("add meanDataLane"));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_MeanData(edgeMeanData, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertMeanData(edgeMeanData);
            edgeMeanData->incRef("buildEdgeMeanData");
        }
        return true;
    } else {
        return false;
    }
}


std::vector<GNEEdge*>
GNEMeanDataHandler::parseEdges(const SumoXMLTag tag, const std::vector<std::string>& edgeIDs) {
    std::vector<GNEEdge*> edges;
    for (const auto& edgeID : edgeIDs) {
        GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
        // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
        if (edge == nullptr) {
            writeError(TLF("Could not build % in netedit", toString(tag)) + std::string("; ") + TL("Edge doesn't exist."));
            edges.clear();
            return edges;
        } else {
            edges.push_back(edge);
        }
    }
    return edges;
}


std::vector<SumoXMLAttr>
GNEMeanDataHandler::parseAttributes(const SumoXMLTag tag, const std::vector<std::string>& attrStrs) {
    std::vector<SumoXMLAttr> attrs;
    for (const auto& attrStr : attrStrs) {
        if (SUMOXMLDefinitions::Attrs.hasString(attrStr)) {
            attrs.push_back(static_cast<SumoXMLAttr>(SUMOXMLDefinitions::Attrs.get(attrStr)));
        } else {
            writeError(TLF("Could not build % in netedit", toString(tag)) + std::string("; ") + TLF("Attribute '%' doesn't exist.", attrStr));
            attrs.clear();
            return attrs;
        }
    }
    return attrs;
}


bool
GNEMeanDataHandler::checkDuplicatedMeanDataElement(const SumoXMLTag tag, const std::string& id) {
    // retrieve meanData element
    auto meanDataElement = myNet->getAttributeCarriers()->retrieveMeanData(tag, id, false);
    // if meanData exist, check if overwrite (delete)
    if (meanDataElement) {
        if (myOverwriteElements) {
            // delete meanData element (and all of their childrens)
            myNet->deleteMeanData(meanDataElement, myNet->getViewNet()->getUndoList());
        } else if (myRemainElements) {
            // duplicated dataset
            return writeWarningDuplicated(tag, meanDataElement->getID(), meanDataElement->getTagProperty()->getTag());
        } else {
            // open overwrite dialog
            GNEOverwriteElement overwriteElementDialog(this, meanDataElement);
            // continue depending of result
            if (overwriteElementDialog.getResult() == GNEOverwriteElement::Result::ACCEPT) {
                // delete meanData element (and all of their childrens)
                myNet->deleteMeanData(meanDataElement, myNet->getViewNet()->getUndoList());
            } else if (overwriteElementDialog.getResult() == GNEOverwriteElement::Result::CANCEL) {
                // duplicated demand
                return writeWarningDuplicated(tag, meanDataElement->getID(), meanDataElement->getTagProperty()->getTag());
            } else {
                return false;
            }
        }
    }
    return true;
}


bool
GNEMeanDataHandler::checkExcludeEmpty(const SumoXMLTag tag, const std::string& id, const std::string& excludeEmpty) {
    if (GNEAttributeCarrier::canParse<bool>(excludeEmpty)) {
        return true;
    } else if (excludeEmpty == SUMOXMLDefinitions::ExcludeEmptys.getString(ExcludeEmpty::DEFAULTS)) {
        return true;
    } else {
        return writeError(TLF("Could not build % with ID '%' in netedit; Invalid value '%' for %.", toString(tag), id, excludeEmpty, toString(SUMO_ATTR_EXCLUDE_EMPTY)));
    }
}

/****************************************************************************/
