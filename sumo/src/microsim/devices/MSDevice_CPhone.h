#ifndef MSDevice_CPhoneH
#define MSDevice_CPhoneH
/***************************************************************************
                          MSDevice_CPhone.h  -
    A cellular phone device
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by DLR http://www.dlr.de
    author               : Eric Nicolay
    email                : Eric.Nicolay@dlr.de
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
// Revision 1.12  2006/11/28 12:15:41  dkrajzew
// documented TOL-classes and made them faster
//
// Revision 1.11  2006/11/08 17:25:46  ericnicolay
// add getCallId
//
// Revision 1.10  2006/09/18 11:34:20  dkrajzew
// debugged building on Windows
//
// Revision 1.9  2006/09/18 10:02:55  dkrajzew
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utils/helpers/Command.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSDevice_CPhone
 * @brief A cellular phone device
 *
 * class in order to expand the MSVehicle-class concerning the availability
 *  and the usage state of cellphones carried along.
 */
class MSDevice_CPhone {
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

    MSDevice_CPhone(MSVehicle &vehicle);
    ~MSDevice_CPhone();
    const std::vector<CPhoneBroadcastCell> &GetProvidedCells() const;
    State GetState() const;
    int SetProvidedCells(const std::vector<CPhoneBroadcastCell> &ActualCells);
    int SetState(int ActualState);
    SUMOTime changeState();
	void setCurrentCellId( unsigned int id ){ mycurrentCellId = id; };
	int getCurrentCellId(){return mycurrentCellId;};
    void onDepart();
	void setId(std::string id){myId=id;};
	int getCallId(){return gCallID;};
	std::string getId(){return myId;};
	void invalidateCommand();

protected:
    class MyCommand : public Command
    {
    public:
        /// Constructor
        MyCommand(MSDevice_CPhone &parent);

        /// virtual destructor
        virtual ~MyCommand( void );

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
	int callid;
    MyCommand *myCommand;

	/*this id reminds the cell-id the phone is currently in*/
	/*if it is -1 the car still not cross a cellborder*/
	int mycurrentCellId;
	int mycurrentLAId;
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

