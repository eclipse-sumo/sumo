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
