#ifndef MSVehicleContainer_h
#define MSVehicleContainer_h
/***************************************************************************
                          MSVehicleContainer.h  -  A fast container for
                          vehicles sorted by their departures
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <iostream>
#include "MSNet.h"
#include "MSVehicle.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSVehicleContainer
 * A storage for vehicles, mainly used by the vehicle/route loading structures
 * and the emit control. Stores vehicles in a heap of vehiclevector/departure-
 * pairs.
 */
class MSVehicleContainer {
public:
    /// definition of a list of vehicles which have the same departure time
    typedef std::vector<MSVehicle*> VehicleVector;

    /** definition of a structure storing the departure time and a list
        of vehicles leaving at this time */
    typedef std::pair<MSNet::Time, VehicleVector> VehicleDepartureVector;

public:
    /// Constructor
    MSVehicleContainer(int capacity=10);

    /// Destructor
    ~MSVehicleContainer();

    /// Adds a single vehicle
    void add( MSVehicle *veh );

    /// Adds a container with vehicles departing at the given time
    void add( MSNet::Time time, const VehicleVector &cont );

    /** @briefMoves vehicles from the given container
        (The given container is empty afterwards) */
    void moveFrom( MSVehicleContainer &cont );

    /// Returns the information whether any vehicles want to depart at the given time
    bool anyWaitingFor(MSNet::Time time) const;

    /// Returns the uppermost vehicle vector
    const VehicleVector &top();

    /// Removes the uppermost vehicle vector
    void pop();

    /// Returns the information whether the container is empty
    bool isEmpty() const;

    /// Returns the size of the container
    size_t size() const;

    /// Prints the contents of the container
    friend std::ostream &operator << (std::ostream &strm, MSVehicleContainer &cont) {
        strm << "------------------------------------" << std::endl;
        while(!cont.isEmpty()) {
            const VehicleVector &v = cont.top();
            for(VehicleVector::const_iterator i=v.begin(); i!=v.end(); i++) {
                std::cout << (*i)->desiredDepart() << std::endl;
            }
            cont.pop();
        }
        return strm;
    }

private:
    /** @brief Replaces the existing single departure time vector by the one given
    */
    void addReplacing( const VehicleDepartureVector &cont );

    /** Returns the information whether the container must be extended */
    bool isFull( ) const;

    /// Sort-criterion for vehicle departure lists
    class VehicleDepartureVectorSortCrit
    {
    public:
        /// comparison operator
        bool operator() ( const VehicleDepartureVector& e1,
            const VehicleDepartureVector& e2 ) const;
    };

    /// Searches for the VehicleDepartureVector with the wished depart
    class DepartFinder
    {
    public:
        /// constructor
        explicit DepartFinder(MSNet::Time time);

        /// comparison operator
        bool operator() ( const VehicleDepartureVector& e ) const;

    private:
        /// the searched departure time
        MSNet::Time myTime;
    };

    /// Number of elements in heap
    int currentSize;

    /// Definition of the heap type
    typedef std::vector<VehicleDepartureVector> VehicleHeap;

    /// The vehicle vector heap
    VehicleHeap array;

    /// Moves the elements down
    void percolateDown( int hole );

};

#endif

