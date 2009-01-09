/****************************************************************************/
/// @file    MSJunctionControl.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Container for junctions; performs operations on all stored junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunctionControl
 * @brief Container for junctions; performs operations on all stored junctions
 */
class MSJunctionControl : public NamedObjectCont<MSJunction*>
{
public:
    /// @brief Constructor
    MSJunctionControl() throw();


    /// @brief Destructor
    ~MSJunctionControl() throw();


    /** @brief Closes building of junctions
     *
     * Calls "postloadInit" on all stored junctions.
     * @exception ProcessError From the called "postloadInit"
     * @see MSJunction::postloadInit
     */
    void postloadInitContainer() throw(ProcessError);


    /** @brief Resets the requests for all lanes
     *
     * Calls "clearRequests" for all stored junctions.
     * @see MSJunction::clearRequests
     */
    void resetRequests() throw();


    /** @brief Sets the responds
     *
     * Calls "setAllowed" for all stored junctions.
     * @see MSJunction::setAllowed
     */
    void setAllowed() throw();


private:
    /// @brief Invalidated copy constructor.
    MSJunctionControl(const MSJunctionControl&);

    /// @brief Invalidated assignment operator.
    MSJunctionControl& operator=(const MSJunctionControl&);

};


#endif

/****************************************************************************/

