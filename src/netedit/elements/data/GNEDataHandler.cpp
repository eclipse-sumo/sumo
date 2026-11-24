/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_DataInterval.h>
#include <netedit/changes/GNEChange_DataSet.h>
#include <netedit/changes/GNEChange_GenericData.h>
#include <netedit/dialogs/basic/GNEOverwriteElement.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/elements/data/GNEEdgeRelData.h>
#include <netedit/elements/data/GNETAZRelData.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDataHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDataHandler::GNEDataHandler(GNENet* net, FileBucket* fileBucket, const bool allowUndoRedo) :
    DataHandler(fileBucket),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEDataHandler::~GNEDataHandler() {
    // update options based in current buckets
    myNet->getGNEApplicationWindow()->getFileBucketHandler()->updateOptions();
}


bool
GNEDataHandler::parse() {
    myLoadingFile = true;
    const bool result = DataHandler::parse();
    myLoadingFile = false;
    return result;
}


bool
GNEDataHandler::hasMergedData() const {
    return myMergedData;
}


bool
GNEDataHandler::buildDataSet(const std::string& id) {
    // first check if dataSet exist
    if (!checkValidAdditionalID(SUMO_TAG_DATASET, id)) {
        return false;
    } else if (!checkDuplicatedDataSet(id)) {
        return false;
    } else {
        GNEDataSet* dataSet = new GNEDataSet(id, myNet, myFileBucket);
        if (myAllowUndoRedo) {
            myNet->getUndoList()->begin(dataSet, TL("add data set"));
            myNet->getUndoList()->add(new GNEChange_DataSet(dataSet, true), true);
            myNet->getUndoList()->end();
        } else {
            // insert dataSet without allowing undo/redo
            myNet->getAttributeCarriers()->insertDataSet(dataSet);
            dataSet->incRef("buildDataSet");
        }
        return true;
    }
}


bool
GNEDataHandler::buildDataInterval(const CommonXMLStructure::SumoBaseObject* /* sumoBaseObject */,
                                  const std::string& dataSetID, const double begin, const double end) {
    // get dataSet
    GNEDataSet* dataSet = myNet->getAttributeCarriers()->retrieveDataSet(dataSetID, false);
    // first check if dataSet exist
    if (dataSet == nullptr) {
        // create dataset AND data interval
        dataSet = new GNEDataSet(dataSetID, myNet, myFileBucket);
        GNEDataInterval* dataInterval = new GNEDataInterval(dataSet, begin, end);
        if (myAllowUndoRedo) {
            myNet->getUndoList()->begin(dataInterval, TL("add data set and data interval"));
            myNet->getUndoList()->add(new GNEChange_DataSet(dataSet, true), true);
            myNet->getUndoList()->add(new GNEChange_DataInterval(dataInterval, true), true);
            myNet->getUndoList()->end();
        } else {
            // insert dataSet allowing undo/redo
            myNet->getAttributeCarriers()->insertDataSet(dataSet);
            dataSet->incRef("buildDataInterval");
            // insert dataInterval without allowing undo/redo
            dataSet->addDataIntervalChild(dataInterval);
            dataInterval->incRef("buildDataInterval");
        }
        return true;
    } else if (dataSet->retrieveInterval(begin, end) == nullptr) {
        if (dataSet->getDataIntervalChildren().count(begin) != 0) {
            return writeError(TLF("Data set '%' already defines an interval starting at %.", dataSetID, begin));
        }
        myMergedData |= dataSet->getFileBucket() != myFileBucket;
        GNEDataInterval* dataInterval = new GNEDataInterval(dataSet, begin, end);
        if (myAllowUndoRedo) {
            myNet->getUndoList()->begin(dataInterval, TL("add data interval"));
            myNet->getUndoList()->add(new GNEChange_DataInterval(dataInterval, true), true);
            myNet->getUndoList()->end();
        } else {
            // insert dataInterval without allowing undo/redo
            dataSet->addDataIntervalChild(dataInterval);
            dataInterval->incRef("buildDataInterval");
        }
        return true;
    } else {
        return false;
    }
}


