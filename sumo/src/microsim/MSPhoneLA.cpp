/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "MSPhoneLA.h"

MSPhoneLA::MSPhoneLA( int pos_id, int d ):
last_time( 0 ),
position_id( pos_id ),
dir( d ),
sum_changes( 0 ),
quality_id( 0 ),
intervall( 0 )
{}

MSPhoneLA::~MSPhoneLA(){}

void
MSPhoneLA::addCall( ){
	sum_changes++;
}

void
MSPhoneLA::addCall(std::string id){
	if ( getCall( id ) )
		return;
	_Calls.push_back( id );
	++sum_changes;
}

void
MSPhoneLA::remCall( std::string id ){
	for ( icalls=_Calls.begin(); icalls!=_Calls.end(); (icalls++) ){
			if((*icalls).compare(id)==0){
			if(sum_changes>0)
				--sum_changes;
			_Calls.erase( icalls );
			break;
		}
	}
}

bool
MSPhoneLA::getCall( std::string id ){
	bool ret = false;
	std::string debug_tmp;
	for ( icalls=_Calls.begin(); icalls!=_Calls.end(); icalls++){
		debug_tmp = (*icalls);
		if ( debug_tmp.compare ( id ) == 0 ){
			ret = true;
			break;
		}
	}
	return  ret;
}

bool
MSPhoneLA::operator ==(MSPhoneLA * other){
	if(this->position_id==other->position_id &&
		this->dir==other->dir)
		return true;
	else
		return false;
}

bool
MSPhoneLA::operator !=(MSPhoneLA * other){
	if(this->position_id!=other->position_id ||
		this->dir!=other->dir)
		return true;
	else
		return false;
}

void
MSPhoneLA::writeOutput( SUMOTime t ){
	intervall = t - last_time;
	MSCORN::saveTOSS2_LA_ChangesData( t, position_id, dir, sum_changes, quality_id, intervall );
	last_time = t;
	sum_changes = 0;
}

void
MSPhoneLA::writeSQLOutput( SUMOTime t ){
	intervall = t - last_time;
	MSCORN::saveTOSS2SQL_LA_ChangesData( t, position_id, dir, sum_changes, quality_id, intervall );
	last_time = t;
	sum_changes = 0;
}
