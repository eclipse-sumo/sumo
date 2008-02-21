/****************************************************************************/
/// @file    MSDevice_CPhone.h
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// A cellular phone device
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
#ifndef MSDevice_CPhone_h
#define MSDevice_CPhone_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <math.h>
#include <utils/common/WrappingCommand.h>
#include "MSDevice.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_CPhone
 * @brief A cellular phone device
 *
 * Class in order to expand the MSVehicle-class concerning the availability
 *  and the usage state of cellphones carried along.
 */
class MSDevice_CPhone : public MSDevice
{
public:
    /** @brief Inserts MSDevice_CPhone-options
     */
    static void insertOptions() throw();


    /** @brief Build cellular phone devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether cphone-devices shall be built
     *  for the given vehicle.
     *
     * For each seen vehicle, the global vehicle index is increased.
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which devices may be built
     * @param[in, filled] into The vector to store the built device(s) in
     * @todo Recheck setting the number of devices in dependence of the vehicle type
     */
    static void buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw();


public:
    /** @enum State
     * @brief The cellular phone's state
     */
    enum State {
        /// @brief The phone is switched off
        STATE_OFF,
        /// @brief The phone is in idle mode
        STATE_IDLE,
        /// @brief The is called
        STATE_CONNECTED_IN,
        /// @brief The calls
        STATE_CONNECTED_OUT
    };


    /** @struct CPhoneBroadcastCell
     * @brief Information about one of the cells the phone knows
     */
    struct CPhoneBroadcastCell {
        /// @brief The ID of the cell
        int m_CellID;
        /// @brief The cell's level-of-service
        int m_LoS;
    };


    State GetState() const;
    int SetState(int ActualState);
    int SetState(State s, int Duration);
    int GetCallCellCount() {
        return myCallCellCount;
    }
    void IncCallCellCount() {
        ++myCallCellCount;
    }
    void setCurrentCellId(unsigned int id) {
        mycurrentCellId = id;
    };
    int getCurrentCellId() {
        return mycurrentCellId;
    };
    void setCurrentLAId(unsigned int id) {
        mycurrentLAId = id;
    };
    int getCurrentLAId() {
        return mycurrentLAId;
    };
    int getCallId() {
        return myCallId;
    };

    void setNotTriggeredByCell() {
        notTriggeredByCell = true;
    };

    bool getNotTriggeredByCell() {
        return notTriggeredByCell;
    };

        SUMOTime stateChangeCommandExecution(SUMOTime currentTime) throw(ProcessError);


    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{
    /** @brief Update of members if vehicle enters a new lane in the emit step
     *
     * !!! describe
     *
     * @param[in] enteredLane The lane the vehicle enters (unused)
     * @param[in] state The vehicle's state during the emission (unused)
     */
    void enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &state);

    /// @}

private:
    /** @brief Constructor
     * 
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_CPhone(MSVehicle &vehicle, const std::string &id) throw();


    /// @brief Destructor.
    ~MSDevice_CPhone() throw();




private:
    /// @brief The phone's state
    State m_State;

    //the best 6 of the available broadcast cells; index "0" represents the actual serving cell
    std::vector<CPhoneBroadcastCell> m_ProvidedCells;

    static int gCallID;
    int myCallId;
    WrappingCommand< MSDevice_CPhone > *myCommand;
    static int myVehicleIndex;

    /*this id reminds the cell-id the phone is currently in*/
    /*if it is -1 the car still not cross a cellborder*/
    int mycurrentCellId;
    int mycurrentLAId;

    /*if the device is calling,it counts the passed cells*/
    int myCallCellCount;

    /*if this */
    bool notTriggeredByCell;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_CPhone(const MSDevice_CPhone&);

    /// @brief Invalidated assignment operator.
    MSDevice_CPhone& operator=(const MSDevice_CPhone&);

};


#endif

/****************************************************************************/

