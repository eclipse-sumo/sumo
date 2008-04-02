/****************************************************************************/
/// @file    MSTriggerControl.h
/// @author  Daniel Krajzewicz
/// @date    2006
/// @version $Id$
///
// A storage for triggers
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
#ifndef MSTriggerControl_h
#define MSTriggerControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrigger;



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTriggerControl
 * @brief A simple container for "triggers".
 *
 * Please note that this control takes care of deletion of added triggers.
 * That's basically all why this control is here. Retrieval is only done
 *  for bus-stops.
 *
 * @see MSTrigger
 */
class MSTriggerControl
{
public:
    /** @brief Constructor
     */
    MSTriggerControl() throw();


    /// @brief Destructor
    ~MSTriggerControl() throw();


    /** @brief Adds a trigger
     * 
     * Stored the trigger in "myTrigger".
     *
     * @param[in] t The trigger to add
     */
    void addTrigger(MSTrigger *t) throw();


    /** @brief Returns the named trigger
     * 
     * Searched for the name in "myTrigger". Returns the named trigger if
     *  its id matches the given. Returns 0 if no matching trigger could be 
     *  found.
     *
     * @param[in] t The trigger to add
     * @todo This may be quite insecure as several triggers may have the same name
     * @todo Searching in the list is not very fast...
     */
    MSTrigger *getTrigger(const std::string &id) throw();


protected:
    /// @brief Definition of a trigger vector
    typedef std::vector<MSTrigger*> TriggerVector;

    /// @brief The stored triggers
    TriggerVector myTrigger;


private:
    /// @brief Invalidated copy constructor.
    MSTriggerControl(const MSTriggerControl&);

    /// @brief Invalidated assignment operator.
    MSTriggerControl& operator=(const MSTriggerControl&);


};


#endif

/****************************************************************************/

