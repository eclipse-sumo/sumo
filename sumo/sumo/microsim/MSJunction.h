/***************************************************************************
                          MSJunction.h  -  Base class for all kinds of 
                                           junctions.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
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
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:15  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/01 11:40:34  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.13  2001/12/20 11:54:05  croessel
// using namespace std replaced by std::
//
// Revision 1.12  2001/12/12 17:46:01  croessel
// Initial commit. Part of a new junction hierarchy.
//

#ifndef MSJunction_H
#define MSJunction_H

#include <string>
#include <map>

/**
 */
class MSJunction
{
public:
    /// Destructor.
    virtual ~MSJunction();
    
    /** Use this constructor only. */    
    MSJunction( std::string id );
    
    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    virtual bool clearRequests() = 0;

    /** Call all lanes in myLanes and let them set their first vehicle's
        requests. Before, empty old requests/responds. */
    virtual bool setFirstVehiclesRequests() = 0;

    /** Collect the first car's requests, calculate the respond
        according to the right of way rules and move the vehicles on
        their lane resp. set them in the succeeding lane's buffer. */
    virtual bool moveFirstVehicles() = 0;
    
    /** Integrate the moved vehicles into their target-lane. This is 
        neccessary if you use not thread-safe containers. */
    virtual bool vehicles2targetLane() = 0;

    /** Inserts MSJunction into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSJunction* junction);

    /** Returns the MSEdgeControl associated to the key id if exists,
        otherwise returns 0. */
    static MSJunction* dictionary( std::string id);
    
protected:
    // unique ID
    std::string myID;

private:
    /** Static dictionary to associate string-ids with objects. */
    typedef std::map< std::string, MSJunction* > DictType;
    static DictType myDict;
    
    /// Default constructor.
    MSJunction();
    
    /// Copy constructor.
    MSJunction( const MSJunction& );
    
    /// Assignment operator.
    MSJunction& operator=( const MSJunction& );     
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










