/****************************************************************************/
/// @file    GUISelectedStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// Storage for "selected" objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GUISelectedStorage.h"
#include "GUIDialog_GLChosenEditor.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * for GUISelectedStorage::SingleTypeSelections
 * ----------------------------------------------------------------------- */
GUISelectedStorage::SingleTypeSelections::SingleTypeSelections() throw()
{}


GUISelectedStorage::SingleTypeSelections::~SingleTypeSelections() throw()
{}


bool
GUISelectedStorage::SingleTypeSelections::isSelected(GLuint id) throw()
{
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    return i!=mySelected.end();
}


void
GUISelectedStorage::SingleTypeSelections::select(GLuint id) throw()
{
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i==mySelected.end()) {
        mySelected.push_back(id);
    }
}


void
GUISelectedStorage::SingleTypeSelections::deselect(GLuint id) throw()
{
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i!=mySelected.end()) {
        mySelected.erase(i);
    }
}


void
GUISelectedStorage::SingleTypeSelections::clear() throw()
{
    mySelected.clear();
}


void
GUISelectedStorage::SingleTypeSelections::load(const std::string &filename) throw(IOError)
{
    ifstream strm(filename.c_str());
    while (strm.good()) {
        string name;
        strm >> name;
    }
}


void
GUISelectedStorage::SingleTypeSelections::save(const std::string &filename) throw(IOError)
{
    OutputDevice &dev = OutputDevice::getDevice(filename);
    for (std::vector<GLuint>::iterator i=mySelected.begin(); i!=mySelected.end(); ++i) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(*i);
        if (object!=0) {
            std::string name = object->getFullName();
            dev << name << "\n";
            gIDStorage.unblockObject(*i);
        }
    }
    dev.close();
}


const std::vector<GLuint> &
GUISelectedStorage::SingleTypeSelections::getSelected() const throw()
{
    return mySelected;
}



/* -------------------------------------------------------------------------
 * for GUISelectedStorage
 * ----------------------------------------------------------------------- */
GUISelectedStorage::GUISelectedStorage() throw()
{}


GUISelectedStorage::~GUISelectedStorage() throw()
{}


bool
GUISelectedStorage::isSelected(int type, GLuint id) throw(ProcessError)
{
    if (type==-1) {
        GUIGlObject *object =
            gIDStorage.getObjectBlocking(id);
        if (object!=0) {
            type = object->getType();
            gIDStorage.unblockObject(id);
        } else {
            throw ProcessError("Unkown object in GUISelectedStorage::isSelected (id=" + toString(id) + ").");
        }
    }
    switch (type) {
    case GLO_NETWORK:
        return false;
    case GLO_VEHICLE:
        return mySelectedVehicles.isSelected(id);
    case GLO_TLLOGIC:
        return mySelectedTLLogics.isSelected(id);
    case GLO_DETECTOR:
        return mySelectedDetectors.isSelected(id);
    case GLO_EMITTER:
        return mySelectedEmitters.isSelected(id);
    case GLO_LANE:
        return mySelectedLanes.isSelected(id);
    case GLO_EDGE:
        return mySelectedEdges.isSelected(id);
    case GLO_JUNCTION:
        return mySelectedJunctions.isSelected(id);
    case GLO_TRIGGER:
        return mySelectedTriggers.isSelected(id);
    case GLO_SHAPE:
        return mySelectedShapes.isSelected(id);
    case GLO_ADDITIONAL:
        return
            mySelectedTriggers.isSelected(id)
            | mySelectedEmitters.isSelected(id)
            | mySelectedDetectors.isSelected(id);
    default:
        throw ProcessError("Unkown object type in GUISelectedStorage::isSelected (type=" + toString(type) + ").");
    }
}


