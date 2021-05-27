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
/// @file    AdditionalHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2021
///
// Structure for common XML Parsing
/****************************************************************************/
#include <config.h>

#include <utils/xml/SUMOSAXHandler.h>

#include "CommonXMLStructure.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// CommonXMLStructure::SumoBaseObject - methods
// ---------------------------------------------------------------------------

CommonXMLStructure::SumoBaseObject::SumoBaseObject(SumoBaseObject* parent, const SumoXMLTag tag) :
    mySumoBaseObjectParent(parent),
    myTag(tag) {
    // add this SumoBaseObject into parent children
    if (parent) {
        parent->addSumoBaseObjectChild(this);
    }
}


CommonXMLStructure::SumoBaseObject::~SumoBaseObject() {
    // delete all SumoBaseObjectChildrens
    while(mySumoBaseObjectChildren.size() > 0) {
        delete mySumoBaseObjectChildren.back();
        mySumoBaseObjectChildren.pop_back();
    }
}


const SumoXMLTag 
CommonXMLStructure::SumoBaseObject::getTag() const {
    return myTag;
}


CommonXMLStructure::SumoBaseObject* 
CommonXMLStructure::SumoBaseObject::getParentSumoBaseObject() const {
    return mySumoBaseObjectParent;
}



const std::string&
CommonXMLStructure::SumoBaseObject::getStringAttribute(const SumoXMLAttr attr) const {
    if (hasStringAttribute(attr)) {
        return myStringAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


int
CommonXMLStructure::SumoBaseObject::getIntAttribute(const SumoXMLAttr attr) const {
    if (hasIntAttribute(attr)) {
        return myIntAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


double
CommonXMLStructure::SumoBaseObject::getDoubleAttribute(const SumoXMLAttr attr) const {
    if (hasDoubleAttribute(attr)) {
        return myDoubleAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


bool
CommonXMLStructure::SumoBaseObject::getBoolAttribute(const SumoXMLAttr attr) const {
    if (hasBoolAttribute(attr)) {
        return myBoolAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


const Position&
CommonXMLStructure::SumoBaseObject::getPositionAttribute(const SumoXMLAttr attr) const {
    if (hasPositionAttribute(attr)) {
        return myPositionAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


SUMOTime
CommonXMLStructure::SumoBaseObject::getTimeAttribute(const SumoXMLAttr attr) const {
    if (hasTimeAttribute(attr)) {
        return myTimeAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


const RGBColor &
CommonXMLStructure::SumoBaseObject::getColorAttribute(const SumoXMLAttr attr) const {
    if (hasColorAttribute(attr)) {
        return myColorAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


const std::vector<std::string>&
CommonXMLStructure::SumoBaseObject::getStringListAttribute(const SumoXMLAttr attr) const {
    if (hasStringListAttribute(attr)) {
        return myStringListAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


const PositionVector&
CommonXMLStructure::SumoBaseObject::getPositionVectorAttribute(const SumoXMLAttr attr) const {
    if (hasPositionVectorAttribute(attr)) {
        return myPositionVectorAttributes.at(attr);
    } else {
        throw ProcessError("Attr doesn't exist");
    }
}


const std::map<std::string, std::string>&
CommonXMLStructure::SumoBaseObject::getParameters() const {
    return myParameters;
}


const std::vector<CommonXMLStructure::SumoBaseObject*>&
CommonXMLStructure::SumoBaseObject::getSumoBaseObjectChildren() const {
    return mySumoBaseObjectChildren;
}


bool
CommonXMLStructure::SumoBaseObject::hasStringAttribute(const SumoXMLAttr attr) const {
    return myStringAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasIntAttribute(const SumoXMLAttr attr) const {
    return myIntAttributes.count(attr) > 0;
}


bool 
CommonXMLStructure::SumoBaseObject::hasDoubleAttribute(const SumoXMLAttr attr) const {
    return myDoubleAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasBoolAttribute(const SumoXMLAttr attr) const {
    return myBoolAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasPositionAttribute(const SumoXMLAttr attr) const {
    return myPositionAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasTimeAttribute(const SumoXMLAttr attr) const {
    return myTimeAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasColorAttribute(const SumoXMLAttr attr) const {
    return myColorAttributes.count(attr) > 0;
}


bool 
CommonXMLStructure::SumoBaseObject::hasStringListAttribute(const SumoXMLAttr attr) const {
    return myStringListAttributes.count(attr) > 0;
}


bool 
CommonXMLStructure::SumoBaseObject::hasPositionVectorAttribute(const SumoXMLAttr attr) const {
    return myPositionVectorAttributes.count(attr) > 0;
}


void 
CommonXMLStructure::SumoBaseObject::addStringAttribute(const SumoXMLAttr attr, const std::string &value) {
    myStringAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addIntAttribute(const SumoXMLAttr attr, const int value) {
    myIntAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addDoubleAttribute(const SumoXMLAttr attr, const double value) {
    myDoubleAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addBoolAttribute(const SumoXMLAttr attr, const bool value) {
    myBoolAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addPositionAttribute(const SumoXMLAttr attr, const Position &value) {
    myPositionAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addTimeAttribute(const SumoXMLAttr attr, const SUMOTime value) {
    myTimeAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addColorAttribute(const SumoXMLAttr attr, const RGBColor &value) {
    myColorAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addStringListAttribute(const SumoXMLAttr attr, const std::vector<std::string> &value) {
    myStringListAttributes[attr] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addPositionVectorAttribute(const SumoXMLAttr attr, const PositionVector &value) {
    myPositionVectorAttributes[attr] = value;

}


void 
CommonXMLStructure::SumoBaseObject::addParameter(const std::string &key, const std::string &value) {
    myParameters[key] = value;
}


void 
CommonXMLStructure::SumoBaseObject::addSumoBaseObjectChild(SumoBaseObject* sumoBaseObject) {
    // just add it into mySumoBaseObjectChildren
    mySumoBaseObjectChildren.push_back(sumoBaseObject);
}


void
CommonXMLStructure::SumoBaseObject::removeSumoBaseObjectChild(SumoBaseObject* sumoBaseObject) {
    // find sumoBaseObject
    auto it = std::find(mySumoBaseObjectChildren.begin(), mySumoBaseObjectChildren.end(), sumoBaseObject);
    // check iterator
    if (it != mySumoBaseObjectChildren.end()) {
        mySumoBaseObjectChildren.erase(it);
    }
}

// ---------------------------------------------------------------------------
// CommonXMLStructure - methods
// ---------------------------------------------------------------------------

CommonXMLStructure::CommonXMLStructure() :
    mySumoBaseObjectRoot(nullptr),
    myLastInsertedSumoBaseObject(nullptr) {

}


CommonXMLStructure::~CommonXMLStructure() {
    // delete mySumoBaseObjectRoot (this will also delete all SumoBaseObjectChildrens)
    if (mySumoBaseObjectRoot) {
        delete mySumoBaseObjectRoot;
    }
}


void
CommonXMLStructure::openTag(const SumoXMLTag myTag) {
    // first check if root is empty
    if (mySumoBaseObjectRoot == nullptr) {
        // create root
        mySumoBaseObjectRoot = new SumoBaseObject(nullptr, myTag);
        // update last inserted Root
        myLastInsertedSumoBaseObject = mySumoBaseObjectRoot;
    } else {
        // create new node
        SumoBaseObject* newSumoBaseObject = new SumoBaseObject(myLastInsertedSumoBaseObject, myTag);
        // update last inserted node
        myLastInsertedSumoBaseObject = newSumoBaseObject; 
    }
}


void 
CommonXMLStructure::closeTag() {
    // check that myLastInsertedSumoBaseObject is valid
    if (myLastInsertedSumoBaseObject) {
        // just update last inserted SumoBaseObject
        myLastInsertedSumoBaseObject = myLastInsertedSumoBaseObject->getParentSumoBaseObject();
    }
}


void 
CommonXMLStructure::clearSumoBaseObjectRoot() {
    mySumoBaseObjectRoot = nullptr;
}


CommonXMLStructure::SumoBaseObject* 
CommonXMLStructure::getSumoBaseObjectRoot() const {
    return mySumoBaseObjectRoot;
}


CommonXMLStructure::SumoBaseObject* 
CommonXMLStructure::getLastInsertedSumoBaseObject() const {
    return myLastInsertedSumoBaseObject;
}

/****************************************************************************/
