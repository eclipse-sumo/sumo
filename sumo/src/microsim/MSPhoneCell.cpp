/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "MSPhoneCell.h"

MSPhoneCell::MSPhoneCell( int id ):
Cell_Id( id ),
Calls_In( 0 ),
Calls_Out( 0 ),
Dyn_Calls_In( 0 ),
Dyn_Calls_Out( 0 ),
Sum_Calls( 0 ),
Intervall( 0 ),
last_time( 0 ){
}

MSPhoneCell::~MSPhoneCell(){
}

int
MSPhoneCell::getIntervall(){
	return Intervall;
}

void
MSPhoneCell::addCall( std::string id, CallType t){
	Call c;
	c.ct=t;
	c.id=id;
	if ( getCall( id ) )
		int i = 0;
	_Calls.push_back( c );
	switch ( t ){
		case STATICIN:
			Calls_In++;
			break;
		case STATICOUT:
			Calls_Out++;
			break;
		case DYNIN:
			Dyn_Calls_In++;
			break;
		case DYNOUT:
			Dyn_Calls_Out++;
			break;
	}
	++Sum_Calls;
}

void
MSPhoneCell::remCall( std::string id ){
	for ( icalls=_Calls.begin(); icalls!=_Calls.end(); icalls++){
		if ( (*icalls).id.compare( id ) == 0 ){
			switch ( (*icalls).ct ){
				case STATICIN:
					Calls_In--;
					break;
				case STATICOUT:
					Calls_Out--;
					break;
				case DYNIN:
					Dyn_Calls_In--;
					break;
				case DYNOUT:
					Dyn_Calls_Out--;
					break;
			}
			if(Sum_Calls>0)
				--Sum_Calls;
			_Calls.erase( icalls );
			break;
		}
	}
}

void
MSPhoneCell::setStatParams( int interval, int statcallcount/*, int dyncallcount */){
	vexpectStatCount.push_back( std::make_pair( interval, /*std::make_pair( */statcallcount/*, dyncallcount ) */) );
}

void
MSPhoneCell::setDynParams( int interval, int count, float duration, float deviation ){
    DynParam p;
    p.count = count;
    p.deviation = deviation;
    p.duration = duration;
	vexpectDuration.push_back( std::make_pair( interval, p ) );// std::make_pair( duration, deviation ) ) );
}


void
MSPhoneCell::setnextexpectData(SUMOTime t){
	itCount = vexpectStatCount.begin();
	if( itCount !=vexpectStatCount.end() && ( t % 300 ) == 0 ){
		Intervall = itCount->first;
		statcallcount = itCount->second/*.first*/;
		/*verteile ein/aus erstmal 50/50*/
		Calls_In = statcallcount / 2;
		Calls_Out = statcallcount / 2;
//		dyncallcount = itCount->second.second;
		vexpectStatCount.erase(itCount);
	}
	itDuration = vexpectDuration.begin();
	if( itDuration != vexpectDuration.end() && ( t % 3600 ) == 0 ){
        dyncallcount = itDuration->second.count;
		dynduration = itDuration->second.duration;
		dyndeviation = itDuration->second.deviation;
		vexpectDuration.erase( itDuration );
	}
}

bool
MSPhoneCell::getCall( std::string id ){
	if ( Cell_Id == 1 )
		int i = 1;
	bool ret = false;
	std::string debug_tmp;
	for ( icalls=_Calls.begin(); icalls!=_Calls.end(); (icalls++)){
		debug_tmp = (*icalls).id;
		if ( debug_tmp.compare ( id ) == 0 ){
		//if ( (*icalls).id.compare( id ) == 0 ){
			ret = true;
			break;
		}
	}
	return  ret;
}

void
MSPhoneCell::writeOutput( SUMOTime t ){
	Intervall = t - last_time;
	MSCORN::saveTOSS2_CellStateData( t, Cell_Id, Calls_In, Calls_Out,
		Dyn_Calls_In, Dyn_Calls_Out, Sum_Calls, Intervall );
	last_time = t;
}

void
MSPhoneCell::writeSQLOutput( SUMOTime t ){
	Intervall = t - last_time;
	MSCORN::saveTOSS2SQL_CellStateData( t, Cell_Id, Calls_In, Calls_Out,
		Dyn_Calls_In, Dyn_Calls_Out, Sum_Calls, Intervall );
	last_time = t;
}
