#ifndef MSPHONELA_H
#define MSPHONELA_H

#include "MSCORN.h"

class MSPhoneLA{
public:
    MSPhoneLA(int pos_id, int dir);
    ~MSPhoneLA();
	void addCall( );
	void writeOutput( SUMOTime t );
	void writeSQLOutput( SUMOTime t );
private:
    int last_time;
    int position_id;
    int dir; //direction
    int sum_changes;
    int quality_id;
    int intervall;
};

#endif

