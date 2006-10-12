/***************************************************************************
                          MSDevice_CPhone.cpp  -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.14  2006/10/12 14:50:37  ericnicolay
// add code for the ss2-sql-output
//
// Revision 1.13  2006/09/18 10:02:55  dkrajzew
// documentation added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma hdrstop


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

#include "MSDevice_CPhone.h"
#include "../MSEventControl.h"
#include "../MSCORN.h"
#include "../MSNet.h"
#include "../MSVehicle.h"
#include "../MSLane.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static variables
 * ======================================================================= */
int MSDevice_CPhone::gCallID = 0; // !!! reinit on simulation reload


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * MSDevice_CPhone::Command-methods
 * ----------------------------------------------------------------------- */
MSDevice_CPhone::MyCommand::MyCommand(MSDevice_CPhone &parent)
    : myParent(parent), myAmActive(true)
{
}


MSDevice_CPhone::MyCommand::~MyCommand( void )
{
	if(myAmActive)
		myParent.invalidateCommand();
}


SUMOTime
MSDevice_CPhone::MyCommand::execute(SUMOTime currentTime)
{
    SUMOTime ret = 0;
    if(myAmActive) {
        ret = myParent.changeState();
    } else {
        // inactivated -> the cell phone is not longer simulated
        return 0;
    }
    // assert that this device is not removed from the event handler
    if(ret==0) {
        ret = 1;
    }
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
MSDevice_CPhone::MSDevice_CPhone(MSVehicle &vehicle)
    : myVehicle(vehicle), myCommand(0)
{
	myId = "";
	mycurrentCellId = -1;
}

//---------------------------------------------------------------------------

MSDevice_CPhone::~MSDevice_CPhone()
{
	/*if registered to a cell then deregist from it*/
	if ( mycurrentCellId != -1 ){
		MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
		if ( pPhone != 0 ){
			MSPhoneCell * cell = pPhone->getcurrentVehicleCell( myVehicle.getID() );
			if ( cell != 0 )
				cell->remCall( myVehicle.getID() );
		}
	}
    if(myCommand!=0) {
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
if(m_State == 0 || m_State == STATE_OFF)
return 1;
else {
for(int i=0;i<7;i++) {
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
if(m_State != 0 && ActualState != 0)
{
	m_State = (State)ActualState;
return 0;
}
return 1;
}

//---------------------------------------------------------------------------

SUMOTime
MSDevice_CPhone::changeState()
{
    SUMOTime next;
    SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
	SUMOReal r2 = rand()/(SUMOReal) RAND_MAX;
    switch(m_State) {
	case STATE_OFF:
		{
			if(r1>0.5) {
				// some people wait for a call
				m_State = STATE_IDLE;
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
			} else {
				// some start telephoning
				if ( r2 > 0.5 )
					m_State = STATE_CONNECTED_IN;
				else
					m_State = STATE_CONNECTED_OUT;

				if ( mycurrentCellId != -1 ){
					MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
					MSPhoneCell * cell = pPhone->getMSPhoneCell( mycurrentCellId );
					if ( m_State == STATE_CONNECTED_IN )
						cell->addCall( myVehicle.getID(), STATICIN );
					else
						cell->addCall( myVehicle.getID(), STATICOUT );
				}
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // telephone some seconds
				gCallID++;
			}
		}
		break;
	case STATE_IDLE:
		{
			if(r1>0.8) {
				// some people switch off
				m_State = STATE_OFF;
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // keep it off
			} else {
				// most start telephoning
				if ( r2 > 0.5 )
					m_State = STATE_CONNECTED_IN;
				else
					m_State = STATE_CONNECTED_OUT;

				if ( mycurrentCellId != -1 ){
					MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
					MSPhoneCell * cell = pPhone->getMSPhoneCell( mycurrentCellId );
					if ( m_State == STATE_CONNECTED_IN )
						cell->addCall( myVehicle.getID(), STATICIN );
					else
						cell->addCall( myVehicle.getID(), STATICOUT );
				}
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // telephone some seconds
				gCallID++;
			}
		}
		break;
	case STATE_CONNECTED_IN:
		{
			if(r1>0.1) {
				// most people stop telephonig
				m_State = STATE_IDLE;
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
			} else {
				// some switch off
				m_State = STATE_OFF;
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // keep it off
			}
		}
		break;
	case STATE_CONNECTED_OUT:
		{
			if(r1>0.1) {// most people stop telephonig
				m_State = STATE_IDLE;
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
			} else {// some switch off
				m_State = STATE_OFF;
				next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // keep it off
			}
		}
		break;
	default:
		throw 1;
	}
	// TOL_SPEC_SS2 (1.2)
	if((m_State==STATE_CONNECTED_IN||m_State==STATE_CONNECTED_OUT)&&MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2)) {
		MSCORN::saveTOSS2_CalledPositionData(
			MSNet::getInstance()->getCurrentTimeStep(), gCallID,
			myVehicle.getLane().getEdge()->getID(), 0); // !!! recheck quality indicator
	}
	if((m_State==STATE_CONNECTED_IN||m_State==STATE_CONNECTED_OUT)&&MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2_SQL)){
		MSCORN::saveTOSS2SQL_CalledPositionData(
			MSNet::getInstance()->getCurrentTimeStep(), gCallID,
			myVehicle.getLane().getEdge()->getID(), 0); // !!! recheck quality indicator
	}
	return next;
}

void
MSDevice_CPhone::onDepart()
{
    SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
	SUMOReal r2 = rand()/(SUMOReal) RAND_MAX;
    SUMOTime t1;
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
				cell->addCall( myVehicle.getID(), STATICIN );
			else
				cell->addCall( myVehicle.getID(), STATICOUT );
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
	// TOL_SPEC_SS2 (1.2)
	if(m_State==STATE_CONNECTED_IN||m_State==STATE_CONNECTED_OUT){
		if(MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2)){
			MSCORN::saveTOSS2_CalledPositionData(
            MSNet::getInstance()->getCurrentTimeStep(), gCallID,
            myVehicle.getLane().getEdge()->getID(), 0); // !!! recheck quality indicator
		}
		else if(MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2_SQL)){
			MSCORN::saveTOSS2SQL_CalledPositionData(
            MSNet::getInstance()->getCurrentTimeStep(), gCallID,
            myVehicle.getLane().getEdge()->getID(), 0); // !!! recheck quality indicator
		}
    }
    //
    myCommand = new MyCommand(*this);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
		myCommand, t1 + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
}


void
MSDevice_CPhone::invalidateCommand(){
	this->myCommand = 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

