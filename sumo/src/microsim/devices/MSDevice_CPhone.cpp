//---------------------------------------------------------------------------


#pragma hdrstop

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

using namespace std;


int MSDevice_CPhone::gCallID = 0; // !!! reinit on simulation reload

/* -------------------------------------------------------------------------
 * MSDevice_CPhone::Command-methods
 * ----------------------------------------------------------------------- */
MSDevice_CPhone::MyCommand::MyCommand(MSDevice_CPhone &parent)
    : myParent(parent), myAmActive(true)
{
}


MSDevice_CPhone::MyCommand::~MyCommand( void )
{
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
}

//---------------------------------------------------------------------------

MSDevice_CPhone::~MSDevice_CPhone()
{
    if(myCommand!=0) {
        myCommand->setInactivated();
    }
    vector<CPhoneBroadcastCell*>::iterator i;
    for(i=m_ProvidedCells.begin(); i!=m_ProvidedCells.end(); ++i) {
        delete *i;
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

const vector<MSDevice_CPhone::CPhoneBroadcastCell*> &
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
MSDevice_CPhone::SetProvidedCells(const vector<MSDevice_CPhone::CPhoneBroadcastCell*> &ActualCells)
{
    if(m_State == 0 || m_State == STATE_OFF)
        return 1;
    else {
        for(int i=0;i<7;i++) {
            m_ProvidedCells[i]->m_CellID = ActualCells[i]->m_CellID;
            m_ProvidedCells[i]->m_LoS = ActualCells[i]->m_LoS;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
// the state of the cellphone can only be set if one is available,
// an existing cellphone cannot be set "non-existent"
/*
int
MSDevice_CPhone::SetState(int ActualState)
{
  if(m_State != 0 && ActualState != 0)
  {
    m_State = ActualState;
    return 0;
  }
  return 1;
}
*/
//---------------------------------------------------------------------------

SUMOTime
MSDevice_CPhone::changeState()
{
    SUMOTime next;
    SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
    switch(m_State) {
    case STATE_OFF:
        {
            if(r1>0.5) {
                // some people wait for a call
                m_State = STATE_IDLE;
                next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
            } else {
                // some start telephoning
                m_State = STATE_CONNECTED;
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
                m_State = STATE_CONNECTED;
                next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // telephone some seconds
                gCallID++;
            }
        }
        break;
    case STATE_CONNECTED:
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
    default:
        throw 1;
    }
    // TOL_SPEC_SS2 (1.2)
    if(m_State==STATE_CONNECTED&&MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2)) {
        MSCORN::saveTOSS2_CalledPositionData(
            MSNet::getInstance()->getCurrentTimeStep(), gCallID,
            myVehicle.getLane().edge().getID(), 0); // !!! recheck quality indicator
    }
    return next;
}

void
MSDevice_CPhone::onDepart()
{
    SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
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
        m_State = STATE_CONNECTED;
        t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // stop telephoning after some time
        gCallID++;
    }
    for(int i=0; i<7; i++) {
        CPhoneBroadcastCell* TempCell = new CPhoneBroadcastCell;
        TempCell->m_CellID = 0;
        TempCell->m_LoS = -1;
        m_ProvidedCells.push_back(TempCell);
    }
    // TOL_SPEC_SS2 (1.2)
    if(m_State==STATE_CONNECTED&&MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2)) {
        MSCORN::saveTOSS2_CalledPositionData(
            MSNet::getInstance()->getCurrentTimeStep(), gCallID,
            myVehicle.getLane().edge().getID(), 0); // !!! recheck quality indicator
    }
    //
    myCommand = new MyCommand(*this);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        myCommand, t1, MSEventControl::ADAPT_AFTER_EXECUTION);
}


//#pragma package(smart_init)
