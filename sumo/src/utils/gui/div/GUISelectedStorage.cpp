//---------------------------------------------------------------------------//
//                        GUISelectedStorage.cpp -
//  Storage for object selections
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2004/11/23 10:38:29  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:47  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/07/02 08:08:33  dkrajzew
// global object selection added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <algorithm>
//#include "GUIGlobals.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GUISelectedStorage.h"
/*
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <microsim/MSLane.h>
*/


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * for GUISelectedStorage::SingleTypeSelections
 * ----------------------------------------------------------------------- */
GUISelectedStorage::SingleTypeSelections::SingleTypeSelections()
{
}


GUISelectedStorage::SingleTypeSelections::~SingleTypeSelections()
{
}


bool
GUISelectedStorage::SingleTypeSelections::isSelected(size_t id)
{
    std::vector<size_t>::iterator i=
        find(mySelected.begin(), mySelected.end(), id);
    return i!=mySelected.end();
}


void
GUISelectedStorage::SingleTypeSelections::select(size_t id)
{
    std::vector<size_t>::iterator i=
        find(mySelected.begin(), mySelected.end(), id);
    if(i==mySelected.end()) {
        mySelected.push_back(id);
    }
}


void
GUISelectedStorage::SingleTypeSelections::deselect(size_t id)
{
    std::vector<size_t>::iterator i=
        find(mySelected.begin(), mySelected.end(), id);
    if(i!=mySelected.end()) {
        mySelected.erase(i);
    }
}


void
GUISelectedStorage::SingleTypeSelections::clear()
{
    mySelected.clear();
}


void
GUISelectedStorage::SingleTypeSelections::load(const std::string &filename)
{
    ifstream strm(filename.c_str());
    while(strm.good()) {
        string name;
        strm >> name;
    }
}


void
GUISelectedStorage::SingleTypeSelections::save(const std::string &filename)
{
    ofstream strm(filename.c_str());
    save(strm);
}


void
GUISelectedStorage::SingleTypeSelections::save(std::ofstream &strm)
{
    for(std::vector<size_t>::iterator i=mySelected.begin(); i!=mySelected.end(); ++i) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(*i);
        if(object!=0) {
            std::string name = object->getFullName();
            strm << name << endl;
            gIDStorage.unblockObject(*i);
        }
    }
}


const std::vector<size_t> &
GUISelectedStorage::SingleTypeSelections::getSelected() const
{
    return mySelected;
}



/* -------------------------------------------------------------------------
 * for GUISelectedStorage
 * ----------------------------------------------------------------------- */
GUISelectedStorage::GUISelectedStorage()
{
}


GUISelectedStorage::~GUISelectedStorage()
{
}


bool
GUISelectedStorage::isSelected(int type, size_t id)
{
    if(type==-1) {
        GUIGlObject *object =
            gIDStorage.getObjectBlocking(id);
        if(object!=0) {
            type = object->getType();
            gIDStorage.unblockObject(id);
        } else {
            // !!! error message (could not be selected)
            return false;
        }
    }
    switch(type) {
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
    case GLO_LANESPEEDTRIGGER:
        return mySelectedTriggers.isSelected(id);
    case GLO_ADDITIONAL:
        return
            mySelectedTriggers.isSelected(id)
            | mySelectedEmitters.isSelected(id)
            | mySelectedDetectors.isSelected(id);
    default:
        throw 1;
    }
}


void
GUISelectedStorage::select(int type, size_t id)
{
    if(type==-1) {
        GUIGlObject *object =
            gIDStorage.getObjectBlocking(id);
        if(object!=0) {
            type = object->getType();
            gIDStorage.unblockObject(id);
        } else {
            // !!! error message (could not be selected)
            return;
        }
    }
    switch(type) {
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
    case GLO_LANESPEEDTRIGGER:
        mySelectedTriggers.select(id);
        break;
    default:
        throw 1;
    }
    std::vector<size_t>::iterator i=
        find(mySelected.begin(), mySelected.end(), id);
    if(i==mySelected.end()) {
        mySelected.push_back(id);
    }
}


