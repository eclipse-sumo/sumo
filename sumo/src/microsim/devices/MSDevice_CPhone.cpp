/****************************************************************************/
/// @file    MSDevice_CPhone.cpp
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
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma hdrstop
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDevice_CPhone.h"
#include "../MSEventControl.h"
#include "../MSCORN.h"
#include "../MSNet.h"
#include "../MSVehicle.h"
#include "../MSLane.h"
#include "../MSPhoneCell.h"
#include "../MSPhoneLA.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static variables
// ===========================================================================
int MSDevice_CPhone::gCallID = 0; // !!! reinit on simulation reload


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
* MSDevice_CPhone::Command-methods
* ----------------------------------------------------------------------- */
MSDevice_CPhone::MyCommand::MyCommand(MSDevice_CPhone &parent)
        : myParent(parent), myAmActive(true)
{}


MSDevice_CPhone::MyCommand::~MyCommand(void)
{
    if (myAmActive)
        myParent.invalidateCommand();
}


SUMOTime
MSDevice_CPhone::MyCommand::execute(SUMOTime)
{
    SUMOTime ret = 0;
    if (myAmActive) {
        ret = myParent.changeState();
    } else {
        // inactivated -> the cell phone is not longer simulated
        return 0;
    }
    // assert that this device is not removed from the event handler
    //if(ret==0) {
    //  ret = 1;
    //}
    // return the time to next call
    return ret;
}


void
MSDevice_CPhone::MyCommand::setInactivated()
{
    myAmActive = false;
}


/* -------------------------------------------------------------------------
* MSDevice_CPhone-methods
* ----------------------------------------------------------------------- */
MSDevice_CPhone::MSDevice_CPhone(MSVehicle &vehicle, const std::string &id)
        : myVehicle(vehicle), myId(id), myCommand(0)
{
    mycurrentCellId = -1;
    mycurrentLAId = -1;
    myCallId = -1;
    m_State = STATE_IDLE;
    myCallCellCount = 0;
    if (myVehicle.getVehicleType().getID().compare("zug") == 0) {
        notTriggeredByCell = true;

    } else {
        notTriggeredByCell = false;
    }
}

//---------------------------------------------------------------------------

MSDevice_CPhone::~MSDevice_CPhone()
{
    /*if registered to a cell then deregist from it*/
    MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
    if (pPhone != 0) {
        MSPhoneCell * cell = pPhone->getCurrentVehicleCell(myId);
        if (cell != 0) {
            if (m_State!=STATE_IDLE && m_State!=STATE_OFF) {
                cell->remCall(myCallId);
                OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELLPHONE_DUMP_TO);
                if (od!=0) {
                    od->getOStream()
                    << MSNet::getInstance()->getCurrentTimeStep() << ';'
                    << myCallId << ';' << mycurrentCellId << ';'
                    << 2 << "\n";
                }
            }
            cell->remCPhone(myId);
        } /*else {
                                    assert(m_State==STATE_IDLE||m_State==STATE_OFF);
                                }*/
        MSPhoneLA * la = pPhone->getCurrentVehicleLA(myId);
        if (la != 0) {
            la->remCall(myId);
        }/* else {
                                    assert(m_State==STATE_IDLE||m_State==STATE_OFF);
                                }*/
    }
    if (myCommand!=0) {
        myCommand->setInactivated();
    }
    m_ProvidedCells.clear();
}

//---------------------------------------------------------------------------

const vector<MSDevice_CPhone::CPhoneBroadcastCell> &
MSDevice_CPhone::GetProvidedCells() const
{
    return m_ProvidedCells;
}

//---------------------------------------------------------------------------

MSDevice_CPhone::State
MSDevice_CPhone::GetState() const
{
    return m_State;
}