bool
GNEDataHandler::buildEdgeData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID,
                              const Parameterised::Map& parameters) {
    // get dataSet
    GNEDataSet* dataSet = myNet->getAttributeCarriers()->retrieveDataSet(sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (dataSet) {
        // get interval
        GNEDataInterval* dataInterval = dataSet->retrieveInterval(
                                            sumoBaseObject->getParentSumoBaseObject()->getDoubleAttribute(SUMO_ATTR_BEGIN),
                                            sumoBaseObject->getParentSumoBaseObject()->getDoubleAttribute(SUMO_ATTR_END));
        if (dataInterval) {
            // get data
            GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
            if (edge == nullptr) {
                return writeErrorInvalidParent(GNE_TAG_EDGEREL_SINGLE, {SUMO_TAG_EDGE});
            } else if (auto* existing = dataInterval->findEdgeData(edge)) {
                if (!mayOverwrite(existing)) {
                    return false;
                }
                overwrite(existing, parameters);
                return true;
            } else {
                // create edge data
                myMergedData |= dataSet->getFileBucket() != myFileBucket;
                GNEGenericData* edgeData = new GNEEdgeData(dataInterval, edge, parameters);
                if (myAllowUndoRedo) {
                    myNet->getUndoList()->begin(edgeData, TL("add edge rel"));
                    myNet->getUndoList()->add(new GNEChange_GenericData(edgeData, true), true);
                    myNet->getUndoList()->end();
                } else {
                    dataInterval->addGenericDataChild(edgeData);
                    edge->addChildElement(edgeData);
                    edgeData->incRef("buildEdgeData");
                }
                return true;
            }
        } else {
            return writeErrorInvalidParent(GNE_TAG_EDGEREL_SINGLE, {SUMO_TAG_DATAINTERVAL});
        }
    } else {
        return writeErrorInvalidParent(GNE_TAG_EDGEREL_SINGLE, {SUMO_TAG_DATASET});
    }
}


bool
GNEDataHandler::buildEdgeRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID,
                                      const std::string& toEdgeID, const Parameterised::Map& parameters) {
    // get dataSet
    GNEDataSet* dataSet = myNet->getAttributeCarriers()->retrieveDataSet(sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (dataSet != nullptr) {
        // get interval
        GNEDataInterval* dataInterval = dataSet->retrieveInterval(
                                            sumoBaseObject->getParentSumoBaseObject()->getDoubleAttribute(SUMO_ATTR_BEGIN),
                                            sumoBaseObject->getParentSumoBaseObject()->getDoubleAttribute(SUMO_ATTR_END));
        if (dataInterval != nullptr) {
            // get data
            GNEEdge* const fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
            GNEEdge* const toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
            if (fromEdge == nullptr) {
                return writeErrorInvalidParent(SUMO_TAG_EDGEREL, {SUMO_TAG_EDGE}, fromEdgeID);
            } else if (toEdge == nullptr) {
                return writeErrorInvalidParent(SUMO_TAG_EDGEREL, {SUMO_TAG_EDGE}, toEdgeID);
            } else if (auto* existing = dataInterval->findEdgeRelationData(fromEdge, toEdge)) {
                if (!mayOverwrite(existing)) {
                    return false;
                }
                overwrite(existing, parameters);
                return true;
            } else {
                myMergedData |= dataSet->getFileBucket() != myFileBucket;
                GNEGenericData* edgeData = new GNEEdgeRelData(dataInterval, fromEdge, toEdge, parameters);
                if (myAllowUndoRedo) {
                    myNet->getUndoList()->begin(edgeData, TL("add edge rel"));
                    myNet->getUndoList()->add(new GNEChange_GenericData(edgeData, true), true);
                    myNet->getUndoList()->end();
                } else {
                    dataInterval->addGenericDataChild(edgeData);
                    fromEdge->addChildElement(edgeData);
                    toEdge->addChildElement(edgeData);
                    edgeData->incRef("buildEdgeRelationData");
                }
                return true;
            }
        } else {
            return writeErrorInvalidParent(SUMO_TAG_EDGEREL, {SUMO_TAG_DATAINTERVAL});
        }
    } else {
        return writeErrorInvalidParent(SUMO_TAG_EDGEREL, {SUMO_TAG_DATASET});
    }
}


bool
GNEDataHandler::buildTAZRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromTAZID,
                                     const std::string& toTAZID, const Parameterised::Map& parameters) {
    // get dataSet
    GNEDataSet* dataSet = myNet->getAttributeCarriers()->retrieveDataSet(sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (dataSet != nullptr) {
        // get interval
        GNEDataInterval* dataInterval = dataSet->retrieveInterval(
                                            sumoBaseObject->getParentSumoBaseObject()->getDoubleAttribute(SUMO_ATTR_BEGIN),
                                            sumoBaseObject->getParentSumoBaseObject()->getDoubleAttribute(SUMO_ATTR_END));
        if (dataInterval != nullptr) {
            // get from TAZs
            GNEAdditional* fromTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, fromTAZID, false);
            GNEAdditional* toTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, toTAZID, false);
            if (fromTAZ == nullptr) {
                return writeErrorInvalidParent(SUMO_TAG_TAZREL, {SUMO_TAG_TAZ}, fromTAZID);
            } else if (toTAZ == nullptr) {
                return writeErrorInvalidParent(SUMO_TAG_TAZREL, {SUMO_TAG_TAZ}, toTAZID);
            } else if (auto* existing = fromTAZ == toTAZ ? dataInterval->findTAZRelationData(fromTAZ)
                       : dataInterval->findTAZRelationData(fromTAZ, toTAZ)) {
                if (!mayOverwrite(existing)) {
                    return false;
                }
                overwrite(existing, parameters);
                return true;
            } else if (fromTAZ == toTAZ) {
                myMergedData |= dataSet->getFileBucket() != myFileBucket;
                GNEGenericData* edgeData = new GNETAZRelData(dataInterval, fromTAZ, parameters);
                if (myAllowUndoRedo) {
                    myNet->getUndoList()->begin(edgeData, TL("add TAZ rel"));
                    myNet->getUndoList()->add(new GNEChange_GenericData(edgeData, true), true);
                    myNet->getUndoList()->end();
                } else {
                    dataInterval->addGenericDataChild(edgeData);
                    fromTAZ->addChildElement(edgeData);
                    edgeData->incRef("buildTAZRelationData");
                }
                return true;
            } else {
                myMergedData |= dataSet->getFileBucket() != myFileBucket;
                GNEGenericData* edgeData = new GNETAZRelData(dataInterval, fromTAZ, toTAZ, parameters);
                if (myAllowUndoRedo) {
                    myNet->getUndoList()->begin(edgeData, TL("add TAZ rel"));
                    myNet->getUndoList()->add(new GNEChange_GenericData(edgeData, true), true);
                    myNet->getUndoList()->end();
                } else {
                    dataInterval->addGenericDataChild(edgeData);
                    fromTAZ->addChildElement(edgeData);
                    toTAZ->addChildElement(edgeData);
                    edgeData->incRef("buildTAZRelationData");
                }
                return true;
            }
        } else {
            return writeErrorInvalidParent(SUMO_TAG_TAZREL, {SUMO_TAG_DATAINTERVAL});
        }
    } else {
        return writeErrorInvalidParent(SUMO_TAG_TAZREL, {SUMO_TAG_DATASET});
    }
}


