#ifndef MSRouteLoaderControl_h
#define MSRouteLoaderControl_h
/***************************************************************************
                          MSRouteLoaderControl.h
    Class responsible for loading of routes from some files
                             -------------------
    begin                : Wed, 06 Nov 2002
    copyright            : (C) 2001 by IVF/DLR (http://ivf.dlr.de)
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.2  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <helpers/PreStartInitialised.h>
#include "MSVehicleContainer.h"
#include "MSNet.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSRouteLoader;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSRouteLoaderControl
 * This controls is initialised with the list of route loaders and uses them
 * to load routes step wise.
 * The parameter myInAdvanceStepNo holds the number of time steps to read the
 * routes in forward. If it is 0 (default), all routes will be read at once.
 */
class MSRouteLoaderControl : public PreStartInitialised {
public:
    /// definition of the loader vector
    typedef std::vector<MSRouteLoader*> LoaderVector;

public:
    /// constructor
    MSRouteLoaderControl(MSNet &net, int inAdvanceStepNo,
        LoaderVector loader);

    /// destructor
    ~MSRouteLoaderControl();

    /// loads the next routes
    MSVehicleContainer &loadNext(SUMOTime step);

    /** reinitialises the loader for reading
        (implemented from PreStartInitialised:
            at every simulation-reentrance, the first,
            possibly destroyed, vehicles must be read) */
    void init(MSNet &net);

public:
    /// the last time step new routes were loaded
    int myLastLoadTime;

    /// the number of routes to read in forward
    size_t myInAdvanceStepNo;

    /// the list of route loaders
    LoaderVector myRouteLoaders;

    /// the list of loaded vehicles
    MSVehicleContainer myVehCont;

    /** information whether all routes shall be loaded and whether
        they were loaded */
    bool myLoadAll, myAllLoaded;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

