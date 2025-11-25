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
/// @date    Nov 2025
///
// class used for link Elements with their filenames
/****************************************************************************/

#include "FileBucket.h"

// ===========================================================================
// static members
// ===========================================================================

const std::vector<FileBucket::Type> FileBucket::types = {
    FileBucket::Type::SUMO_CONFIG,
    FileBucket::Type::NETEDIT_CONFIG,
    FileBucket::Type::NETCONVERT_CONFIG,
    FileBucket::Type::NETWORK,
    FileBucket::Type::EDGETYPE,
    FileBucket::Type::TLS,
    FileBucket::Type::DEMAND,
    FileBucket::Type::MEANDATA,
    FileBucket::Type::ADDITIONAL,
    FileBucket::Type::DATA
};

const std::vector<FileBucket::Type> FileBucket::prefixes = {
    FileBucket::Type::SUMO_PREFIX,
    FileBucket::Type::NETEDIT_PREFIX,
    FileBucket::Type::NETCONVERT_PREFIX,
    FileBucket::Type::NETWORK_PREFIX
};

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


void
FileBucket::addElement() {
    myNumElements++;
}


void
FileBucket::removeElement() {
    myNumElements--;
}


bool
FileBucket::isEmpty() const {
    return myNumElements == 0;
}

/****************************************************************************/