bool
GNEDataHandler::mayOverwrite(const GNEGenericData* existing) {
    const auto tag = existing->getTagProperty()->getTag();
    if (!myLoadingFile) {
        return writeError(TLF("There is already a % with ID '%'.", existing->getTagStr(), existing->getID()));
    }
    if (myOverwriteElements) {
        return true;
    }
    if (myRemainElements || !myAllowUndoRedo) {
        return writeWarningDuplicated(tag, existing->getID(), tag);
    }
    const GNEOverwriteElement dialog(this, existing);
    if (dialog.getResult() == GNEOverwriteElement::Result::ACCEPT) {
        return true;
    }
    if (dialog.getResult() == GNEOverwriteElement::Result::CANCEL) {
        return writeWarningDuplicated(tag, existing->getID(), tag);
    }
    return false;
}


namespace {

void
applyDataValues(GNEGenericData* data, const Parameterised& parameters) {
    data->setParameters(parameters);
    data->getDataIntervalParent()->getDataSetParent()->updateAttributeColors();
    data->getNet()->getViewNet()->getIntervalBar().markForUpdate();
    data->getNet()->getSavingStatus()->requireSaveDataElements();
}

class GNEChange_DataValues final : public GNEChange {
public:
    GNEChange_DataValues(GNEGenericData* data, const Parameterised::Map& parameters) :
        GNEChange(Supermode::DATA, true, data->isAttributeCarrierSelected()),
        myData(data),
        myBefore(data->getParametersMap()),
        myAfter(parameters) {
        myData->incRef("GNEChange_DataValues");
    }

    ~GNEChange_DataValues() override {
        myData->decRef("GNEChange_DataValues");
        if (myData->unreferenced()) {
            delete myData;
        }
    }

    void undo() override {
        applyDataValues(myData, myBefore);
    }

    void redo() override {
        applyDataValues(myData, myAfter);
    }

    std::string undoName() const override {
        return TL("Undo overwrite data values");
    }

    std::string redoName() const override {
        return TL("Redo overwrite data values");
    }

private:
    GNEGenericData* const myData;
    const Parameterised myBefore;
    const Parameterised myAfter;
};

} // namespace


void
GNEDataHandler::overwrite(GNEGenericData* existing, const Parameterised::Map& parameters) {
    if (existing->getParametersMap() == parameters) {
        return;
    }
    myMergedData |= existing->getFileBucket() != myFileBucket;
    if (myAllowUndoRedo) {
        myNet->getUndoList()->add(new GNEChange_DataValues(existing, parameters), true);
    } else {
        applyDataValues(existing, Parameterised(parameters));
    }
}


bool
GNEDataHandler::checkDuplicatedDataSet(const std::string& id) {
    // retrieve data set
    auto dataSet = myNet->getAttributeCarriers()->retrieveDataSet(id, false);
    // if demand exist, check if overwrite (delete)
    if (dataSet) {
        if (myOverwriteElements) {
            // delete data element (and all of their childrens)
            myNet->deleteDataSet(dataSet, myNet->getUndoList());
        } else if (myRemainElements) {
            // duplicated dataset
            return writeWarningDuplicated(SUMO_TAG_DATASET, id, SUMO_TAG_DATASET);
        } else {
            // open overwrite dialog
            GNEOverwriteElement overwriteElementDialog(this, dataSet);
            // continue depending of result
            if (overwriteElementDialog.getResult() == GNEOverwriteElement::Result::ACCEPT) {
                // delete data element (and all of their childrens)
                myNet->deleteDataSet(dataSet, myNet->getUndoList());
            } else if (overwriteElementDialog.getResult() == GNEOverwriteElement::Result::CANCEL) {
                // duplicated dataset
                return writeWarningDuplicated(SUMO_TAG_DATASET, id, SUMO_TAG_DATASET);
            } else {
                return false;
            }
        }
    }
    return true;
}

/****************************************************************************/