void
GUISelectedStorage::addObjectChecking(size_t id, long withShift)
{
    GUIGlObject *o =
        gIDStorage.getObjectBlocking(id);
    if(o==0) {
        return;
    }
    bool selected = isSelected(-1, id);
    if(!selected) {
        select(o->getType(), id);
    } else {
        deselect(o->getType(), id);
    }
    /*
    //
    if(o->getType()==GLO_LANE&&withShift!=0) {
        throw 1;
        /*
        string name = o->microsimID();
        const GUIEdge &e =
            static_cast<const GUIEdge&>(MSLane::dictionary(name)->edge());
        for(size_t i=0; i<e.nLanes(); i++) {
            const GUILaneWrapper &l = e.getLaneGeometry(i);
            if(!selected) {
                select(GLO_LANE, l.getGlID());
            } else {
                deselect(GLO_LANE, l.getGlID());
            }
        }
    }
        */
    gIDStorage.unblockObject(id);
}


void
GUISelectedStorage::deselect(int type, size_t id)
{
    if(type==-1) {
        GUIGlObject *object =
            gIDStorage.getObjectBlocking(id);
        if(object!=0) {
            type = object->getType();
            gIDStorage.unblockObject(id);
        } else {
            // !!! error message (could not be selected)
            return;
        }
    }
    switch(type) {
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
    case GLO_LANESPEEDTRIGGER:
        mySelectedTriggers.deselect(id);
        break;
    default:
        throw 1;
    }
    std::vector<size_t>::iterator i=
        find(mySelected.begin(), mySelected.end(), id);
    if(i!=mySelected.end()) {
        mySelected.erase(i);
    }
}


const std::vector<size_t> &
GUISelectedStorage::getAllSelected() const
{
    return mySelected;
}


void
GUISelectedStorage::clear()
{
    mySelectedVehicles.clear();
    mySelectedTLLogics.clear();
    mySelectedDetectors.clear();
    mySelectedEmitters.clear();
    mySelectedLanes.clear();
    mySelectedEdges.clear();
    mySelectedJunctions.clear();
    mySelectedTriggers.clear();
    mySelected.clear();
}


void
GUISelectedStorage::load(int type, const std::string &filename)
{
    if(type!=-1) {
        switch(type) {
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
        case GLO_LANESPEEDTRIGGER:
            mySelectedTriggers.load(filename);
            break;
        default:
            throw 1;
        }
        return;
    }
    // ok, load all
    ifstream strm(filename.c_str());
    while(strm.good()) {
        string name;
        strm >> name;
    }
}


void
GUISelectedStorage::save(int type, const std::string &filename)
{
    if(type!=-1) {
        switch(type) {
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
        case GLO_LANESPEEDTRIGGER:
            mySelectedTriggers.save(filename);
            break;
        default:
            throw 1;
        }
        return;
    }
    // ok, save all
    ofstream strm(filename.c_str());
    for(std::vector<size_t>::iterator i=mySelected.begin(); i!=mySelected.end(); ++i) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(*i);
        if(object!=0) {
            std::string name = object->getFullName();
            strm << name << endl;
            gIDStorage.unblockObject(*i);
        }
    }
}


const std::vector<size_t> &
GUISelectedStorage::getSelected(int type) const
{
    switch(type) {
    case GLO_VEHICLE:
        return mySelectedVehicles.getSelected();
        break;
    case GLO_TLLOGIC:
        return mySelectedTLLogics.getSelected();
        break;
    case GLO_DETECTOR:
        return mySelectedDetectors.getSelected();
        break;
    case GLO_EMITTER:
        return mySelectedEmitters.getSelected();
        break;
    case GLO_LANE:
        return mySelectedLanes.getSelected();
        break;
    case GLO_EDGE:
        return mySelectedEdges.getSelected();
        break;
    case GLO_JUNCTION:
        return mySelectedJunctions.getSelected();
        break;
    case GLO_LANESPEEDTRIGGER:
        return mySelectedTriggers.getSelected();
        break;
    }
    throw 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



