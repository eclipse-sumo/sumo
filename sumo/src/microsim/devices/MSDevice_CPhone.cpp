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


double beginProbs[] = {
    0.0000000000,
    0.0000034488,
    0.0000000000,
    0.0000000000,
    0.0000000000,
    0.0000037179,
    0.0000034165,
    0.0000415533,
    0.0000867595,
    0.0001017927,
    0.0001178796,
    0.0001147367,
    0.0001046343,
    0.0001111894,
    0.0001006325,
    0.0000910834,
    0.0000870450,
    0.0000886780,
    0.0000914446,
    0.0000833619,
    0.0000829264,
    0.0000820729,
    0.0000865700,
    0.0000947382,
    0.0000923030,
    0.0001057666,
    0.0001035174,
    0.0001010719,
    0.0001025705,
    0.0000939282,
    0.0000857735,
    0.0000770837,
    0.0000656241,
    0.0000652935,
    0.0000693141,
    0.0000906244,
    0.0000885826,
    0.0000741091,
    0.0000662889,
    0.0000632351,
    0.0000529729,
    0.0000228884,
    0.0000174365,
    0.0000034186,
    0.0000000000,
    0.0000000000,
    0.0000000000,
    0.0000000000,
    /*

    0.000159335,
    0.000174396,
    0.000191955,
    0.000180723,
    0.000163997,
    0.00019457,
    0.000217754,
    0.00021592,
    0.000307402,
    0.000390569,
    0.0005094,
    0.000566064,
    0.000532274,
    0.000520971,
    0.000543689,
    0.000472037,
    0.000452854,
    0.000405372,
    0.000419775,
    0.000417058,
    0.000407825,
    0.000410568,
    0.000388599,
    0.00040962,
    0.000439665,
    0.000461603,
    0.000517258,
    0.000490517,
    0.000490263,
    0.000478685,
    0.000457273,
    0.000406477,
    0.000360701,
    0.000316793,
    0.000297329,
    0.000310726,
    0.000408869,
    0.00043672,
    0.00040814,
    0.000414152,
    0.000329344,
    0.000249434,
    0.000208789,
    0.000177732,
    0.000150707,
    0.000131785,
    0.000112191,
    0.000126129
    */
};


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include <utils/options/OptionsCont.h>


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

SUMOReal tolDefaultProb = -1;

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
* MSDevice_CPhone::Command-methods
* ----------------------------------------------------------------------- */
MSDevice_CPhone::MyCommand::MyCommand(MSDevice_CPhone &parent) throw()
        : myParent(parent), myAmActive(true)
{}


MSDevice_CPhone::MyCommand::~MyCommand() throw()
{
    if (myAmActive)
        myParent.invalidateCommand();
}


SUMOTime
MSDevice_CPhone::MyCommand::execute(SUMOTime) throw(ProcessError)
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
inline int getTrainDuration(void)
{
    int duration=0;
    double randvalue1=0, randvalue2=0;
    randvalue2=0;
    while (randvalue2==0) {
        randvalue2=double(rand())/double(RAND_MAX);
    }
    //Bei 16 % der mobilen Werte gleichverteilte Werte zwischen 30 (minimal)
    //und 60 (maximale Haeufigkeit => ab hier greift die Funktion) Sekunden
    if (randvalue2 < 0.16)
        duration=(int)(30+300*randvalue2);
    else {
        duration=0;
        //nur Werte ueber 60 Sekunden simulieren, da die anderen oben abgedeckt sind
        while (duration < 60) {
            randvalue1=0;
            while (randvalue1==0) {
                randvalue1=double(rand())/double(RAND_MAX);
            }
            duration= (int)(-235.583*log(randvalue1)+9.2057);
        }
    }
//        duration = 1000* duration;
    return duration;
}



MSDevice_CPhone::MSDevice_CPhone(MSVehicle &vehicle, const std::string &id)
        : myVehicle(vehicle), myID(id), myCommand(0)
{
    OptionsCont &oc = OptionsCont::getOptions();
    mycurrentCellId = -1;
    mycurrentLAId = -1;
    myCallId = -1;
    m_State = STATE_IDLE;
    myCallCellCount = 0;
    if (myVehicle.getVehicleType().getID().compare("SBahn") == 0) {
        notTriggeredByCell = true;
    } else if (myVehicle.getVehicleType().getID().substr(0, 3)=="PKW") {
        notTriggeredByCell = true;
    } else {
        notTriggeredByCell = false;
    }
    tolDefaultProb = oc.getFloat("cell-def-prob");
}

//---------------------------------------------------------------------------

