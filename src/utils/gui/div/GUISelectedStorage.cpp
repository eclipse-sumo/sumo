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
/// @file    GUISelectedStorage.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Jun 2004
///
// Storage for "selected" objects
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/ToString.h>

#include "GUISelectedStorage.h"
#include "GUIDialog_GLChosenEditor.h"


// ===========================================================================
// member method definitions
// ===========================================================================

/* -------------------------------------------------------------------------
 * for GUISelectedStorage::SingleTypeSelections
 * ----------------------------------------------------------------------- */

GUISelectedStorage::SingleTypeSelections::SingleTypeSelections() {}


GUISelectedStorage::SingleTypeSelections::~SingleTypeSelections() {}


bool
GUISelectedStorage::SingleTypeSelections::isSelected(GUIGlID id) {
    return mySelected.count(id) > 0;
}


void
GUISelectedStorage::SingleTypeSelections::select(GUIGlID id) {
    mySelected.insert(id);
}


void
GUISelectedStorage::SingleTypeSelections::deselect(GUIGlID id) {
    mySelected.erase(id);
}


void
GUISelectedStorage::SingleTypeSelections::clear() {
    mySelected.clear();
}


void
GUISelectedStorage::SingleTypeSelections::save(const std::string& filename) {
    GUISelectedStorage::save(filename, mySelected);
}


const std::unordered_set<GUIGlID>&
GUISelectedStorage::SingleTypeSelections::getSelected() const {
    return mySelected;
}

/* -------------------------------------------------------------------------
 * for GUISelectedStorage
 * ----------------------------------------------------------------------- */

GUISelectedStorage::GUISelectedStorage() {}


GUISelectedStorage::~GUISelectedStorage() {}


bool
GUISelectedStorage::isSelected(GUIGlObjectType type, GUIGlID id) {
    switch (type) {
        case GLO_NETWORK:
            return false;
        default:
            return mySelections[type].isSelected(id);
    }
}

bool
GUISelectedStorage::isSelected(const GUIGlObject* o) {
    if (o == nullptr) {
        return false;
    } else {
        return isSelected(o->getType(), o->getGlID());
    }
}

void
GUISelectedStorage::select(GUIGlID id, bool update) {
    GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (!object) {
        throw ProcessError("Unknown object in GUISelectedStorage::select (id=" + toString(id) + ").");
    }
    GUIGlObjectType type = object->getType();
    GUIGlObjectStorage::gIDStorage.unblockObject(id);

    mySelections[type].select(id);
    myAllSelected.insert(id);
    if (update && myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


void
GUISelectedStorage::deselect(GUIGlID id) {
    GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (!object) {
        throw ProcessError("Unknown object in GUISelectedStorage::deselect (id=" + toString(id) + ").");
    }
    GUIGlObjectType type = object->getType();
    GUIGlObjectStorage::gIDStorage.unblockObject(id);

    mySelections[type].deselect(id);
    myAllSelected.erase(id);
    if (myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


void
GUISelectedStorage::deselect(GUIGlObjectType type, GUIGlID id) {
    mySelections[type].deselect(id);
    myAllSelected.erase(id);
}


void
GUISelectedStorage::toggleSelection(GUIGlID id) {
    GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (!object) {
        throw ProcessError("Unknown object in GUISelectedStorage::toggleSelection (id=" + toString(id) + ").");
    }

    bool selected = isSelected(object->getType(), id);
    if (!selected) {
        select(id);
    } else {
        deselect(id);
    }
    GUIGlObjectStorage::gIDStorage.unblockObject(id);
}


const std::unordered_set<GUIGlID>&
GUISelectedStorage::getSelected() const {
    return myAllSelected;
}


const std::unordered_set<GUIGlID>&
GUISelectedStorage::getSelected(GUIGlObjectType type) {
    return mySelections[type].getSelected();
}


void
GUISelectedStorage::clear() {
    for (auto& selection : mySelections) {
        selection.second.clear();
    }
    myAllSelected.clear();
    if (myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


void
GUISelectedStorage::notifyChanged() {
    if (myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


std::set<GUIGlID>
GUISelectedStorage::loadIDs(std::istream& strm, std::string& msgOut, GUIGlObjectType type, std::ostream* dynamicNotFound, int maxErrors) {
    std::set<GUIGlID> result;
    std::ostringstream msg;
    int numIgnored = 0;
    int numMissing = 0;
    while (strm.good()) {
        std::string line;
        strm >> line;
        if (line.length() == 0) {
            continue;
        }
        if (StringUtils::startsWith(line, "node:")) {
            line = StringUtils::replace(line, "node:", "junction:");
        }

        GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(line);
        if (object) {
            if (type != GLO_MAX && (object->getType() != type)) {
                numIgnored++;
                if (numIgnored + numMissing <= maxErrors) {
                    msg << TLF("Ignoring item '%' because of invalid type %\n", line, toString(object->getType()));
                }
            } else {
                result.insert(object->getGlID());
            }
        } else {
            numMissing++;
            if (dynamicNotFound != nullptr && (
                        StringUtils::startsWith(line, "vehicle:") ||
                        StringUtils::startsWith(line, "person:") ||
                        StringUtils::startsWith(line, "container:"))) {
                (*dynamicNotFound) << line << "\n";
            } else {
                if (numIgnored + numMissing <= maxErrors) {
                    msg << TLF("Item '%' not found\n", line);
                }
            }
            continue;
        }
    }
    if (numIgnored + numMissing > maxErrors) {
        msg << "...\n" << TLF("% objects ignored, % objects not found\n", numIgnored, numMissing);
    }
    msgOut = msg.str();
    return result;
}


std::string
GUISelectedStorage::load(const std::string& filename, GUIGlObjectType type, std::ostream* dynamicNotFound) {
    std::ifstream strm(filename.c_str());
    if (!strm.good()) {
        return TLF("Could not open '%'.\n", filename);
    }
    std::string errors = load(strm, type, dynamicNotFound);
    strm.close();
    return errors;
}


std::string
GUISelectedStorage::load(std::istream& strm, GUIGlObjectType type, std::ostream* dynamicNotFound) {
    std::string errors;
    const std::set<GUIGlID> ids = loadIDs(strm, errors, type, dynamicNotFound);
    for (const auto glID : ids) {
        select(glID, false);
    }
    if (myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
    return errors;
}


void
GUISelectedStorage::save(GUIGlObjectType type, const std::string& filename) {
    mySelections[type].save(filename);
}


void
GUISelectedStorage::save(const std::string& filename) const {
    save(filename, myAllSelected);
}


void
GUISelectedStorage::add2Update(UpdateTarget* updateTarget) {
    myUpdateTarget = updateTarget;
}


void
GUISelectedStorage::remove2Update() {
    myUpdateTarget = nullptr;
}


void
GUISelectedStorage::save(const std::string& filename, const std::unordered_set<GUIGlID>& ids) {
    OutputDevice& dev = OutputDevice::getDevice(filename);
    for (const auto glID : ids) {
        GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(glID);
        if (object != nullptr) {
            std::string name = object->getFullName();
            dev << name << "\n";
            GUIGlObjectStorage::gIDStorage.unblockObject(glID);
        }
    }
    dev.close();
}


/****************************************************************************/
