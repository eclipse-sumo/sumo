/****************************************************************************/
/// @file    MSBitSetLogic.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// Container for holding a right-of-way matrix
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSBitSetLogic_h
#define MSBitSetLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <bitset>
#include <vector>
#include "MSJunctionLogic.h"
#include "MSLogicJunction.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBitSetLogic
 *
 * N is sum of the number of links of the junction's inLanes.
 */
template< int N >
class MSBitSetLogic : public MSJunctionLogic {
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
    MSBitSetLogic(int nLinks,
                  Logic* logic,
                  Foes* foes,
                  std::bitset<SUMO_MAX_CONNECTIONS> conts)
        : MSJunctionLogic(nLinks), myLogic(logic),
          myInternalLinksFoes(foes), myConts(conts) {}


    /// Destructor.
    ~MSBitSetLogic() {
        delete myLogic;
        delete myInternalLinksFoes;
    }


    /// @brief Returns the response for the given link
    const MSLogicJunction::LinkBits& getResponseFor(int linkIndex) const {
        return (*myLogic)[linkIndex];
    }

    /// @brief Returns the foes for the given link
    const MSLogicJunction::LinkBits& getFoesFor(int linkIndex) const {
        return (*myInternalLinksFoes)[linkIndex];
    }

    bool getIsCont(int linkIndex) const {
        return myConts.test(linkIndex);
    }

    virtual bool hasFoes() const {
        for (typename Logic::const_iterator i = myLogic->begin(); i != myLogic->end(); ++i) {
            if ((*i).any()) {
                return true;
            }
        }
        return false;
    }

private:
    /// junctions logic based on std::bitset
    Logic* myLogic;

    /// internal lanes logic
    Foes* myInternalLinksFoes;

    std::bitset<SUMO_MAX_CONNECTIONS> myConts;

private:
    /// @brief Invalidated copy constructor.
    MSBitSetLogic(const MSBitSetLogic&);

    /// @brief Invalidated assignment operator.
    MSBitSetLogic& operator=(const MSBitSetLogic&);

};


/** To make things easier we use a fixed size. SUMO_MAX_CONNECTIONS will hopefully be sufficient even for
    large asian junctions.
    So, here comes the type which should be used by the netbuilder. */
typedef MSBitSetLogic<SUMO_MAX_CONNECTIONS> MSBitsetLogic;


#endif

/****************************************************************************/

