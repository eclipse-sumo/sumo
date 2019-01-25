/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSJunctionLogic.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// kinds of logic-implementations.
/****************************************************************************/
#ifndef MSJunctionLogic_h
#define MSJunctionLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StdDefs.h>
#include "MSLogicJunction.h"
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunctionLogic
 */
class MSJunctionLogic {
public:
    /// Destructor.
    virtual ~MSJunctionLogic();

    /// Returns the logic's number of links.
    int nLinks();

    /// @brief Returns the response for the given link
    virtual const MSLogicJunction::LinkBits& getResponseFor(int linkIndex) const {
        UNUSED_PARAMETER(linkIndex);
        return myDummyFoes;
    }

    /// @brief Returns the foes for the given link
    virtual const MSLogicJunction::LinkBits& getFoesFor(int linkIndex) const {
        UNUSED_PARAMETER(linkIndex);
        return myDummyFoes;
    }
    virtual bool getIsCont(int linkIndex) const {
        UNUSED_PARAMETER(linkIndex);
        return false;
    }


    int getLogicSize() const {
        return myNLinks;
    }

    virtual bool hasFoes() const {
        return false;
    }




protected:
    /// Constructor.
    MSJunctionLogic(int nLinks);

    /// The logic's number of links.
    int myNLinks;

    /// @brief A dummy foe container
    static MSLogicJunction::LinkBits myDummyFoes;

private:
    /// Default constructor.
    MSJunctionLogic();

    /// Copy constructor.
    MSJunctionLogic(const MSJunctionLogic&);

    /// Assignment operator.
    MSJunctionLogic& operator=(const MSJunctionLogic&);

};


#endif

/****************************************************************************/

