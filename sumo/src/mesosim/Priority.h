/****************************************************************************/
/// @file    Priority.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id: Priority.h 96 2007-06-06 07:40:46Z behr_mi $
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
#ifndef Priority_h
#define Priority_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <iostream>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Priority
 * A storage for vehicles, mainly used by the vehicle/route loading structures
 * and the emit control. Stores vehicles in a heap of vehiclevector/departure-
 * pairs.
 */
template<class T>
class Priority
{
public:
    /// definition of a list of vehicles which have the same departure time
    typedef std::vector<T*> VehicleVector;

    /** definition of a structure storing the departure time and a list
        of vehicles leaving at this time */
    typedef std::pair<SUMOReal, VehicleVector> VehicleDepartureVector;

public:
    /// Constructor
    Priority(int capacity=10)
            : currentSize(0), array(capacity + 1, VehicleDepartureVector(0, VehicleVector()))
    {
        if (capacity<1)
            throw 1;
    }

    /// Destructor
    ~Priority()
    {
        // vehicles are deleted in MSVehicle
    }

    /// Adds a single vehicle
    void add(T *veh,  SUMOReal val)
    {
        // check whether a new item shall be added or the vehicle may be
        //  added to an existing list
        typename VehicleHeap::iterator i =
            find_if(array.begin()+1, array.begin()+currentSize+1, DepartFinder(val));
        if (currentSize==0 || i==array.begin()+currentSize+1) {
            // a new heap-item is necessary
            VehicleDepartureVector newElem(val, VehicleVector());
            newElem.second.push_back(veh);
            addReplacing(newElem);
        } else {
            // add vehicle to an existing heap-item
            (*i).second.push_back(veh);
        }
    }

    bool erase(T *veh,  SUMOReal val)
    {
        typename VehicleHeap::iterator i =
            find_if(array.begin()+1, array.begin()+currentSize+1, DepartFinder(val));
        if (currentSize==0 || i==array.begin()+currentSize+1) {
            // !!! not in container
            return false;
        }
        VehicleVector &vv = (*i).second;
        typename VehicleVector::iterator j = std::find(vv.begin(), vv.end(), veh);
        if (j!=vv.end()) {
            vv.erase(j);
            return true;
        }
        return false;
    }


    bool remap(T *veh, SUMOReal prev, SUMOReal newTime)
    {
        if (prev==newTime) {
            return true;
        }
        if (erase(veh, prev)) {
            add(veh, newTime);
            return true;
        }
        return false;
        /*
        VehicleHeap::iterator i =
            find_if(array.begin()+1, array.begin()+currentSize+1,
                DepartFinder(val));
        if(currentSize==0 || i==array.begin()+currentSize+1) {
            // !!! not in container
            return;
        }
        VehicleVector &vv = (*i).second;
        VehicleVector::iterator j = std::find(vv.begin(), vv.end(), veh);
        if(j!=vv.end()) {
            vv.erase(j);
        }
        */
    }


    /// Adds a container with vehicles departing at the given time
    void add(SUMOReal time, const VehicleVector &cont)
    {
        if (time==28173.97) {
            std::cout << "4:----" << std::endl;
        }
        typename VehicleHeap::iterator j =
            find_if(array.begin()+1, array.begin()+currentSize+1, DepartFinder(time));
        if (currentSize==0 || j==array.begin()+currentSize+1) {
            VehicleDepartureVector newElem(time, VehicleVector(cont));
            addReplacing(newElem);
        } else {
            VehicleVector &stored = (*j).second;
            stored.reserve(stored.size() + cont.size());
            copy(cont.begin(), cont.end(), back_inserter(stored));
        }
    }

    /** @briefMoves vehicles from the given container
        (The given container is empty afterwards) */
    void moveFrom(Priority &cont)
    {
        if (cont.size()==0) {
            return;
        }
        for (typename VehicleHeap::iterator i=cont.array.begin()+1; i!=cont.array.begin()+cont.currentSize+1; i++) {
            VehicleDepartureVector &v = (*i);
            add(v.first, v.second);
            v.second.clear();
        }
        cont.currentSize = 0;
    }

