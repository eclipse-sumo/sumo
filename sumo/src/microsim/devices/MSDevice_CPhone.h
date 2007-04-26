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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/helpers/Command.h>


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
 * class in order to expand the MSVehicle-class concerning the availability
 *  and the usage state of cellphones carried along.
 */
class MSDevice_CPhone
{
public:
    enum State {
        STATE_OFF,
        STATE_IDLE,
        STATE_CONNECTED_IN,
        STATE_CONNECTED_OUT
    };

    struct CPhoneBroadcastCell
    {
        int m_CellID;
        int m_LoS;          //Level of Service
    };

    MSDevice_CPhone(MSVehicle &vehicle, const std::string &id);
    ~MSDevice_CPhone();
    const std::vector<CPhoneBroadcastCell> &GetProvidedCells() const;
    State GetState() const;
    int SetProvidedCells(const std::vector<CPhoneBroadcastCell> &ActualCells);
    int SetState(int ActualState);
    int SetState(State s, int Duration);
    SUMOTime changeState();
    void setCurrentCellId(unsigned int id)
    {
        mycurrentCellId = id;
    };
    int getCurrentCellId()
    {
        return mycurrentCellId;
    };
    void setCurrentLAId(unsigned int id)
    {
        mycurrentLAId = id;
    };
    int getCurrentLAId()
    {
        return mycurrentLAId;
    };
    void onDepart();
    int getCallId()
    {
        return myCallId;
    };
    std::string getID()
    {
        return myId;
    };
   
    void invalidateCommand();

protected:
class MyCommand : public Command
    {
    public:
        /// Constructor
        MyCommand(MSDevice_CPhone &parent);

        /// virtual destructor
        virtual ~MyCommand(void);

        /** Execute the command and return an offset for recurring commands
            or 0 for single-execution commands. */
        virtual SUMOTime execute(SUMOTime currentTime);

        void setInactivated();

    private:
        /// The parent reader
        MSDevice_CPhone &myParent;

        bool myAmActive;

    };

private:
    std::string myId;
    //the State the cellphone (if available) is in
    //0: no cellphone; 1: turned off; 2: idle ; 3: connected
    int m_PhoneCount;
    State m_State;

    //the best 6 of the available broadcast cells; index "0" represents the actual serving cell
    std::vector<CPhoneBroadcastCell> m_ProvidedCells;
    MSVehicle &myVehicle;

    static int gCallID;
    int myCallId;
    MyCommand *myCommand;

    /*this id reminds the cell-id the phone is currently in*/
    /*if it is -1 the car still not cross a cellborder*/
    int mycurrentCellId;
    int mycurrentLAId;

};


#endif

/****************************************************************************/