void
GUISelectedStorage::select(int type, GLuint id, bool update) throw(ProcessError)
{
    if (type==-1) {
        GUIGlObject *object =
            gIDStorage.getObjectBlocking(id);
        if (object!=0) {
            type = object->getType();
            gIDStorage.unblockObject(id);
        } else {
            throw ProcessError("Unkown object in GUISelectedStorage::select (id=" + toString(id) + ").");
        }
    }
    switch (type) {
    case GLO_VEHICLE:
        mySelectedVehicles.select(id);
        break;
    case GLO_TLLOGIC:
        mySelectedTLLogics.select(id);
        break;
    case GLO_DETECTOR:
        mySelectedDetectors.select(id);
        break;
    case GLO_EMITTER:
        mySelectedEmitters.select(id);
        break;
    case GLO_LANE:
        mySelectedLanes.select(id);
        break;
    case GLO_EDGE:
        mySelectedEdges.select(id);
        break;
    case GLO_JUNCTION:
        mySelectedJunctions.select(id);
        break;
    case GLO_TRIGGER:
        mySelectedTriggers.select(id);
        break;
    case GLO_SHAPE:
        mySelectedShapes.select(id);
        break;
    default:
        throw ProcessError("Unkown object type in GUISelectedStorage::select (type=" + toString(type) + ").");
    }
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i==mySelected.end()) {
        mySelected.push_back(id);
    }
    if (update&&my2Update!=0) {
        my2Update->rebuildList();
        my2Update->update();
    }
}


void
GUISelectedStorage::deselect(int type, GLuint id) throw(ProcessError)
{
    if (type==-1) {
        GUIGlObject *object =
            gIDStorage.getObjectBlocking(id);
        if (object!=0) {
            type = object->getType();
            gIDStorage.unblockObject(id);
        } else {
            throw ProcessError("Unkown object in GUISelectedStorage::deselect (id=" + toString(id) + ").");
        }
    }
    switch (type) {
    case GLO_VEHICLE:
        mySelectedVehicles.deselect(id);
        break;
    case GLO_TLLOGIC:
        mySelectedTLLogics.deselect(id);
        break;
    case GLO_DETECTOR:
        mySelectedDetectors.deselect(id);
        break;
    case GLO_EMITTER:
        mySelectedEmitters.deselect(id);
        break;
    case GLO_LANE:
        mySelectedLanes.deselect(id);
        break;
    case GLO_EDGE:
        mySelectedEdges.deselect(id);
        break;
    case GLO_JUNCTION:
        mySelectedJunctions.deselect(id);
        break;
    case GLO_TRIGGER:
        mySelectedTriggers.deselect(id);
        break;
    case GLO_SHAPE:
        mySelectedShapes.deselect(id);
        break;
    default:
        throw ProcessError("Unkown object type in GUISelectedStorage::deselect (type=" + toString(type) + ").");
    }
    std::vector<GLuint>::iterator i = find(mySelected.begin(), mySelected.end(), id);
    if (i!=mySelected.end()) {
        mySelected.erase(i);
    }
    if (my2Update!=0) {
        my2Update->rebuildList();
        my2Update->update();
    }
}


void
GUISelectedStorage::toggleSelection(GLuint id) throw(ProcessError)
{
    GUIGlObject *o =
        gIDStorage.getObjectBlocking(id);
    if (o==0) {
        throw ProcessError("Unkown object in GUISelectedStorage::toggleSelection (id=" + toString(id) + ").");
    }
    bool selected = isSelected(-1, id);
    if (!selected) {
        select(o->getType(), id);
    } else {
        deselect(o->getType(), id);
    }
    gIDStorage.unblockObject(id);
}


const std::vector<GLuint> &
GUISelectedStorage::getSelected() const throw()
{
    return mySelected;
}


