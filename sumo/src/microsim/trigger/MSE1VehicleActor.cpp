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
// $Log$
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
        SUMOReal positionInMeters, unsigned int laid, unsigned int cellid, unsigned int type )
								   : MSMoveReminder(lane), MSTrigger(id),
								   posM(positionInMeters),   _LAId( laid ), _AreaId( cellid ), _ActorType( type ){
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
	// !!! do something
	/*in welchen zustand ist das handy?????????*/
	if( _ActorType == 1 ) { // 1 == cell/la
	int noCellPhones = ( int ) veh.getCORNDoubleValue( (MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE );
	for(int np=0; np<noCellPhones; np++){
		MSDevice_CPhone* cp = (MSDevice_CPhone*)veh.getCORNPointerValue((MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np));
		MSPhoneNet * pPhone = MSNet::getInstance()->getMSPhoneNet();
		MSDevice_CPhone::State state = cp->GetState();
		if(state!=MSDevice_CPhone::STATE_OFF){
			MSPhoneLA * lold   = pPhone->getcurrentVehicleLA(veh.getID());
			MSPhoneLA * lnew   = pPhone->getMSPhoneLA( _AreaId );
			if(lold!=NULL){
				if(lold!=lnew){
					lold->remCall(veh.getID());
					lnew->addCall(veh.getID());
				}
			}
			else
				lnew->addCall(veh.getID());
		}
		cp->setCurrentCellId( _AreaId );
		MSPhoneCell * cold = pPhone->getcurrentVehicleCell( veh.getID() );
		MSPhoneCell * cnew = pPhone->getMSPhoneCell( _AreaId );
		switch(cp->GetState()){
			case	MSDevice_CPhone::STATE_OFF:
				if ( cold != 0 )
					cold->remCall( veh.getID() );
				break;
			case	MSDevice_CPhone::STATE_IDLE:
				if ( cold != 0 )
					cold->remCall( veh.getID() );
				break;
			case	MSDevice_CPhone::STATE_CONNECTED_IN:
			if ( cold != 0 )
				cold->remCall( veh.getID() );
				cnew->addCall( veh.getID(), DYNIN );
			break;
		case	MSDevice_CPhone::STATE_CONNECTED_OUT:
			if ( cold != 0 )
				cold->remCall( veh.getID() );
				cnew->addCall( veh.getID(), DYNOUT );
			break;
		}
	}
	}
	else { // TOL_SA
		char buffer[65];
		int noCellPhones = ( int ) veh.getCORNDoubleValue( (MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE );
		for(int np=0; np<noCellPhones; np++){
			MSDevice_CPhone* cp = (MSDevice_CPhone*)veh.getCORNPointerValue((MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np));
			MSDevice_CPhone::State state = cp->GetState();
			if(state==MSDevice_CPhone::STATE_CONNECTED_IN||state==MSDevice_CPhone::STATE_CONNECTED_OUT){
				if(MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2))
				MSCORN::saveTOSS2_CalledPositionData(
					MSNet::getInstance()->getCurrentTimeStep(), cp->getCallId(), itoa(_AreaId, buffer, 10), 0); // !!! recheck quality indicator
			if(MSCORN::wished(MSCORN::CORN_OUT_DEVICE_TO_SS2_SQL))
				MSCORN::saveTOSS2SQL_CalledPositionData(
					MSNet::getInstance()->getCurrentTimeStep(), cp->getCallId(), itoa(_AreaId, buffer, 10), 0); // !!! recheck quality indicator
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