//---------------------------------------------------------------------------
// the actually provided cells can only be set if a cellphone is existent and in
// idle/connected mode (m_state=2||3); if it's OK, the function returns 0
int
MSDevice_CPhone::SetProvidedCells(const vector<MSDevice_CPhone::CPhoneBroadcastCell> &ActualCells)
{
    if (m_State == 0 || m_State == STATE_OFF)
        return 1;
    else {
        for (int i=0;i<7;i++) {
            m_ProvidedCells[i].m_CellID = ActualCells[i].m_CellID;
            m_ProvidedCells[i].m_LoS = ActualCells[i].m_LoS;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
// the state of the cellphone can only be set if one is available,
// an existing cellphone cannot be set "non-existent"

int
MSDevice_CPhone::SetState(int ActualState)
{
    if (m_State != 0 && ActualState != 0) {
        m_State = (State)ActualState;
        return 0;
    }
    return 1;
}

int
MSDevice_CPhone::SetState(State s, int dur)
{
    if (dur<1) dur = 1;
    assert(mycurrentCellId != -1);
    /*wenn s == dynin oder dynout starte ein neues gespraech.*/
    /*setze den gewuenschten status*/
    m_State = s;

    /*wird fuer das neue gespraech zurueck gesetzt*/
    myCallCellCount = 0;

    /*pruefe, dass das cphone wirklich eine aktuelle zelle hat.*/
    /*gib den cphone eine eindeutige verbindungsnummer*/
    myCallId = ++gCallID;
    MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
    MSPhoneCell * cell = pPhone->getMSPhoneCell(mycurrentCellId);
    if (m_State == STATE_CONNECTED_IN)
        cell->addCall(myCallId, DYNIN, myCallCellCount);
    else
        cell->addCall(myCallId, DYNOUT, myCallCellCount);

    /*erzeuge einen neuen Event, damit das cphone aufhoehrt zu telefonieren*/
    myCommand = new MyCommand(*this);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        myCommand, dur + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
    OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELLPHONE_DUMP_TO);
    if (od!=0) {
        od->getOStream()
        << MSNet::getInstance()->getCurrentTimeStep() << ';'
        << myCallId << ';' << mycurrentCellId << ';'
        << 0 << "\n";
    }
    return dur;
}
//---------------------------------------------------------------------------

/*stop the current call*/
SUMOTime
MSDevice_CPhone::changeState()
{
    assert(myCallId != -1);
    assert(mycurrentCellId != -1);

    OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELLPHONE_DUMP_TO);
    if (od!=0) {
        od->getOStream()
        << MSNet::getInstance()->getCurrentTimeStep() << ';'
        << myCallId << ';' << mycurrentCellId << ';'
        << 2 << "\n";
    }

    MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
    MSPhoneCell * cell = pPhone->getMSPhoneCell(mycurrentCellId);
    if (m_State == STATE_CONNECTED_IN || m_State == STATE_CONNECTED_OUT)
        cell->remCall(myCallId);

    myCallId = -1;
    m_State = STATE_IDLE;

    return (SUMOTime)0;
}

void
MSDevice_CPhone::onDepart()
{
    //SUMOTime t1 = 100;
    /*if( mycurrentCellId == -1 ){
        m_State = STATE_IDLE;
        t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // start telephoning after some time
    }else{
        SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
        SUMOReal r2 = rand()/(SUMOReal) RAND_MAX;
        if(r1<0.1) {
            // 10% are off
            m_State = STATE_OFF;
            t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // switch on after long time
        } else if(r1<0.2) {
            // 70% are idle
            m_State = STATE_IDLE;
            t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // start telephoning after some time
        } else {
            // 20% are connected
            if ( r2 > 0.5 )
                m_State = STATE_CONNECTED_IN;
            else
                m_State = STATE_CONNECTED_OUT;

            if ( mycurrentCellId != -1 ){
                MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
                MSPhoneCell * cell = pPhone->getMSPhoneCell( mycurrentCellId );
                if ( m_State == STATE_CONNECTED_IN )
                    cell->addCall( myId, STATICIN );
                else
                    cell->addCall( myId, STATICOUT );
            }

            t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // stop telephoning after some time
            gCallID++;
        }
        CPhoneBroadcastCell TempCell;
        for(int i=0;i<7;i++){
            TempCell.m_CellID = 0;
            TempCell.m_LoS = -1;
            m_ProvidedCells.push_back(TempCell);
        }
    }*/
    //myCommand = new MyCommand(*this);
    //MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
    //  myCommand, t1 + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
}


void
MSDevice_CPhone::invalidateCommand()
{
    this->myCommand = 0;
}



/****************************************************************************/