const std::vector<GLuint> &
GUISelectedStorage::getSelected(GUIGlObjectType type) const throw(ProcessError)
{
    switch (type) {
    case GLO_VEHICLE:
        return mySelectedVehicles.getSelected();
    case GLO_TLLOGIC:
        return mySelectedTLLogics.getSelected();
    case GLO_DETECTOR:
        return mySelectedDetectors.getSelected();
    case GLO_EMITTER:
        return mySelectedEmitters.getSelected();
    case GLO_LANE:
        return mySelectedLanes.getSelected();
    case GLO_EDGE:
        return mySelectedEdges.getSelected();
    case GLO_JUNCTION:
        return mySelectedJunctions.getSelected();
    case GLO_TRIGGER:
        return mySelectedTriggers.getSelected();
    case GLO_SHAPE:
        return mySelectedShapes.getSelected();
    }
    throw ProcessError("Unkown object type in GUISelectedStorage::getSelected (type=" + toString(type) + ").");
}


void
GUISelectedStorage::clear() throw()
{
    mySelectedVehicles.clear();
    mySelectedTLLogics.clear();
    mySelectedDetectors.clear();
    mySelectedEmitters.clear();
    mySelectedLanes.clear();
    mySelectedEdges.clear();
    mySelectedJunctions.clear();
    mySelectedTriggers.clear();
    mySelectedShapes.clear();
    mySelected.clear();
    if (my2Update!=0) {
        my2Update->rebuildList();
        my2Update->update();
    }
}


void
GUISelectedStorage::load(int type, const std::string &filename) throw(IOError)
{
    if (type!=-1) {
        switch (type) {
        case GLO_VEHICLE:
            mySelectedVehicles.load(filename);
            break;
        case GLO_TLLOGIC:
            mySelectedTLLogics.load(filename);
            break;
        case GLO_DETECTOR:
            mySelectedDetectors.load(filename);
            break;
        case GLO_EMITTER:
            mySelectedEmitters.load(filename);
            break;
        case GLO_LANE:
            mySelectedLanes.load(filename);
            break;
        case GLO_EDGE:
            mySelectedEdges.load(filename);
            break;
        case GLO_JUNCTION:
            mySelectedJunctions.load(filename);
            break;
        case GLO_TRIGGER:
            mySelectedTriggers.load(filename);
            break;
        case GLO_SHAPE:
            mySelectedShapes.load(filename);
            break;
        default:
            throw ProcessError("Unkown object type in GUISelectedStorage::load (type=" + toString(type) + ").");
        }
        return;
    }

}


void
GUISelectedStorage::save(int type, const std::string &filename) throw(IOError)
{
    if (type!=-1) {
        switch (type) {
        case GLO_VEHICLE:
            mySelectedVehicles.save(filename);
            break;
        case GLO_TLLOGIC:
            mySelectedTLLogics.save(filename);
            break;
        case GLO_DETECTOR:
            mySelectedDetectors.save(filename);
            break;
        case GLO_EMITTER:
            mySelectedEmitters.save(filename);
            break;
        case GLO_LANE:
            mySelectedLanes.save(filename);
            break;
        case GLO_EDGE:
            mySelectedEdges.save(filename);
            break;
        case GLO_JUNCTION:
            mySelectedJunctions.save(filename);
            break;
        case GLO_TRIGGER:
            mySelectedTriggers.save(filename);
            break;
        case GLO_SHAPE:
            mySelectedShapes.save(filename);
            break;
        default:
            throw ProcessError("Unkown object type in GUISelectedStorage::load (type=" + toString(type) + ").");
        }
        return;
    }
    // ok, save all
    OutputDevice &dev = OutputDevice::getDevice(filename);
    for (std::vector<GLuint>::iterator i=mySelected.begin(); i!=mySelected.end(); ++i) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(*i);
        if (object!=0) {
            std::string name = object->getFullName();
            dev << name << '\n';
            gIDStorage.unblockObject(*i);
        }
    }
    dev.close();
}


void
GUISelectedStorage::add2Update(GUIDialog_GLChosenEditor *ed) throw()
{
    my2Update = ed;
}


void
GUISelectedStorage::remove2Update(GUIDialog_GLChosenEditor *) throw()
{
    my2Update = 0;
}



/****************************************************************************/

