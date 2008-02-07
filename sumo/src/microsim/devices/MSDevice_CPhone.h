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
#include <utils/common/Command.h>


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

    struct CPhoneBroadcastCell {
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
    int GetCallCellCount() {
        return myCallCellCount;
    }
    void IncCallCellCount() {
        ++myCallCellCount;
    }
    SUMOTime changeState();
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
    void onDepart();
    int getCallId() {
        return myCallId;
    };
    std::string getID() {
        return myID;
    };

    void invalidateCommand();

    void setNotTriggeredByCell() {
        notTriggeredByCell = true;
    };

    bool getNotTriggeredByCell() {
        return notTriggeredByCell;
    };

protected:
class MyCommand : public Command
    {
    public:
        /// Constructor
        MyCommand(MSDevice_CPhone &parent) throw();

        /// virtual destructor
        virtual ~MyCommand(void) throw();

        /** Execute the command and return an offset for recurring commands
            or 0 for single-execution commands. */
        virtual SUMOTime execute(SUMOTime currentTime) throw(ProcessError);

        void setInactivated();

    private:
        /// The parent reader
        MSDevice_CPhone &myParent;

        bool myAmActive;

    private:
        /// @brief Invalidated copy constructor.
        MyCommand(const MyCommand&);

        /// @brief Invalidated assignment operator.
        MyCommand& operator=(const MyCommand&);


    };

private:
    /*
        inline int getTrainDuration(void)
    {
        int duration=0;
        double randvalue1=0, randvalue2=0;randvalue2=0;
        while(randvalue2==0)
        {
            randvalue2=double(rand())/double(RAND_MAX);
        }
        //Bei 16 % der mobilen Werte gleichverteilte Werte zwischen 30 (minimal)
        //und 60 (maximale Häufigkeit => ab hier greift die Funktion) Sekunden
        if(randvalue2 < 0.16)
            duration=30+300*randvalue2;
        else
        {
            duration=0;
            //nur Werte über 60 Sekunden simulieren, da die anderen oben abgedeckt sind
            while (duration < 60) {
                randvalue1=0;
                while (randvalue1==0)
                {
                    randvalue1=double(rand())/double(RAND_MAX);
                }
                duration= (-1)*(235.583)*log(randvalue1)+9.2057;
            }
        }
    //        duration = 1000* duration;
        return duration;
    }
    */

    std::string myparentid;
    std::string myID;
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

