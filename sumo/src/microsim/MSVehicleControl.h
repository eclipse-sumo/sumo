#ifndef MSVehicleControl_h
#define MSVehicleControl_h
/***************************************************************************
                          MSVehicleControl.h  -
    The class responsible for building and deletion of vehicles
                             -------------------
    begin                : Wed, 10. Dec 2003
    copyright            : (C) 2002 by DLR http://ivf.dlr.de
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
// Revision 1.1  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "MSNet.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSRoute;
class MSVehicleType;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSVehicleControl
 * This class is responsible for vehicle building and deletion. It was
 *  reinvented due to the handling of GUIVehicles and the different deletion
 *  modalities within the pure microsim and the gui version.
 * Use this class for the pure microsim and GUIVehicleControl within the gui.
 */
class MSVehicleControl {
public:
    /// Constructor
    MSVehicleControl(MSNet &net);

    /// Destructor
    virtual ~MSVehicleControl();

    /// Builds a vehicle
    virtual MSVehicle *buildVehicle(std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset);

    /// Builds a vehicle in the case his color is given
    virtual MSVehicle *buildVehicle(std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset, const RGBColor &col);

    /// Removes the vehicle
    virtual void scheduleVehicleRemoval(MSVehicle *v);

    /// Informs this instance about the existance of a new, not yet build vehicle
    void newUnbuildVehicleLoaded();

    /// Informs this instance the new vehicle was build
    void newUnbuildVehicleBuild();

    /// Returns the number of build vehicles
    size_t getLoadedVehicleNo() const;

    /// Returns the number of removed vehicles
    size_t getEndedVehicleNo() const;

    /// Returns the number of build and emitted, but not yet deleted vehicles
    size_t getRunningVehicleNo() const;

    /// Returns the number of emitted vehicles
    size_t getEmittedVehicleNo() const;

    /// Informs this instance about the successfull emission of a vehicle
    void vehiclesEmitted(size_t no=1);

    /// Returns the information whether all build vehicles have been removed
    bool haveAllVehiclesQuit() const;

protected:
    /// The number of build vehicles
	size_t myLoadedVehNo;

    /// The number of emitted vehicles
	size_t myEmittedVehNo;

    /// The number of vehicles within the network (build and emitted but not removed)
    size_t myRunningVehNo;

    /// The number of removed vehicles
	size_t myEndedVehNo;

    /// The network this instance belongs to
    MSNet &myNet;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#endif

#endif

// Local Variables:
// mode:C++
// End:
