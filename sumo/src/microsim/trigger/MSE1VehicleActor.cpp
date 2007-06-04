/****************************************************************************/
/// @file    MSE1VehicleActor.cpp
/// @author  Daniel Krajzewicz
/// @date    23.03.2006
/// @version $Id$
///
// An actor which changes a vehicle's state
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

#include "MSE1VehicleActor.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/helpers/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <microsim/MSEventControl.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSLane.h>
#include <microsim/MSPhoneNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
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
// method definitions
// ===========================================================================
MSE1VehicleActor::MSE1VehicleActor(const std::string& id, MSLane* lane,
                                   SUMOReal positionInMeters,
                                   unsigned int laid, unsigned int cellid,
                                   unsigned int type)
        : MSMoveReminder(lane), MSTrigger(id), posM(positionInMeters),
        _LAId(laid), _AreaId(cellid), _ActorType(type),
        myPassedVehicleNo(0), myPassedCPhonesNo(0), myPassedConnectedCPhonesNo(0)
{
    assert(posM >= 0 && posM <= laneM->length());
    //eintragen in MSPhoneNet

    OptionsCont &oc = OptionsSubSys::getOptions();
    percentOfActivity = oc.getBool("device.cell-phone.percent-of-activity");

    if (type == 1) {
        MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
        /*if ( pPhone->getMSPhoneCell( _AreaId ) == 0 )
            pPhone->addMSPhoneCell( _AreaId, _LAId );
        else*/
        pPhone->addMSPhoneCell(_AreaId, _LAId);
    }
}


MSE1VehicleActor::~MSE1VehicleActor()
{}

map<MSVehicle *, MSPhoneCell*> LastCells;

