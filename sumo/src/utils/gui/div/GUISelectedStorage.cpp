/****************************************************************************/
/// @file    GUISelectedStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// Storage for "selected" objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include "GUISelectedStorage.h"
#include "GUIDialog_GLChosenEditor.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * for GUISelectedStorage::SingleTypeSelections
 * ----------------------------------------------------------------------- */
GUISelectedStorage::SingleTypeSelections::SingleTypeSelections() throw() {}


GUISelectedStorage::SingleTypeSelections::~SingleTypeSelections() throw() {}


bool
GUISelectedStorage::SingleTypeSelections::isSelected(GLuint id) throw() {
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    return i!=mySelected.end();
}


void
GUISelectedStorage::SingleTypeSelections::select(GLuint id) throw() {
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i==mySelected.end()) {
        mySelected.push_back(id);
    }
}


void
GUISelectedStorage::SingleTypeSelections::deselect(GLuint id) throw() {
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i!=mySelected.end()) {
        mySelected.erase(i);
    }
}


void
GUISelectedStorage::SingleTypeSelections::clear() throw() {
    mySelected.clear();
}


void
GUISelectedStorage::SingleTypeSelections::load(const std::string &filename) throw(IOError) {
    std::ifstream strm(filename.c_str());
    while (strm.good()) {
        std::string name;
        strm >> name;
    }
}


void
GUISelectedStorage::SingleTypeSelections::save(const std::string &filename) throw(IOError) {
    OutputDevice &dev = OutputDevice::getDevice(filename);
    for (std::vector<GLuint>::iterator i=mySelected.begin(); i!=mySelected.end(); ++i) {
        GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(*i);
        if (object!=0) {
            std::string name = object->getFullName();
            dev << name << "\n";
            GUIGlObjectStorage::gIDStorage.unblockObject(*i);
        }
    }
    dev.close();
}


const std::vector<GLuint> &
GUISelectedStorage::SingleTypeSelections::getSelected() const throw() {
    return mySelected;
}



/* -------------------------------------------------------------------------
 * for GUISelectedStorage
 * ----------------------------------------------------------------------- */
GUISelectedStorage::GUISelectedStorage() throw() {}


GUISelectedStorage::~GUISelectedStorage() throw() {}


bool
GUISelectedStorage::isSelected(GUIGlObjectType type, GLuint id) throw(ProcessError) {
    switch (type) {
        case GLO_NETWORK:
            return false;
        case GLO_ADDITIONAL:
            return isSelected(GLO_TRIGGER, id) || isSelected(GLO_DETECTOR, id);
        default:
            return mySelections[type].isSelected(id);
    }
}


void
GUISelectedStorage::select(GLuint id, bool update) throw(ProcessError) {
    GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (!object) {
        throw ProcessError("Unkown object in GUISelectedStorage::deselect (id=" + toString(id) + ").");
    }
    GUIGlObjectType type = object->getType();
    GUIGlObjectStorage::gIDStorage.unblockObject(id);

    mySelections[type].select(id);
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i==mySelected.end()) {
        mySelected.push_back(id);
    }
    if (update && myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


void
GUISelectedStorage::deselect(GLuint id) throw(ProcessError) {
    GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (!object) {
        throw ProcessError("Unkown object in GUISelectedStorage::deselect (id=" + toString(id) + ").");
    }
    GUIGlObjectType type = object->getType();
    GUIGlObjectStorage::gIDStorage.unblockObject(id);

    mySelections[type].deselect(id);
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i!=mySelected.end()) {
        mySelected.erase(i);
    }
    if (myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


void
GUISelectedStorage::toggleSelection(GLuint id) throw(ProcessError) {
    GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (!object) {
        throw ProcessError("Unkown object in GUISelectedStorage::toggleSelection (id=" + toString(id) + ").");
    }

    bool selected = isSelected(object->getType(), id);
    if (!selected) {
        select(id);
    } else {
        deselect(id);
    }
    GUIGlObjectStorage::gIDStorage.unblockObject(id);
}


const std::vector<GLuint> &
GUISelectedStorage::getSelected() const {
    return mySelected;
}


const std::vector<GLuint> &
GUISelectedStorage::getSelected(GUIGlObjectType type) {
    return mySelections[type].getSelected();
}


void
GUISelectedStorage::clear() throw() {
    for (std::map<GUIGlObjectType, SingleTypeSelections>::iterator it = mySelections.begin(); it != mySelections.end(); it++) {
        it->second.clear();
    }
    if (myUpdateTarget) {
        myUpdateTarget->selectionUpdated();
    }
}


void
GUISelectedStorage::load(GUIGlObjectType type, const std::string &filename) throw(IOError) {
    mySelections[type].load(filename);
}


void
GUISelectedStorage::save(GUIGlObjectType type, const std::string &filename) throw(IOError) {
    mySelections[type].save(filename);
}


void
GUISelectedStorage::save(const std::string &filename) const throw(IOError) {
    OutputDevice &dev = OutputDevice::getDevice(filename);
    for (std::vector<GLuint>::const_iterator i=mySelected.begin(); i!=mySelected.end(); ++i) {
        GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(*i);
        if (object!=0) {
            std::string name = object->getFullName();
            dev << name << '\n';
            GUIGlObjectStorage::gIDStorage.unblockObject(*i);
        }
    }
    dev.close();
}


void
GUISelectedStorage::add2Update(UpdateTarget *updateTarget) throw() {
    myUpdateTarget = updateTarget;
}


void
GUISelectedStorage::remove2Update() throw() {
    myUpdateTarget = 0;
}

/****************************************************************************/
