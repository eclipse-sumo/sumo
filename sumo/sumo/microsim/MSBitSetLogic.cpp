/***************************************************************************
                          MSBitSetLogic.cpp  -  Logic based on std::bitset.
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
// Revision 1.6  2002/06/18 18:29:07  croessel
// Added #ifdef EXTERNAL_TEMPLATE_DEFINITION to prevent multiple inclusions.
//
// Revision 1.5  2002/06/18 16:41:50  croessel
// Re-addition of this file. Is now cfront-compliant.
//
// Revision 1.3  2002/04/11 11:33:56  croessel
// in respond(): changed if with &&.
//
// Revision 1.2  2002/04/11 11:30:29  croessel
// in respond(): Perform response-calculation for set-responds only.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.2  2002/03/06 11:04:17  croessel
// Assert added to be sure that passed respond has the correct size.
//
// Revision 2.1  2002/02/21 19:50:22  croessel
// MSVC++ Linking-problems solved, hopefully.
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/01/31 14:04:28  croessel
// Changed construtor logic- and transform-parameters to pointer-type.
//
// Revision 1.4  2002/01/31 14:01:52  croessel
// Pleased MSVC++ who cannot handle declarations in for-loops.
//
// Revision 1.3  2002/01/30 14:45:48  croessel
// Destructor-definition added.
//
// Revision 1.2  2002/01/30 10:48:33  croessel
// Introduction of a fixed size MSBitSetLogic<64>=MSBitsetLogic
// (different spelling). Constructor and respond() needed slight
// modifications.
//
// Revision 1.1  2001/12/19 15:47:36  croessel
// Initial commit.
//

#ifdef EXTERNAL_TEMPLATE_DEFINITION
namespace
{
    const char rcsid[] =
    "$Id$";
}
#include "MSBitSetLogic.h"
#endif // EXTERNAL_TEMPLATE_DEFINITION

#include <bitset>
#include <vector>
#include <cassert>
#include "MSJunctionLogic.h"
#include "MSLogicJunction.h"



template< size_t N >
MSBitSetLogic< N >::MSBitSetLogic< N >( unsigned int nLinks,
                                        unsigned int nInLanes,
                                        Logic* logic,
                                        Link2LaneTrafo* transform ) :
    MSJunctionLogic( nLinks, nInLanes ),
    myLogic( logic ),
    myTransform( transform )
{
}

//-------------------------------------------------------------------------//

template< size_t N >
MSBitSetLogic< N >::~MSBitSetLogic< N >()
{
    ( *myLogic ).clear();
    ( *myTransform ).clear();
}
               
//-------------------------------------------------------------------------//
    
template< size_t N > void 
MSBitSetLogic< N >::respond( const MSLogicJunction::Request& request,
                             MSLogicJunction::Respond& respond ) const
{
    
    // convert request to bitset
    std::bitset< N > requestBS;
     unsigned int i = 0;   
    for ( ; i < myNLinks; ++i ) {
    
        requestBS.set( i, request[ i ] );
    } 
    
    // calculate respond
    std::bitset< N > respondBS;    
    
    for ( i = 0; i < myNLinks; ++i ) {

        bool linkPermit = requestBS.test( i ) &&
            ( requestBS & ( *myLogic )[ i ]).none();   
        respondBS.set( i, linkPermit ); 
    }
    
    // perform the link to lane transformation  
    assert( respond.size() == myNInLanes );
    for ( i = 0; i < myNInLanes; ++i ) {
    
        bool lanePermit = ( ( *myTransform)[ i ] & respondBS ).any();
        respond[ i ] = lanePermit;   
    }
    
    return;
}   

//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSLogicJunction.icc"
//#endif


// Local Variables:
// mode:C++
// End:







