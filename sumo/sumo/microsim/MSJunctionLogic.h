/***************************************************************************
                          MSJunctionLogic.h  -  Base class for different 
                          kinds of logic-implementations.
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
// Revision 2.0  2002/02/14 14:43:16  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.2  2002/02/13 16:29:45  croessel
// Added dictionary.
//
// Revision 1.1  2001/12/13 15:43:13  croessel
// Initial commit.
//

#ifndef MSJunctionLogic_H
#define MSJunctionLogic_H

#include "MSLogicJunction.h"
#include <string>

/**
 */
class MSJunctionLogic
{
public:
    /// Destructor.
    virtual ~MSJunctionLogic();

    /// Modifies the passed respond according to the request.
    virtual void respond( const MSLogicJunction::Request& request,
                          MSLogicJunction::Respond& respond ) const = 0;
                          
    /// Returns the logic's number of links.
    unsigned int nLinks();
    
    /// Returns the logic's number of inLanes.
    unsigned int nInLanes();
    
    /** Inserts MSJunctionLogic into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSJunctionLogic* junction );

    /** Returns the MSJunctionLogic associated to the key id if exists,
        otherwise returns 0. */
    static MSJunctionLogic* dictionary( std::string id);
    
protected:
    /// Constructor.
    MSJunctionLogic( unsigned int nLinks, unsigned int nInLanes );
    
    /// The logic's number of links.
    unsigned int myNLinks;
    
    /// The logic's number of inLanes.
    unsigned int myNInLanes;
    
private:
    /** Static dictionary to associate string-ids with objects. */
    typedef std::map< std::string, MSJunctionLogic* > DictType;
    static DictType myDict;
    
    /// Default constructor.
    MSJunctionLogic();
    
    /// Copy constructor.
    MSJunctionLogic(const MSJunctionLogic&);
    
    /// Assignment operator.
    MSJunctionLogic& operator=(const MSJunctionLogic&);     
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSJunctionLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










