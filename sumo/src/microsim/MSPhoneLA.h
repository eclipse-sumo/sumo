#ifndef MSPHONELA_H
#define MSPHONELA_H

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)
#include "MSCORN.h"
#include <string>
#include <vector>

class MSPhoneLA{
public:
    MSPhoneLA(int pos_id, int dir);
    ~MSPhoneLA();
	void addCall( );
	void addCall(std::string id);
	bool getCall(std::string id);
	void remCall(std::string id);
	void writeOutput( SUMOTime t );
	void writeSQLOutput( SUMOTime t );
	bool operator==( MSPhoneLA* );
	bool operator!=( MSPhoneLA* );
private:
    int last_time;
    int position_id;
    int dir; //direction
    int sum_changes;
    int quality_id;
    int intervall;
	std::vector<std::string> _Calls;
	std::vector<std::string>::iterator icalls;
};

#endif

