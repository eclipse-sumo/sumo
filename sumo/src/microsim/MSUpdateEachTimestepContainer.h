/****************************************************************************/
/// @file    MSUpdateEachTimestepContainer.h
/// @author  Christian Roessel
/// @date    Thu Oct 23 16:38:35 2003
/// @version $Id$
///
// Container of typed objects that shall be updated in each simulation step
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
#ifndef MSUpdateEachTimestepContainer_h
#define MSUpdateEachTimestepContainer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSUpdateEachTimestepContainer
 * @brief Container of typed objects that shall be updated in each simulation step
 */
template< class UpdateEachTimestep >
class MSUpdateEachTimestepContainer
{
public:
    /** @brief Singleton instance retriever
     * @return The instance of this singleton
     */
    static MSUpdateEachTimestepContainer* getInstance() throw() {
        if (myInstance == 0) {
            myInstance = new MSUpdateEachTimestepContainer();
        }
        return myInstance;
    }


    /** @brief Adds an item that shall be updated in each time step
     * @patam[in] item The item to add
     */
    void addItemToUpdate(UpdateEachTimestep* item) throw() {
        myContainer.push_back(item);
    }


    /** @brief Removes an item 
     * @patam[in] item The item to remove
     */
    void removeItemToUpdate(UpdateEachTimestep* item) throw() {
        typename std::vector< UpdateEachTimestep* >::iterator i =
            std::find(myContainer.begin(), myContainer.end(), item);
        if (i!=myContainer.end()) {
            myContainer.erase(i);
        }
    }


    /** @brief Updates all items
     *
     * Calls updateEachTimestep on all stored items
     * @see MSUpdateEachTimestep::updateEachTimestep
     */
    void updateAll() {
        std::for_each(myContainer.begin(), myContainer.end(),
                      std::mem_fun(&UpdateEachTimestep::updateEachTimestep));
    }


    /// @brief Destructor
    ~MSUpdateEachTimestepContainer() throw() {
        myContainer.clear();
        myInstance = 0;
    }


    /** @brief Clears the container
     *
     * All items will be destructed
     */
    void clear() throw() {
        for (typename std::vector< UpdateEachTimestep* >::iterator i=myContainer.begin(); i!=myContainer.end(); ++i) {
            delete(*i);
        }
        myContainer.clear();
    }


private:
    /// @brief Constructor
    MSUpdateEachTimestepContainer()
            : myContainer() {}


    /// @brief The container of items that shall be updated
    std::vector< UpdateEachTimestep* > myContainer;

    /// @brief The singleton instance
    static MSUpdateEachTimestepContainer* myInstance;


};

// initialize static member
template< class UpdateEachTimestep >
MSUpdateEachTimestepContainer< UpdateEachTimestep >*
MSUpdateEachTimestepContainer< UpdateEachTimestep >::myInstance = 0;


#endif

/****************************************************************************/

