/***************************************************************************
                          MSNoLogicJunction.h  -  Junction that needs no 
                          logic, e.g. for exits.
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
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 16:02:21  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.2  2001/12/20 14:37:12  croessel
// using namespace std replaced by std:
//
// Revision 1.1  2001/12/12 17:46:02  croessel
// Initial commit. Part of a new junction hierarchy.
//

#ifndef MSNoLogicJunction_H
#define MSNoLogicJunction_H

#include <string>
#include <vector>
#include "MSJunction.h"

class MSLane;

/**
 */
class MSNoLogicJunction  : public MSJunction
{
public:
    /// Destructor.
    ~MSNoLogicJunction();
    
    /** Container for incoming lanes. */
    typedef std::vector< MSLane* > InLaneCont;
    
    /** Use this constructor only. */
    MSNoLogicJunction( std::string id, InLaneCont* in );
    
    /** Here, do nothing. */
    bool clearRequests();

    /** Here, do nothing. */
    bool setFirstVehiclesRequests();

    /** Look forward and move the vehicles to their target lane resp. 
        set them in the succeeding lane's buffer. */
    bool moveFirstVehicles();
    
    /** Integrate the moved vehicles into their target-lane. This is 
        neccessary if you use not thread-safe containers. */
    bool vehicles2targetLane();
    
protected:

private:
    /** Junction's in-lanes. */
    InLaneCont* myInLanes;

    /// Default constructor.
    MSNoLogicJunction();
    
    /// Copy constructor.
    MSNoLogicJunction( const MSNoLogicJunction& );
    
    /// Assignment operator.
    MSNoLogicJunction& operator=( const MSNoLogicJunction& );     
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSNoLogicJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










