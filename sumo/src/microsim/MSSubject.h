#ifndef MSSUBJECT_H
#define MSSUBJECT_H


/**
 * @file    MSSubject.h
 * @author  Christian Roessel <christian.roessel@dlr.de>
 * @date    Started Fri Nov 21 12:44:14 2003
 * @version $Id$
 * 
 * @brief  
 * 
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#include "MSObserver.h"
#include <algorithm>
#include <functional>
#include <vector>

// See Gamma et al. "Design Patterns" for information about the
// Observer pattern.

// To observe various quantities of the class ObservedType, create
// dummy-classes in a namespace related to the concrete
// Observable. e.g. if the concrete observable is the class Vehicle,
// put dummy classes Move{}, LaneChange{} etc. in a namespace veh. Use
// this dummy classes as third template parameter. It is convenient to
// declare some typedefs in the veh namespace:
// namespace veh
// {
//     class Move{};
//     class LaneChange{};
//    
//     typedef MSSubject< MSVehicle, false, Move >      MoveSubject;
//     typedef MSSubject< MSVehicle, true, LaneChange > LaneChangeSubject;    
//
//     typedef MoveSubject::Observer       MoveObserver;
//     typedef LaneChangeSubject::Observer LaneChangeObserver;
// }
//
// Then derive ObservedType (Vehicle in the example) from MoveSubject
// and from LaneChangeSubject.
//
// In the case of several observable quantities, the concrete subject
// (Vehicle) must override the inherited attach and detach
// methods. Just forward the call to the appropriate base class:
//
//     void attach( veh::LaneChangeObserver* toAttach ) {
//             veh::LaneChangeSubject::attach( toAttach );
//         }
//     void detach( veh::LaneChangeObserver* toDetach ) {
//             veh::LaneChangeSubject::detach( toDetach );
//         }
//     void attach( veh::MoveObserver* toAttach ) {
//             veh::MoveSubject::attach( toAttach );
//         }
//     void detach( veh::MoveObserver* toDetach ) {
//             veh::MoveSubject::detach( toDetach );
//         }    
//
// If you just have one quantity to observe, omit the third template
// parameter and do not override the detach and attach methods.
// 
// The parameter passesObserved selects the way the Observer is
// updated. If true, a reference to the observed object (Vehicle) is
// passed, so the Observer need not hold a reference/pointer to the
// observable/subject. If passesObserved === false, nothing is passed
// and the Observer must know the observable/subject.
//
// If an observable state of the concrete subject changes, call the
// notify method of the appropriate base:
//             veh::MoveSubject::notify();
//             veh::LaneChangeSubject::notify( *this );
// or just notify(maybe with parameter) if you have only one
// observable quantity.

class EmptyType{};

template<
    class ObservedType
    , bool passesObserved
    , class ObservedQuantity = EmptyType
    >
class MSSubject
{
public:
    typedef ObservedType Observed;
    
    typedef MSObserver<
        Observed
        , passesObserved
        , ObservedQuantity
        > Observer;
    
    typedef Observer* ObserverPtr;
    typedef std::vector< ObserverPtr > Observers;
    typedef typename Observers::iterator ObserversIt;
    
    void attach( ObserverPtr toAttach )
        {
            observersM.push_back( toAttach );
        }

    void detach( ObserverPtr toDetach )
        {
            ObserversIt eraseIt =
                std::find( observersM.begin(), observersM.end(), toDetach );
            assert( eraseIt != observersM.end() );
            observersM.erase( eraseIt );
        }
    
protected:
    
    MSSubject( void )
        : observersM()
        {}

    virtual ~MSSubject( void )
        {
            observersM.clear();
        }

    void notify( Observed& aObserved )
        {
            for ( ObserversIt observer = observersM.begin();
                  observer != observersM.end(); ++observer ) {
                (*observer)->update( aObserved );
            }
        }

    Observers observersM;

private:

    MSSubject( const MSSubject& );
    MSSubject& operator=( const MSSubject& );
};


// specialization for passesObserved == false
template<
    class ObservedType
    , class ObservedQuantity
    >
class MSSubject<
    ObservedType
    , false
    , ObservedQuantity
    >
{
public:
    typedef ObservedType Observed;
    
    typedef MSObserver<
        Observed
        , false
        , ObservedQuantity
        > Observer;
    
    typedef Observer* ObserverPtr;
    typedef std::vector< ObserverPtr > Observers;
    typedef typename Observers::iterator ObserversIt;

    void attach( ObserverPtr toAttach )
        {
            observersM.push_back( toAttach );
        }

    void detach( ObserverPtr toDetach )
        {
            ObserversIt eraseIt =
                std::find( observersM.begin(), observersM.end(), toDetach );
            assert( eraseIt != observersM.end() );
            observersM.erase( eraseIt );
        }
    
protected:

    MSSubject( void )
        : observersM()
        {}

    virtual ~MSSubject( void )
        {
            observersM.clear();
        }
    
    void notify( void )
        {
            std::for_each( observersM.begin(), observersM.end(),
                           std::mem_fun( &Observer::update ) );
        }

    Observers observersM;

private:

    MSSubject( const MSSubject& );
    MSSubject& operator=( const MSSubject& );
};


// Local Variables:
// mode:C++
// End:

#endif // MSSUBJECT_H
