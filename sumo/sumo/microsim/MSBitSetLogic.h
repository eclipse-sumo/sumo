/***************************************************************************
                          MSBitSetLogic.h  -  Logic based on std::bitset.
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
// Revision 2.1  2002/02/21 19:50:22  croessel
// MSVC++ Linking-problems solved, hopefully.
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/01 11:18:26  croessel
// Moved all definitions from MSBitSetLogic.cpp to MSBitSetLogic.h due to
// template linker problems. Removed MSBitSetLogic.cpp.
//
// Revision 1.4  2002/01/31 14:04:28  croessel
// Changed construtor logic- and transform-parameters to pointer-type.
//
// Revision 1.3  2002/01/30 10:48:33  croessel
// Introduction of a fixed size MSBitSetLogic<64>=MSBitsetLogic
// (different spelling). Constructor and respond() needed slight
// modifications.
//
// Revision 1.2  2001/12/19 16:25:16  croessel
// Replaced using namespace std with std:: and style changes.
//
// Revision 1.1  2001/12/13 15:43:13  croessel
// Initial commit.
//

#ifndef MSBitSetLogic_H
#define MSBitSetLogic_H

#include <bitset>
#include <vector>
#include <cassert>
#include "MSJunctionLogic.h"
#include "MSLogicJunction.h"

/** N is sum of the number of links of the junction's inLanes.
 */
template< size_t N >
class MSBitSetLogic : public MSJunctionLogic
{
public:
    /// Destructor.
    ~MSBitSetLogic();

    /** Container that holds the right of way bitsets. Each link has it's own
        bitset. The bits in the bitsets correspond to the links. To create
        a bitset for a particular link, set the bits to true that correspond
        to links that have the right of way. All others set to false, 
        including the link's "own" link-bit. */
    typedef std::vector< std::bitset< N > > Logic;
    
    /** Container that matches links to lanes. Each inLane has a bitset with
        bits set to true for all links that belong to this lane. All other
        bits set to false. */
    typedef std::vector< std::bitset< N > > Link2LaneTrafo;    
                
    /// Use this constructor only.
    MSBitSetLogic( unsigned int nLinks,
                   unsigned int nInLanes,
                   Logic* logic,
                   Link2LaneTrafo* transform );
                
    /// Modifies the passed respond according to the request.    
    void respond( const MSLogicJunction::Request& request,
                  MSLogicJunction::Respond& respond ) const; 
                     
protected:

private:
    /// junctions logic based on std::bitset
    Logic* myLogic;
    
    /// tranformation from link to lane
    Link2LaneTrafo* myTransform;

    /// Default constructor.
    MSBitSetLogic();
    
    /// Copy constructor.
    MSBitSetLogic( const MSBitSetLogic& );
    
    /// Assignment operator.
    MSBitSetLogic& operator=( const MSBitSetLogic& );     
};


#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "MSBitSetLogic.cpp"
#endif // EXTERNAL_TEMPLATE_DEFINITION

/** To make things easier we use a fixed size. 64 will be sufficient even for
    pathological junctions. If it will consume to much space, reduce it to 32.
    So, here comes the type which should be used by the netbuilder. */
typedef MSBitSetLogic< 64 > MSBitsetLogic;


#endif // MSBitSetLogic_H

// Local Variables:
// mode:C++
// End:
