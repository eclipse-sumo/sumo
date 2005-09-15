#ifndef MSBitSetLogic_H
#define MSBitSetLogic_H
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
// Revision 1.6  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.4  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.3  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.4  2002/06/21 10:59:09  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.3  2002/06/18 16:39:21  croessel
// Moved code to cpp and made file cfront-compliant.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
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
/* =========================================================================
 * used namespaces
 * ======================================================================= */
// Replaced using namespace std with std:: and style changes.
//
// Revision 1.1  2001/12/13 15:43:13  croessel
// Initial commit.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <bitset>
#include <vector>
#include <cassert>
#include "MSJunctionLogic.h"
#include "MSLogicJunction.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/** N is sum of the number of links of the junction's inLanes.
 */
template< size_t N >
class MSBitSetLogic : public MSJunctionLogic
{
public:
    /** @brief Container that holds the right of way bitsets.
        Each link has it's own
        bitset. The bits in the bitsets correspond to the links. To create
        a bitset for a particular link, set the bits to true that correspond
        to links that have the right of way. All others set to false,
        including the link's "own" link-bit. */
    typedef std::vector< std::bitset< N > > Logic;

    /** @brief Container holding the information which internal lanes prohibt which links
        Build the same way as Logic */
    typedef std::vector< std::bitset< N > > Foes;


public:
    /// Use this constructor only.
    MSBitSetLogic( unsigned int nLinks,
                   unsigned int nInLanes,
                   Logic* logic,
                   Foes *foes)
    : MSJunctionLogic( nLinks, nInLanes ), myLogic( logic ),
        myInternalLinksFoes(foes)
    {
    }


    /// Destructor.
    ~MSBitSetLogic()
    {
        delete myLogic;
        delete myInternalLinksFoes;
    }


    /// Modifies the passed respond according to the request.
    void respond( const MSLogicJunction::Request& request,
        const MSLogicJunction::InnerState& innerState,
        MSLogicJunction::Respond& respond ) const
    {
        size_t i;
        // calculate respond
        for ( i = 0; i < myNLinks; ++i ) {
            bool linkPermit = request.test( i ) && ( request & ( *myLogic )[ i ]).none();
            respond.set( i, linkPermit );
        }
        // check whether internal lanes disallow any movement
        //  the number of internal lanes is equal to the number of links
        for ( i = 0; i < myNLinks; ++i ) {
            bool linkPermit = request.test( i ) && respond.test( i ) &&
                ( innerState & ( *myInternalLinksFoes )[ i ]).none();
            respond.set( i, linkPermit );
        }
}


private:
    /// junctions logic based on std::bitset
    Logic* myLogic;

    /// internal lanes logic
    Foes *myInternalLinksFoes;

private:
    /// Invalidated copy constructor.
    MSBitSetLogic( const MSBitSetLogic& );

    /// Invalidated assignment operator.
    MSBitSetLogic& operator=( const MSBitSetLogic& );

};


/** To make things easier we use a fixed size. 64 will be sufficient even for
    pathological junctions. If it will consume to much space, reduce it to 32.
    So, here comes the type which should be used by the netbuilder. */
typedef MSBitSetLogic< 64 > MSBitsetLogic;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

