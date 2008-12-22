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
#include <utils/iodevices/OutputDevice.h>


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
int MSDevice_CPhone::myVehicleIndex = 0;
SUMOReal tolDefaultProb = -1;


// ===========================================================================
// method definitions
// ===========================================================================
void
MSDevice_CPhone::insertOptions() throw()
{
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addOptionSubTopic("Cellular");

    // cell-phones
    oc.doRegister("device.cell-phone.percent-of-activity", new Option_Bool(false));
    oc.addDescription("device.cell-phone.percent-of-activity", "Cellular", "");

    oc.doRegister("device.cell-phone.knownveh", new Option_String());//!!! check, describe
    oc.addDescription("device.cell-phone.knownveh", "Cellular", "");

    oc.doRegister("device.cell-phone.probability", new Option_Float(0.));//!!! check, describe
    oc.addDescription("device.cell-phone.probability", "Cellular", "");

    oc.doRegister("ss2-output", new Option_FileName());//!!! check, describe
    oc.addDescription("ss2-output", "Cellular", "");

    oc.doRegister("ss2-sql-output", new Option_FileName());//!!! check, describe
    oc.addDescription("ss2-sql-output", "Cellular", "");

    oc.doRegister("ss2-cell-output", new Option_FileName());
    oc.addDescription("ss2-cell-output", "Cellular", "");

    oc.doRegister("ss2-sql-cell-output", new Option_FileName());
    oc.addDescription("ss2-sql-cell-output", "Cellular", "");

    oc.doRegister("ss2-la-output", new Option_FileName());
    oc.addDescription("ss2-la-output", "Cellular", "");

    oc.doRegister("ss2-sql-la-output", new Option_FileName());
    oc.addDescription("ss2-sql-la-output", "Cellular", "");

    oc.doRegister("cellphone-dump", new Option_FileName());
    oc.addDescription("cellphone-dump", "Cellular", "");

    oc.doRegister("cell-dynamic-callcount-scale-factor", new Option_Float(1.));
    oc.addDescription("cell-dynamic-callcount-scale-factor", "Cellular", "");

    oc.doRegister("cell-static-callcount-scale-factor", new Option_Float(1.));
    oc.addDescription("cell-static-callcount-scale-factor", "Cellular", "");

    oc.doRegister("cell-dynamic-calldeviation-scale-factor", new Option_Float(1.));
    oc.addDescription("cell-dynamic-calldeviation-scale-factor", "Cellular", "");

    oc.doRegister("cell-dynamic-callduration-scale-factor", new Option_Float(1.));
    oc.addDescription("cell-dynamic-callduration-scale-factor", "Cellular", "");

    oc.doRegister("cell-def-prob", new Option_Float(-1));//!!! check, describe
    oc.addDescription("cell-def-prob", "Cellular", "");

    oc.doRegister("device.cell-phone.amount.min", new Option_Float(1.));//!!! check, describe
    oc.addDescription("device.cell-phone.amount.min", "Cellular", "");

    oc.doRegister("device.cell-phone.amount.max", new Option_Float(1.));//!!! check, describe
    oc.addDescription("device.cell-phone.amount.max", "Cellular", "");

    oc.doRegister("device.cell-phone.sql-date", new Option_String("1970-01-01"));
    oc.addDescription("device.cell-phone.sql-date", "Cellular", "Sets the date to use in sql-dumps");

//    myVehicleIndex = 0;
}


void
MSDevice_CPhone::buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw()
{
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getFloat("device.cell-phone.probability")==0&&!oc.isSet("device.cell-phone.knownveh")) {
        // no c2c communication is modelled
        return;
    }
    // c2c communication is enabled
    bool haveByNumber = false;
    /*
    if (oc.getBool("device.cell-phone.deterministic")) {
        haveByNumber = ((myVehicleIndex%1000) < (int)(oc.getFloat("device.cell-phone.probability")*1000.));
    } else {
    !!!
    */
    haveByNumber = RandHelper::rand()<=oc.getFloat("device.cell-phone.probability");
    // !!!}
    bool haveByName = oc.isSet("device.cell-phone.knownveh") && OptionsCont::getOptions().isInStringVector("device.cell-phone.knownveh", v.getID());
    if (haveByNumber||haveByName) {
        int noCellPhones = (int)RandHelper::rand(oc.getFloat("device.cell-phone.amount.min"), oc.getFloat("device.cell-phone.amount.max"));
        for (int np=0; np<noCellPhones; np++) {
            string phoneid = v.getID() + "_cphone#" + toString(np);
            into.push_back(new MSDevice_CPhone(v, phoneid));
        }
    }
    myVehicleIndex++;
}


