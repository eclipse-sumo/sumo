#ifndef MSPHONECELL_H
#define MSPHONECELL_H

#include <string>
#include <vector>
#include "MSCORN.h"

enum CallType{
	STATICIN,
	STATICOUT,
	DYNIN,
	DYNOUT
};

struct Call{
	CallType ct;
	std::string id;
};

class MSPhoneCell{
public:
    MSPhoneCell(int ID);
    ~MSPhoneCell();
	void addCall(std::string id, CallType);//{Calls_In++; Sum_Calls++;};
    //void addDynCallIn(std::string id);//{Dyn_Calls_In++; Sum_Calls++;};
	void remCall(std::string id);//{Calls_In--; Sum_Calls--;};
    //void remDynCallIn(std::string id)//{Dyn_Calls_In--; Sum_Calls--;};
	bool getCall( std::string id );
    int  getIntervall();
	void writeOutput( SUMOTime t );
	void writeSQLOutput( SUMOTime t );
private:
    int Cell_Id;
    int Calls_In;
    int Calls_Out;
    int Dyn_Calls_In;
    int Dyn_Calls_Out;
    int Sum_Calls;
    int Intervall;
    int last_time;
	std::vector<Call> _Calls;
	std::vector<Call>::iterator icalls;
};
#endif
