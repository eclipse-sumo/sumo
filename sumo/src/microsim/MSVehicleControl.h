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
// Revision 1.7  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:35:40  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.4  2004/11/23 10:20:11  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.3  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.2  2004/04/02 11:36:28  dkrajzew
// "compute or not"-structure added; added two further simulation-wide output
//  (emission-stats and single vehicle trip-infos)
//
// Revision 1.1  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
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

#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSRoute;
class MSVehicleType;
class BinaryInputDevice;


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
    MSVehicleControl();

    /// Destructor
    virtual ~MSVehicleControl();

    /// Builds a vehicle
    virtual MSVehicle *buildVehicle(std::string id, MSRoute* route,
        SUMOTime departTime, const MSVehicleType* type,
        int repNo, int repOffset);

    /// Builds a vehicle in the case his color is given
    virtual MSVehicle *buildVehicle(std::string id, MSRoute* route,
        SUMOTime departTime, const MSVehicleType* type,
        int repNo, int repOffset, const RGBColor &col);

    /// Removes the vehicle
    void scheduleVehicleRemoval(MSVehicle *v);

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

    /// Returns the number of vehicles that wait for being emitted
    size_t getWaitingVehicleNo() const;

    /// Return the meaning waiting time of vehicles (corn-dependent value)
    double getMeanWaitingTime() const;

    /// Return the meaning waiting time of vehicles (corn-dependent value)
    double getMeanTravelTime() const;

    /// Informs this instance about the successfull emission of a vehicle
    void vehiclesEmitted(size_t no=1);

    /// Returns the information whether all build vehicles have been removed
    bool haveAllVehiclesQuit() const;

    /** @brief Informs this control about a vehicle's emission (corn-dependent value)
        Normally, this is done "in a batch" as the number of emitted vehicles
        is given and no explicite information about s single vehicle's emission
        is needed.
        Still, we do need this if we want to compute the mean waiting time. */
    virtual void vehicleEmitted(MSVehicle *v);
    virtual void vehicleMoves(MSVehicle *v);

    void saveState(std::ostream &os, long what);
    void loadState(BinaryInputDevice &bis, long what);


protected:
    virtual void removeVehicle(MSVehicle *v);

protected:
    /// The number of build vehicles
    unsigned int myLoadedVehNo;

    /// The number of emitted vehicles
    unsigned int myEmittedVehNo;

    /// The number of vehicles within the network (build and emitted but not removed)
    unsigned int myRunningVehNo;

    /// The number of removed vehicles
    unsigned int myEndedVehNo;

    /// The aggregated time vehicles had to wait for departure
    long myAbsVehWaitingTime;

    /// The aggregated time vehicles needed to aacomplish their route
    long myAbsVehTravelTime;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