/*
void
MSVehicle::initDevices(int vehicleIndex)
{
    OptionsCont &oc = OptionsCont::getOptions();
    // cell phones
    if (myType->getID().compare("SBahn")== 0) {
        int noCellPhones = 1;
        if ((28800 <= myDesiredDepart && 32400 >= myDesiredDepart) || (61200 <= myDesiredDepart && 64800 >= myDesiredDepart))//40% 8 -9;17-18
            noCellPhones = 154;
        else if ((46800 <= myDesiredDepart && 61200 >= myDesiredDepart) || (64800 <= myDesiredDepart && 68400 >= myDesiredDepart)) //35% 13-17;18-19
            noCellPhones = 134;
        else if ((21600 <= myDesiredDepart && 28800 >= myDesiredDepart) || (32400 <= myDesiredDepart && 46800 >= myDesiredDepart) //25% 6-8;9-13;19-24
                 || (68400 <= myDesiredDepart && 86400 >= myDesiredDepart))
            noCellPhones = 96;
        else if ((0 <= myDesiredDepart && 5400 >= myDesiredDepart) || (14400 <= myDesiredDepart && 21600 >= myDesiredDepart)) //10% 0-1:30;4-6
            noCellPhones = 38;
        vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
        for (int np=0; np<noCellPhones; np++) {
            string phoneid = getID() + "_cphone#" + toString(np);
            v->push_back(new MSDevice_CPhone(*this, phoneid));
        }
        myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
    } else if (myType->getID().substr(0, 3)=="PKW") {
        int noCellPhones = 1;
        vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
        for (int np=0; np<noCellPhones; np++) {
            string phoneid = getID() + "_cphone#" + toString(np);
            v->push_back(new MSDevice_CPhone(*this, phoneid));
        }
        myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
    }
    else if (oc.getBool("device.cell-phone.percent-of-activity")) {
        /*myIntCORNMap[MSCORN::CORN_VEH_DEV_NO_CPHONE] = 1;
        string phoneid = getID() + "_cphone#0";
        MSDevice_CPhone* pdcp  = new MSDevice_CPhone(*this, phoneid);
        myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*)pdcp;/
        if (RandHelper::rand()<=oc.getFloat("device.cell-phone.probability")) {
            vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
            string phoneid = getID() + "_cphone#0";
            v->push_back(new MSDevice_CPhone(*this, phoneid));
            myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
        }
    } else if (oc.getFloat("device.cell-phone.probability")!=0||oc.isSet("device.cell-phone.knownveh")) {
        bool t1 = RandHelper::rand()<=oc.getFloat("device.cell-phone.probability");
        bool t2 = oc.isSet("device.cell-phone.knownveh") && OptionsCont::getOptions().isInStringVector("device.cell-phone.knownveh", myID);
        if (t1||t2) {
            int noCellPhones = (int)RandHelper::rand(oc.getFloat("device.cell-phone.amount.min"),
                                                     oc.getFloat("device.cell-phone.amount.max"));
            vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
            for (int np=0; np<noCellPhones; np++) {
                string phoneid = getID() + "_cphone#" + toString(np);
                v->push_back(new MSDevice_CPhone(*this, phoneid));
            }
            myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
        }
    }
}
*/


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



MSDevice_CPhone::MSDevice_CPhone(MSVehicle &vehicle, const std::string &id) throw()
        : MSDevice(vehicle, id), myCommand(0)
{
    OptionsCont &oc = OptionsCont::getOptions();
    mycurrentCellId = -1;
    mycurrentLAId = -1;
    myCallId = -1;
    m_State = STATE_IDLE;
    myCallCellCount = 0;
    if (vehicle.getVehicleType().getID().compare("SBahn") == 0) {
        notTriggeredByCell = true;
    } else if (vehicle.getVehicleType().getID().substr(0, 3)=="PKW") {
        notTriggeredByCell = true;
    } else {
        notTriggeredByCell = false;
    }
    tolDefaultProb = oc.getFloat("cell-def-prob");
}


MSDevice_CPhone::~MSDevice_CPhone() throw()
{
    /*if registered to a cell then deregist from it*/
    MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
    if (pPhone != 0) {
        MSPhoneCell * cell = pPhone->getCurrentVehicleCell(getID());
        if (cell != 0) {
            if (m_State!=STATE_IDLE && m_State!=STATE_OFF) {
                cell->remCall(myCallId);
                if (OptionsCont::getOptions().isSet("cellphone-dump")) {
                    OutputDevice::getDeviceByOption("cellphone-dump")
                    << MSNet::getInstance()->getCurrentTimeStep() << ';'
                    << myCallId << ';' << mycurrentCellId << ';'
                    << 2 << ';' << getID() <<"\n";
                }
            }
            cell->remCPhone(getID());
        }
        MSPhoneLA * la = pPhone->getCurrentVehicleLA(getID());
        if (la != 0) {
            la->remCall(getID());
        }
    }
    if (myCommand!=0) {
        myCommand->deschedule();
    }
    m_ProvidedCells.clear();
}


MSDevice_CPhone::State
MSDevice_CPhone::GetState() const
{
    return m_State;
}


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
        myCommand->deschedule();
    }
    myCommand = new WrappingCommand< MSDevice_CPhone >(this, &MSDevice_CPhone::stateChangeCommandExecution);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        myCommand, dur + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
    if (OptionsCont::getOptions().isSet("cellphone-dump")) {
        OutputDevice::getDeviceByOption("cellphone-dump")
        << MSNet::getInstance()->getCurrentTimeStep() << ';'
        << myCallId << ';' << mycurrentCellId << ';'
        << 0 << ';' << getID() <<"\n";
    }
    return dur;
}


SUMOTime
MSDevice_CPhone::stateChangeCommandExecution(SUMOTime) throw(ProcessError)
{
    if (!notTriggeredByCell) {
        throw 1;
        assert(myCallId != -1);
        assert(mycurrentCellId != -1);
        if (OptionsCont::getOptions().isSet("cellphone-dump")) {
            OutputDevice::getDeviceByOption("cellphone-dump")
            << MSNet::getInstance()->getCurrentTimeStep() << ';'
            << myCallId << ';' << mycurrentCellId << ';'
            << 2 << ';' << getID() <<"\n";
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
                        << 0 << ';' << getID() <<"\n";
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
                    << 2 << ';' << getID() <<"\n";
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
MSDevice_CPhone::enterLaneAtEmit(MSLane* , const MSVehicle::State &)
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
        myCommand = new WrappingCommand< MSDevice_CPhone >(this, &MSDevice_CPhone::stateChangeCommandExecution);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            myCommand, t1 + MSNet::getInstance()->getCurrentTimeStep(), MSEventControl::ADAPT_AFTER_EXECUTION);
    } else {
        //throw 1;
    }
}



/****************************************************************************/