bool
MSE1VehicleActor::isStillActive(MSVehicle& veh,
                                SUMOReal /*oldPos*/,
                                SUMOReal newPos,
                                SUMOReal /*newSpeed*/)
{
    if (newPos < posM) {
        // detector not reached yet
        return true;
    }
    myPassedVehicleNo++;
    if (_ActorType == 1) {
        SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
        if (LastCells.find(&veh)!=LastCells.end()) {
            MSPhoneCell *cell = LastCells[&veh];
            LastCells.erase(LastCells.find(&veh));
            assert(cell!=0);
            if (cell!=0) {
                cell->removeVehicle(veh, time);
            }
        }
        MSPhoneCell *cell = MSNet::getInstance()->getMSPhoneNet()->getMSPhoneCell(_AreaId);
        cell->incVehiclesEntered(veh, time);
        LastCells[&veh] = cell;
    }
    if (!veh.hasCORNPointerValue(MSCORN::CORN_P_VEH_DEV_CPHONE)) {
        return false;
    }


    vector<MSDevice_CPhone*> *v = (vector<MSDevice_CPhone*>*) veh.getCORNPointerValue(MSCORN::CORN_P_VEH_DEV_CPHONE);
    /*get the count of mobiles for the vehicle*/
    int noCellPhones = v->size();
    myPassedCPhonesNo += noCellPhones;

    if (_ActorType == 1) { /* 1 == cell/la */
        /*get a pointer to the PhoneNet*/
        MSPhoneNet *pPhone = MSNet::getInstance()->getMSPhoneNet();
        /*now change each mobile for the old cell to the new one*/
        for (vector<MSDevice_CPhone*>::iterator i=v->begin(); i!=v->end(); ++i) {
            MSDevice_CPhone *cp = (*i);
            assert(cp != 0);

            /* first buffer the old la, if we might change it*/
            int oldLAId = cp->getCurrentLAId();

            /* set the current cell id an LA id*/
            cp->setCurrentCellId(_AreaId);
            cp->setCurrentLAId(_LAId);

            /*get the state off the mobile*/
            MSDevice_CPhone::State state = cp->GetState();

            if (state!=MSDevice_CPhone::STATE_OFF) {
                // at first we have a look on the current la_id and the old one. if they are equal the is no reason
                // to do anything.
                if (oldLAId != _LAId && oldLAId != -1) {
                    pPhone->addLAChange(toString(oldLAId) + toString(_LAId));
                }
                /*
                // move to the next la if the phone is not off
                MSPhoneLA *oldLA = pPhone->getCurrentVehicleLA(cp->getId());
                MSPhoneLA *newLA = pPhone->getMSPhoneLA(_AreaId);
                assert(newLA!=0);
                //if the pointer to the old LA is NULL this mobile wasnt in a LA befor, in this case we dont have
                  //o deregister it from the old cell
                if( oldLA!=0 ){//be sure, that the old la isnt the same as the new la; 
                               //if true there is no reason for a change
                    if(oldLA!=newLA){
                        oldLA->remCall(cp->getId());
                        newLA->addCall(cp->getId());
                        pPhone->addLAChange( toString( oldLA->getPositionId() ) + toString( newLA->getPositionId() ) );
                    }
                } else //there is no old LA
                newLA->addCall(cp->getId());
                }*/
            }

            MSPhoneCell *oldCell = pPhone->getCurrentVehicleCell(cp->getID());
            MSPhoneCell *newCell = pPhone->getMSPhoneCell(_AreaId);

            if (oldCell != 0) {
                oldCell->remCPhone(cp->getID());
            } /*else {
                            // check whether a call shall be started
                            SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
                            if(newCell->useAsIncomingDynamic(time)) {
                                if(randSUMO()>.5) {
                                    cp->SetState(MSDevice_CPhone::STATE_CONNECTED_IN , newCell->getCallDuration() / 2.);
                                } else {
                                    cp->SetState(MSDevice_CPhone::STATE_CONNECTED_OUT , newCell->getCallDuration() / 2.);
                                }
                            }
                        }*/
            assert(newCell != 0);
            newCell->addCPhone(cp->getID(), cp);
            int callCount = cp->GetCallCellCount();
            cp->IncCallCellCount();
            switch (cp->GetState()) {
            case MSDevice_CPhone::STATE_OFF:
                break;
            case MSDevice_CPhone::STATE_IDLE:
                /*!!! 31.05.2007 - reinsertion
                if (percentOfActivity) {
                    SUMOReal r1 = randSUMO();
                    if (r1 < 0.5f) {
                        cp->SetState(MSDevice_CPhone::STATE_CONNECTED_IN , 86400);
                    } else {
                        cp->SetState(MSDevice_CPhone::STATE_CONNECTED_OUT , 86400);
                    }
                }
                */
                break;
            case MSDevice_CPhone::STATE_CONNECTED_IN:
                assert(cp->getCallId() != -1);
                // remove the call from the old cell
                if (oldCell != 0) {
                    oldCell->remCall(cp->getCallId());
                }
                // move to the new cell if the phone is connected
                newCell->addCall(cp->getCallId(), DYNIN, callCount);
                myPassedConnectedCPhonesNo++;
                break;
            case MSDevice_CPhone::STATE_CONNECTED_OUT:
                assert(cp->getCallId() != -1);
                // move to the new cell if the phone is connected
                if (oldCell != 0) {
                    oldCell->remCall(cp->getCallId());
                }
                newCell->addCall(cp->getCallId(), DYNOUT, callCount);
                myPassedConnectedCPhonesNo++;
                break;
            }
            if (state==MSDevice_CPhone::STATE_CONNECTED_IN || state==MSDevice_CPhone::STATE_CONNECTED_OUT) {
                OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELLPHONE_DUMP_TO);
                if (od!=0) {
                    od->getOStream()
                    << MSNet::getInstance()->getCurrentTimeStep() << ';'
                    << cp->getCallId() << ';'
                    << _AreaId << ';'
                    << "1;" << cp->getID() << std::endl;;
                }
            }
        }
    } else { // TOL_SA
        for (vector<MSDevice_CPhone*>::iterator i=v->begin(); i!=v->end(); ++i) {
            MSDevice_CPhone* cp = *i;
            MSDevice_CPhone::State state = cp->GetState();
            if (state==MSDevice_CPhone::STATE_CONNECTED_IN||state==MSDevice_CPhone::STATE_CONNECTED_OUT) {
                myPassedConnectedCPhonesNo++;
                {
                    OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_DEVICE_TO_SS2);
                    if (od!=0) {
                        std::string timestr= OptionsSubSys::getOptions().getString("device.cell-phone.sql-date");
                        timestr = timestr + " " + StringUtils::toTimeString(MSNet::getInstance()->getCurrentTimeStep());
                        // !!! recheck quality indicator
                        od->getOStream()
                        << "01;'" << timestr << "';" << cp->getCallId() << ';' << _AreaId << ';' << 0 << "\n"; // !!! check <CR><LF>-combination
                    }
                }
                {
                    OutputDevice *od = MSNet::getInstance()->getOutputDevice(MSNet::OS_DEVICE_TO_SS2_SQL);
                    if (od!=0) {
                        if (od->getBoolMarker("hadFirstCall")) {
                            od->getOStream() << "," << endl;
                        } else {
                            od->setBoolMarker("hadFirstCall", true);
                        }
                        std::string timestr= OptionsSubSys::getOptions().getString("device.cell-phone.sql-date");
                        timestr = timestr + " " + StringUtils::toTimeString(MSNet::getInstance()->getCurrentTimeStep());
                        od->getOStream()
                        << "(NULL, NULL, '" << timestr << "', " << _AreaId << ", " << cp->getCallId()
                        << ", " << 0 << ")"; // !!! recheck quality indicator
                    }
                }
            }
        }
    }
    return false;
}


void
MSE1VehicleActor::dismissByLaneChange(MSVehicle&)
{}


bool
MSE1VehicleActor::isActivatedByEmitOrLaneChange(MSVehicle& veh)
{
    if (veh.getPositionOnLane()-veh.getLength() > posM) {
        // vehicle-end is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;
}



/****************************************************************************/

