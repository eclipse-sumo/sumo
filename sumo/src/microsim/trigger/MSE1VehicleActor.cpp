//---------------------------------------------------------------------------//
//                        MSE1VehicleActor.cpp -
//  An actor which changes a vehicle's state
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 23.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log: MSE1VehicleActor.cpp,v $
// Revision 1.14  2006/12/06 17:00:24  ericnicolay
// added new output for cellphone_dump
//
// Revision 1.13  2006/12/01 14:42:10  dkrajzew
// added some visualization
//
// Revision 1.12  2006/12/01 09:14:42  dkrajzew
// debugging cell phones
//
// Revision 1.11  2006/11/28 12:15:41  dkrajzew
// documented TOL-classes and made them faster
//
// Revision 1.9  2006/11/24 10:34:59  dkrajzew
// added Eric Nicolay's current code
//
// Revision 1.8  2006/11/16 12:30:54  dkrajzew
// warnings removed
//
// Revision 1.7  2006/11/16 10:50:45  dkrajzew
// warnings removed
//
// Revision 1.6  2006/11/08 16:27:51  ericnicolay
// change code for the cell-actor
//
// Revision 1.5  2006/10/12 08:06:35  dkrajzew
// removed unneeded id member in MSMoveReminder
//
// Revision 1.4  2006/07/06 11:12:56  dkrajzew
// debugging
//
// Revision 1.3  2006/07/05 11:02:06  ericnicolay
// add code for change state of cphones and register them to the cells and las
//
// Revision 1.2  2006/05/15 05:47:50  dkrajzew
// got rid of the cell-to-meter conversions
//
// Revision 1.2  2006/05/08 10:54:42  dkrajzew
// got rid of the cell-to-meter conversions
//
// Revision 1.1  2006/03/27 07:19:47  dkrajzew
// vehicle actors added
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

#include "MSE1VehicleActor.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/helpers/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSPhoneNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include "../MSPhoneCell.h"
#include "../MSPhoneLA.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
* used namespaces
* ======================================================================= */
using namespace std;


/* =========================================================================
* method definitions
* ======================================================================= */
MSE1VehicleActor::MSE1VehicleActor( const std::string& id, MSLane* lane,
                                   SUMOReal positionInMeters,
                                   unsigned int laid, unsigned int cellid,
                                   unsigned int type )
    : MSMoveReminder(lane), MSTrigger(id), posM(positionInMeters),
    _LAId( laid ), _AreaId( cellid ), _ActorType( type ),
    myPassedVehicleNo(0), myPassedCPhonesNo(0), myPassedConnectedCPhonesNo(0)
{
    assert( posM >= 0 && posM <= laneM->length() );
    //eintragen in MSPhoneNet
    if ( type == 1 ){
        MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
        /*if ( pPhone->getMSPhoneCell( _AreaId ) == 0 )
            pPhone->addMSPhoneCell( _AreaId, _LAId );
        else*/
        pPhone->addMSPhoneCell( _AreaId, _LAId );
    }
}


MSE1VehicleActor::~MSE1VehicleActor(){
}


