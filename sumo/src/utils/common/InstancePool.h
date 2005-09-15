#ifndef InstancePool_h
#define InstancePool_h
//---------------------------------------------------------------------------//
//                        InstancePool.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.5  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include <cassert>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<typename T>
class InstancePool {
public:
    InstancePool(bool deleteOnQuit) { }
    ~InstancePool() {
	typedef typename std::vector<T*>::iterator It;
	if(myDeleteOnQuit) {
	    for(It i=myFreeInstances.begin(); i!=myFreeInstances.end(); i++) {
		delete *i;
	    }
	}
    }

    T* getFreeInstance() {
	if(myFreeInstances.size()==0) {
	    return 0;
	} else {
	    T *instance = myFreeInstances.back();
	    myFreeInstances.pop_back();
	    return instance;
	}
    }

    void addFreeInstance(T *instance) {
	myFreeInstances.push_back(instance);
    }

    void addFreeInstances(const std::vector<T*> instances) {
	std::copy(instances.begin(), instances.end(),
		  std::back_inserter(myFreeInstances));
    }


private:
    std::vector<T*> myFreeInstances;
    bool myDeleteOnQuit;
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
