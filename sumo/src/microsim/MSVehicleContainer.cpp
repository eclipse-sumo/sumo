/****************************************************************************/
/// @file    MSVehicleContainer.cpp
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// vehicles sorted by their departures
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#include <algorithm>
#include <cassert>
#include "MSVehicle.h"
#include "MSVehicleContainer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods from MSEmitControl::VehicleDepartureVectorSortCrit
 * ----------------------------------------------------------------------- */
bool
MSVehicleContainer::VehicleDepartureVectorSortCrit::operator()
(const VehicleDepartureVector& e1, const VehicleDepartureVector& e2) const
{
    return e1.first < e2.first;
}



/* -------------------------------------------------------------------------
 * methods from MSVehicleContainer::DepartFinder
 * ----------------------------------------------------------------------- */
MSVehicleContainer::DepartFinder::DepartFinder(SUMOTime time)
        : myTime(time)
{}


bool
MSVehicleContainer::DepartFinder::operator()
(const VehicleDepartureVector& e) const
{
    return myTime == e.first;
}



/* -------------------------------------------------------------------------
 * methods from MSVehicleContainer
 * ----------------------------------------------------------------------- */
MSVehicleContainer::MSVehicleContainer(size_t capacity)
        : currentSize(0), array(capacity + 1, VehicleDepartureVector())
{}


MSVehicleContainer::~MSVehicleContainer()
{
    // !!! vehicles are deleted in MSVehicle
}


void
MSVehicleContainer::add(MSVehicle *veh)
{
    // check whether a new item shall be added or the vehicle may be
    //  added to an existing list
    VehicleHeap::iterator i =
        find_if(array.begin()+1, array.begin()+currentSize+1, DepartFinder(veh->getDesiredDepart()));
    if (currentSize==0 || i==array.begin()+currentSize+1) {
        // a new heap-item is necessary
        VehicleDepartureVector newElem(veh->getDesiredDepart(), VehicleVector());
        newElem.second.push_back(veh);
        addReplacing(newElem);
    } else {
        // add vehicle to an existing heap-item
        (*i).second.push_back(veh);
    }
}


void
MSVehicleContainer::moveFrom(MSVehicleContainer &cont)
{
    if (cont.size()==0) {
        return;
    }
    for (VehicleHeap::iterator i=cont.array.begin()+1; i!=cont.array.begin()+cont.currentSize+1; ++i) {
        VehicleDepartureVector &v = (*i);
        add(v.first, v.second);
        v.second.clear();
    }
    cont.currentSize = 0;
}


void
MSVehicleContainer::add(SUMOTime time, const VehicleVector &cont)
{
    VehicleHeap::iterator j =
        find_if(array.begin()+1, array.begin()+currentSize+1,
                DepartFinder(time));
    if (currentSize==0 || j==array.begin()+currentSize+1) {
        VehicleDepartureVector newElem(time,
                                       VehicleVector(cont));
        addReplacing(newElem);
    } else {
        VehicleVector &stored = (*j).second;
        stored.reserve(stored.size() + cont.size());
        copy(cont.begin(), cont.end(), back_inserter(stored));
    }
}



void
MSVehicleContainer::addReplacing(const VehicleDepartureVector & x)
{
    if (isFull()) {
        std::vector<VehicleDepartureVector> array2((array.size()-1)*2+1, VehicleDepartureVector());
        for (size_t i=array.size(); i-->0;) {
            assert(array2.size()>i);
            array2[i] = array[i];
        }
        array = array2;
    }

    // Percolate up
    int hole = ++currentSize;
    for (; hole > 1 && (x.first < array[ hole / 2 ].first); hole /= 2) {
        assert(array.size()>(size_t) hole);
        array[ hole ] = array[ hole / 2 ];
    }
    assert(array.size()>(size_t) hole);
    array[ hole ] = x;
}


bool
MSVehicleContainer::anyWaitingFor(SUMOTime time) const
{
    VehicleHeap::const_iterator j =
        find_if(array.begin()+1, array.begin()+currentSize+1, DepartFinder(time));
    return j!=array.begin()+currentSize+1;
}


const MSVehicleContainer::VehicleVector &
MSVehicleContainer::top()
{
    if (isEmpty())
        throw 1;//!!!Underflow( );
    assert(array.size()>1);
    return array[ 1 ].second;
}


SUMOTime
MSVehicleContainer::topTime() const
{
    if (isEmpty())
        throw 1;//!!!Underflow( );
    assert(array.size()>1);
    return array[ 1 ].first;
}


void
MSVehicleContainer::pop()

{
    if (isEmpty())
        throw 1;//!!!Underflow( );

    assert(array.size()>1);
    array[ 1 ] = array[ currentSize-- ];
    percolateDown(1);
}


bool
MSVehicleContainer::isEmpty() const
{
    return currentSize == 0;
}


bool
MSVehicleContainer::isFull() const
{
    return currentSize >= ((int) array.size()) - 1;
}


void
MSVehicleContainer::percolateDown(int hole)
{
    int child;
    assert(array.size()>(size_t)hole);
    VehicleDepartureVector tmp = array[ hole ];

    for (; hole * 2 <= currentSize; hole = child) {
        child = hole * 2;
        if (child != currentSize && (array[ child + 1 ].first < array[ child ].first))
            child++;
        if ((array[ child ].first < tmp.first)) {
            assert(array.size()>(size_t) hole);
            array[ hole ] = array[ child ];
        } else
            break;
    }
    assert(array.size()>(size_t) hole);
    array[ hole ] = tmp;
}


size_t
MSVehicleContainer::size() const
{
    return currentSize;
}


void
MSVehicleContainer::showArray() const
{
    for (VehicleHeap::const_iterator i=array.begin()+1; i!=array.begin()+currentSize+1; ++i) {
        if (i!=array.begin()+1) {
            cout << ", ";
        }
        cout << (*i).first;
    }
    cout << endl << "-------------------------" << endl;
}


std::ostream &operator << (std::ostream &strm, MSVehicleContainer &cont)
{
    strm << "------------------------------------" << std::endl;
    while (!cont.isEmpty()) {
        const MSVehicleContainer::VehicleVector &v = cont.top();
        for (MSVehicleContainer::VehicleVector::const_iterator i=v.begin(); i!=v.end(); ++i) {
            strm << (*i)->getDesiredDepart() << std::endl;
        }
        cont.pop();
    }
    return strm;
}



/****************************************************************************/

