/****************************************************************************/
/// @file    MSJunctionControl.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Junction-operations.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSJunctionControl_h
#define MSJunctionControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>

#include <utils/common/NamedObjectCont.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunctionControl
 */
class MSJunctionControl : public NamedObjectCont<MSJunction*>
{
public:
    /// Use this constructor only.
    MSJunctionControl();

    /// Destructor.
    ~MSJunctionControl();

    /** resets the requests for all lanes */
    void resetRequests();

    /// Sets the responds
    void setAllowed();

    void postloadInitContainer();

private:
    /// Copy constructor.
    MSJunctionControl(const MSJunctionControl&);

    /// Assignment operator.
    MSJunctionControl& operator=(const MSJunctionControl&);

};


#endif

/****************************************************************************/