    /// Returns the information whether any vehicles want to depart at the given time
    bool anyWaitingFor(SUMOReal time) const
    {
        typename VehicleHeap::const_iterator j =
            find_if(array.begin()+1, array.begin()+currentSize+1, DepartFinder(time));
        return j!=array.begin()+currentSize+1;
    }


    /// Returns the uppermost vehicle vector
    const VehicleVector &top()
    {
        if (isEmpty())
            throw 1;//!!!Underflow( );
        assert(array.size()>1);
        return array[ 1 ].second;
    }


    /// Returns the time the uppermost vehicle vector is assigned to
    SUMOReal topTime() const
    {
        if (isEmpty())
            throw 1;//!!!Underflow( );
        assert(array.size()>1);
        return array[ 1 ].first;
    }


    /// Removes the uppermost vehicle vector
    void pop()
    {
        if (isEmpty())
            throw 1;//!!!Underflow( );

        assert(array.size()>1);
        array[ 1 ] = array[ currentSize-- ];
        percolateDown(1);
    }


    /// Returns the information whether the container is empty
    bool isEmpty() const
    {
        return currentSize == 0;
    }


    /// Returns the size of the container
    size_t size() const
    {
        return currentSize;
    }



    /// Prints the container (the departure times)
    void showArray() const
    {
        for (typename VehicleHeap::const_iterator i=array.begin()+1; i!=array.begin()+currentSize+1; i++) {
            if (i!=array.begin()+1) {
                std::cout << ", ";
            }
            std::cout << (*i).first;
        }
        std::cout << std::endl << "-------------------------" << std::endl;
    }


    /// Definition of the heap type
    typedef std::vector<VehicleDepartureVector> VehicleHeap;

    typename VehicleHeap::iterator begin()
    {
        return array.begin();
    }

    typename VehicleHeap::iterator end()
    {
        return array.end();
    }


    /// Prints the contents of the container
    friend std::ostream &operator << (std::ostream &strm,
                                      Priority &cont)
    {
        strm << "------------------------------------" << std::endl;
        while (!cont.isEmpty()) {
            const VehicleVector &v = cont.top();
            for (typename VehicleVector::const_iterator i=v.begin(); i!=v.end(); i++) {
                strm << (*i)->desiredDepart() << std::endl;
            }
            cont.pop();
        }
        return strm;
    }

private:
    /** @brief Replaces the existing single departure time vector by the one given
    */
    void addReplacing(const VehicleDepartureVector &x)
    {
        if (isFull()) {
            std::vector<VehicleDepartureVector> array2((array.size()-1)*2+1, VehicleDepartureVector(0, VehicleVector()));
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

    /** Returns the information whether the container must be extended */
    bool isFull() const
    {
        return currentSize >= ((int) array.size()) - 1;
    }

    /// Sort-criterion for vehicle departure lists
    class VehicleDepartureVectorSortCrit
    {
    public:
        /// comparison operator
        bool operator()(const VehicleDepartureVector& e1, const VehicleDepartureVector& e2) const
        {
            return e1.first < e2.first;
        }

    };

    /// Searches for the VehicleDepartureVector with the wished depart
    class DepartFinder
    {
    public:
        /// constructor
        explicit DepartFinder(SUMOReal time): myTime(time)
        {}

        /// comparison operator
        bool operator()(const VehicleDepartureVector& e) const
        {
            return myTime == e.first;
        }

    private:
        /// the searched departure time
        SUMOReal myTime;
    };

    /// Number of elements in heap
    int currentSize;

    /// The vehicle vector heap
    VehicleHeap array;

    /// Moves the elements down
    void percolateDown(int hole)
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


};


#endif

/****************************************************************************/

