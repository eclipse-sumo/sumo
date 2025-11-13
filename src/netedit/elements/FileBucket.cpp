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
/// @file    FileBucket.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// class used for link Elements with their filenames
/****************************************************************************/

#include "GNEAttributeCarrier.h"
#include "FileBucket.h"

// ===========================================================================
// member method definitions
// ===========================================================================

FileBucket::FileBucket(FileBucket::Type type) :
    myFileType(type),
    myDefaultBucket(true) {
}


FileBucket::FileBucket(FileBucket::Type type, const std::string filename) :
    myFileType(type),
    myFilename(filename),
    myDefaultBucket(false) {
}


FileBucket::~FileBucket() {}


FileBucket::Type
FileBucket::getType() const {
    return myFileType;
}


const std::string&
FileBucket::getFilename() const {
    return myFilename;
}


void
FileBucket::setFilename(const std::string& filename) {
    myFilename = filename;
}


bool
FileBucket::isDefaultBucket() const {
    return myDefaultBucket;
}


bool
FileBucket::isEmpty() const {
    return myElements.empty();
}


void
FileBucket::addElement(const void* element) {
    myElements.insert(element);
}


void
FileBucket::removeElement(const void* element) {
    myElements.erase(element);
}


bool
FileBucket::hasElement(const void* element) const {
    return myElements.find(element) != myElements.end();
}


std::string
FileBucket::parseFilenames(const std::vector<FileBucket*>& fileBuckets) {
    std::string result;
    // group all saving files in a single string separated with comma
    for (const auto& bucket : fileBuckets) {
        result.append(bucket->getFilename() + ",");
    }
    // remove last ','
    if (result.size() > 0) {
        result.pop_back();
    }
    return result;
}

/****************************************************************************/
