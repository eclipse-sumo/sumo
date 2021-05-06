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

#include <string>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "CommonXMLStructure.h"


// ===========================================================================
// method definitions
// ===========================================================================

CommonXMLStructure::SumoBaseObject::SumoBaseObject(SumoBaseObject* _parent, const SumoXMLTag _tag) :
	parent(_parent),
	tag(_tag) {
}


CommonXMLStructure::SumoBaseObject::~SumoBaseObject() {
	// delete all SumoBaseObjectChildrens
	for (const auto &sumoBaseObject : SumoBaseObjectChildren) {
		delete sumoBaseObject;
	}
}


CommonXMLStructure::CommonXMLStructure() :
	mySumoBaseObjectRoot(nullptr),
	myLastInsertedSumoBaseObjectRoot(nullptr) {

}


CommonXMLStructure::~CommonXMLStructure() {
	// delete mySumoBaseObjectRoot (this will also delete all SumoBaseObjectChildrens)
	delete mySumoBaseObjectRoot;
}


void
CommonXMLStructure::openTag(const SumoXMLTag tag) {
	// first check if root is empty
	if (mySumoBaseObjectRoot == nullptr) {
		// create root
		mySumoBaseObjectRoot = new SumoBaseObject(nullptr, tag);
		// update last inserted Root
		myLastInsertedSumoBaseObjectRoot = mySumoBaseObjectRoot;
	} else {
		// create new node
		SumoBaseObject* newSumoBaseObject = new SumoBaseObject(myLastInsertedSumoBaseObjectRoot, tag);
		// update last inserted node
		myLastInsertedSumoBaseObjectRoot = newSumoBaseObject; 
	}
}


void 
CommonXMLStructure::closeTag() {
	// check that myLastInsertedNode is valid
	if (myLastInsertedSumoBaseObjectRoot) {
		// just update last inserted node
		myLastInsertedSumoBaseObjectRoot = myLastInsertedSumoBaseObjectRoot->parent;
	}
}


void 
CommonXMLStructure::addAttribute(const SumoXMLAttr attr, const std::string &value) {
	if (myLastInsertedSumoBaseObjectRoot) {
		myLastInsertedSumoBaseObjectRoot->stringAttributes[attr] = value;
	}
}


void 
CommonXMLStructure::addAttribute(const SumoXMLAttr attr, const int value) {
	if (myLastInsertedSumoBaseObjectRoot) {
		myLastInsertedSumoBaseObjectRoot->intAttributes[attr] = value;
	}
}


void 
CommonXMLStructure::addAttribute(const SumoXMLAttr attr, const double value) {
	if (myLastInsertedSumoBaseObjectRoot) {
		myLastInsertedSumoBaseObjectRoot->doubleAttributes[attr] = value;
	}
}


void 
CommonXMLStructure::addAttribute(const SumoXMLAttr attr, const SUMOTime value) {
	if (myLastInsertedSumoBaseObjectRoot) {
		myLastInsertedSumoBaseObjectRoot->SUMOTimeAttributes[attr] = value;
	}
}


void 
CommonXMLStructure::addAttribute(const SumoXMLAttr attr, const bool value) {
	if (myLastInsertedSumoBaseObjectRoot) {
		myLastInsertedSumoBaseObjectRoot->boolAttributes[attr] = value;
	}
}


void 
CommonXMLStructure::addParameter(const std::string &attr, const std::string &value) {
	if (myLastInsertedSumoBaseObjectRoot) {
		myLastInsertedSumoBaseObjectRoot->parameters[attr] = value;
	}
}

/****************************************************************************/