bool
MSE1VehicleActor::isStillActive( MSVehicle& veh,
								SUMOReal /*oldPos*/,
								SUMOReal newPos,
								SUMOReal /*newSpeed*/ )
{
    if ( newPos < posM ) {
        // detector not reached yet
        return true;
    }
    
    if( _ActorType == 1 ) { /* 1 == cell/la */
        /*get a pointer to the PhoneNet*/
        MSPhoneNet *pPhone = MSNet::getInstance()->getMSPhoneNet();
        /*get the count of mobiles for the vehicle*/
        int noCellPhones = ( int ) veh.getCORNDoubleValue( (MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE );
        myPassedCPhonesNo += noCellPhones;
        /*now change each mobile for the old cell to the new one*/
        for(int np=0; np<noCellPhones; np++){
            MSDevice_CPhone *cp = (MSDevice_CPhone*)veh.getCORNPointerValue((MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np));
            assert( cp != 0 );

            /* first buffer the old la, if we might change it*/
            int oldLAId = cp->getCurrentLAId();
            
            /* set the current cell id an LA id*/
            cp->setCurrentCellId(_AreaId);
            cp->setCurrentLAId(_LAId);
            
            /*get the state off the mobile*/
			MSDevice_CPhone::State state = cp->GetState();
            
            if(state!=MSDevice_CPhone::STATE_OFF) {
                // at first we have a look on the current la_id and the old one. if they are equal the is no reason
                // to do anything.
                if ( oldLAId != _LAId && oldLAId != -1 ){
                    pPhone->addLAChange( toString ( oldLAId ) + toString( _LAId ) );
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

            MSPhoneCell *oldCell = pPhone->getCurrentVehicleCell(cp->getId());
            MSPhoneCell *newCell = pPhone->getMSPhoneCell(_AreaId);
            
            if( oldCell != 0 )
                oldCell->remCPhone( cp->getId() );
            assert ( newCell != 0 );
            newCell->addCPhone( cp->getId(), cp );

            switch(cp->GetState())
			{
            case MSDevice_CPhone::STATE_OFF:
                break;
            case MSDevice_CPhone::STATE_IDLE:
                break;
            case MSDevice_CPhone::STATE_CONNECTED_IN:
                assert ( cp->getCallId() != -1 );
                // remove the call from the old cell
                if ( oldCell != 0 ) {
                    oldCell->remCall(cp->getCallId());
                }
                // move to the new cell if the phone is connected
                newCell->addCall(cp->getCallId(), DYNIN );
                myPassedConnectedCPhonesNo++;
                break;
            case MSDevice_CPhone::STATE_CONNECTED_OUT:
                assert ( cp->getCallId() != -1 );
                // move to the new cell if the phone is connected
                if ( oldCell != 0 ) {
                    oldCell->remCall(cp->getCallId());
                }
                newCell->addCall(cp->getCallId(), DYNOUT );
                myPassedConnectedCPhonesNo++;
			    break;
            }
            if(state==MSDevice_CPhone::STATE_CONNECTED_IN || state==MSDevice_CPhone::STATE_CONNECTED_OUT){
                if( MSCORN::wished(MSCORN::CORN_OUT_CELLPHONE_DUMP_TO) ){
                    MSCORN::saveCELLPHONEDUMP( MSNet::getInstance()->getCurrentTimeStep(), _AreaId, cp->getCallId(), 1 );
                }
            }
        }
    } else { // TOL_SA
        int noCellPhones = ( int ) veh.getCORNDoubleValue( (MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE );
        myPassedCPhonesNo += noCellPhones;
        for(int np=0; np<noCellPhones; np++){
            MSDevice_CPhone* cp = (MSDevice_CPhone*) veh.getCORNPointerValue((MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np));
            MSDevice_CPhone::State state = cp->GetState();
            if(state==MSDevice_CPhone::STATE_CONNECTED_IN||state==MSDevice_CPhone::STATE_CONNECTED_OUT) {
                myPassedConnectedCPhonesNo++;
                if(MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2)) {
                    MSCORN::saveTOSS2_CalledPositionData(
                        MSNet::getInstance()->getCurrentTimeStep(), cp->getCallId(),
                        toString(_AreaId), 0); // !!! recheck quality indicator
                }
                if(MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2_SQL)) {
                    MSCORN::saveTOSS2SQL_CalledPositionData(
                        MSNet::getInstance()->getCurrentTimeStep(), cp->getCallId(),
                        toString(_AreaId), 0); // !!! recheck quality indicator
                }
            }
        }
    }
    return false;
}


void
MSE1VehicleActor::dismissByLaneChange( MSVehicle&  )
{
}


bool
MSE1VehicleActor::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
	if ( veh.getPositionOnLane()-veh.getLength() > posM ) {
		// vehicle-end is beyond detector. Ignore
		return false;
	}
	// vehicle is in front of detector
	return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