MSDevice_CPhone::~MSDevice_CPhone()
{
    /*if registered to a cell then deregist from it*/
    MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
    if (pPhone != 0) {
        MSPhoneCell * cell = pPhone->getCurrentVehicleCell(myID);
        if (cell != 0) {
            if (m_State!=STATE_IDLE && m_State!=STATE_OFF) {
                cell->remCall(myCallId);
                if (OptionsCont::getOptions().isSet("cellphone-dump")) {
                    OutputDevice::getDeviceByOption("cellphone-dump")
                    << MSNet::getInstance()->getCurrentTimeStep() << ';'
                    << myCallId << ';' << mycurrentCellId << ';'
                    << 2 << ';' << myID <<"\n";
                }
            }
            cell->remCPhone(myID);
        } /*else {
                                            assert(m_State==STATE_IDLE||m_State==STATE_OFF);
                                        }*/
        MSPhoneLA * la = pPhone->getCurrentVehicleLA(myID);
        if (la != 0) {
            la->remCall(myID);
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
        for (int i=0;i<7;++i) {
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
    if (myCommand!=0) {
        myCommand->setInactivated();
    }
    myCommand = new MyCommand(*this);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        myCommand, dur + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
    if (OptionsCont::getOptions().isSet("cellphone-dump")) {
        OutputDevice::getDeviceByOption("cellphone-dump")
        << MSNet::getInstance()->getCurrentTimeStep() << ';'
        << myCallId << ';' << mycurrentCellId << ';'
        << 0 << ';' << myID <<"\n";
    }
    return dur;
}
//---------------------------------------------------------------------------

/*stop the current call*/
SUMOTime
MSDevice_CPhone::changeState()
{
    if (!notTriggeredByCell) {
        throw 1;
        assert(myCallId != -1);
        assert(mycurrentCellId != -1);
        if (OptionsCont::getOptions().isSet("cellphone-dump")) {
            OutputDevice::getDeviceByOption("cellphone-dump")
            << MSNet::getInstance()->getCurrentTimeStep() << ';'
            << myCallId << ';' << mycurrentCellId << ';'
            << 2 << ';' << myID <<"\n";
        }
        MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
        MSPhoneCell * cell = pPhone->getMSPhoneCell(mycurrentCellId);
        if (m_State == STATE_CONNECTED_IN || m_State == STATE_CONNECTED_OUT)
            cell->remCall(myCallId);

        myCallId = -1;
        m_State = STATE_IDLE;

        return (SUMOTime)0;
    } else { /*triggert by itself*/
        SUMOTime next;
        SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
        SUMOReal r2 = rand()/(SUMOReal) RAND_MAX;
        if (mycurrentCellId == -1) {
            return 10;   // no cell
        } else {
            // ok, have cell, can phone
            switch (m_State) {
            case STATE_OFF:
                throw 1;
                /*
                {
                    if(r1 > callProbabilityAutonomVeh)
                    {
                        // some people wait for a call
                        m_State = STATE_IDLE;
                        next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
                    }
                    else
                    {
                        // some start telephoning
                        if ( r2 > 0.5 )
                            m_State = STATE_CONNECTED_IN;
                        else
                            m_State = STATE_CONNECTED_OUT;

                        myCallId = ++gCallID;
                        myCallCellCount = 0;
                        MSPhoneCell * cell = MSNet::getInstance()->getMSPhoneNet()->getMSPhoneCell( mycurrentCellId );
                        if ( m_State == STATE_CONNECTED_IN )
                            cell->addCall( myCallId, STATICIN, myCallCellCount );
                        else
                            cell->addCall( myCallId, STATICOUT, myCallCellCount );
                        if (OptionsCont::getOptions().isSet("cellphone-dump")) {
                            OutputDevice::getDeviceByOption("cellphone-dump")
                            << MSNet::getInstance()->getCurrentTimeStep() << ';'
                            << myCallId << ';' << mycurrentCellId << ';'
                            << 0 << ';' << myID <<"\n";
                        }
                        //next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // telephone some seconds
                        next = (SUMOTime) getTrainDuration();
                    }
                }
                */
                break;
            case STATE_IDLE: {
                /*
                if(r1>callProbabilityAutonomVeh)
                {
                    // some people switch off
                    /*
                    if ( r2 > 0.5 )
                        m_State = STATE_OFF;
                    else
                    /
                        m_State = STATE_IDLE;
                    next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // keep it off
                }
                else
                {
                */
                SUMOReal prob = (SUMOReal) 0.00015;
                int time = MSNet::getInstance()->getCurrentTimeStep();
                time = time % 86400;
                time = time / 1800;
                if (tolDefaultProb>=0)
                    prob = tolDefaultProb;
                else
                    prob = (SUMOReal)(beginProbs[time] /*/ 2.5*/ * (double) OptionsCont::getOptions().getFloat("cell-dynamic-callcount-scale-factor"));
                next = 1;
                if (RandHelper::rand()<=prob) {
                    // most start telephoning
                    if (r2 > 0.5)
                        m_State = STATE_CONNECTED_IN;
                    else
                        m_State = STATE_CONNECTED_OUT;

                    myCallId = ++gCallID;
                    myCallCellCount = 0;

                    MSPhoneCell * cell = MSNet::getInstance()->getMSPhoneNet()->getMSPhoneCell(mycurrentCellId);

                    if (m_State == STATE_CONNECTED_IN)
                        cell->addCall(myCallId, STATICIN, myCallCellCount);
                    else
                        cell->addCall(myCallId, STATICOUT, myCallCellCount);
                    if (OptionsCont::getOptions().isSet("cellphone-dump")) {
                        OutputDevice::getDeviceByOption("cellphone-dump")
                        << MSNet::getInstance()->getCurrentTimeStep() << ';'
                        << myCallId << ';' << mycurrentCellId << ';'
                        << 0 << ';' << myID <<"\n";
                    }
                    //next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // telephone some seconds
                    next = (SUMOTime) getTrainDuration();
                }
            }
            break;
            case STATE_CONNECTED_IN:
            case STATE_CONNECTED_OUT: {
                if (OptionsCont::getOptions().isSet("cellphone-dump")) {
                    OutputDevice::getDeviceByOption("cellphone-dump")
                    << MSNet::getInstance()->getCurrentTimeStep() << ';'
                    << myCallId << ';' << mycurrentCellId << ';'
                    << 2 << ';' << myID <<"\n";
                }
                MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
                MSPhoneCell * cell = pPhone->getMSPhoneCell(mycurrentCellId);
                cell->remCall(myCallId);
                myCallId = -1;
                m_State = STATE_IDLE;
                next = 1; // (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
                /*
                if(r1>0.1)
                {
                    // most people stop telephonig
                    m_State = STATE_IDLE;
                    next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // no calls for some time
                }
                else
                {
                    // some switch off
                    m_State = STATE_OFF;
                    next = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // keep it off
                }
                */
            }
            break;
            default:
                throw 1;
            }
        }
        return next;
    }

}

void
MSDevice_CPhone::onDepart()
{
    /*wenn dieses handy nicht von einer zelle getriggert wird, dann setze ihn selber*/
    if (notTriggeredByCell) {
        m_State = STATE_IDLE;
        SUMOTime t1 = 10;
        /* !!! 31.05.2007 - reinsertion
        if( mycurrentCellId == -1 )
        {
            m_State = STATE_IDLE;
            t1 = 10;//(SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // start telephoning after some time
        }
        else
        {
            SUMOReal r1 = rand()/(SUMOReal) RAND_MAX;
            SUMOReal r2 = rand()/(SUMOReal) RAND_MAX;
            if(r1<0.1)
            {
                // 10% are off
                m_State = STATE_OFF;
                t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 60. * 60.);   // switch on after long time
            }
            else if(r1<0.2)
            {
                // 70% are idle
                m_State = STATE_IDLE;
                t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 20. * 60.);   // start telephoning after some time
            }
            else
            {
                // 20% are connected
                if ( r2 > 0.5 )
                    m_State = STATE_CONNECTED_IN;
                else
                    m_State = STATE_CONNECTED_OUT;

                myCallCellCount = 0;
                myCallId = ++gCallID;
                /*get a pointer tothe current cell/
                MSPhoneCell * cell = MSNet::getInstance()->getMSPhoneNet()->getMSPhoneCell( mycurrentCellId );
                if ( m_State == STATE_CONNECTED_IN )
                    cell->addCall( myCallId, STATICIN , myCallCellCount);
                else
                    cell->addCall( myCallId, STATICOUT, myCallCellCount );
                if (OptionsCont::getOptions().isSet("cellphone-dump")) {
                    OutputDevice::getDeviceByOption("cellphone-dump")
                    << MSNet::getInstance()->getCurrentTimeStep() << ';'
                    << myCallId << ';' << mycurrentCellId << ';'
                    << 0 << ';' << myID <<"\n";
                }
                t1 = (SUMOTime) (rand()/(SUMOReal) RAND_MAX * 5. * 60.);   // stop telephoning after some time
            }
            CPhoneBroadcastCell TempCell;
            for(int i=0;i<7;i++)
            {
                TempCell.m_CellID = 0;
                TempCell.m_LoS = -1;
                m_ProvidedCells.push_back(TempCell);
            }
        }
        !!! 31.05.2007 - reinsertion*/
        myCommand = new MyCommand(*this);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            myCommand, t1 + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
    } else {
        //throw 1;
    }
}


void
MSDevice_CPhone::invalidateCommand()
{
    this->myCommand = 0;
}



/****************************************************************************/

