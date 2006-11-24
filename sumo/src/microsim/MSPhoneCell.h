#ifndef MSPHONECELL_H
#define MSPHONECELL_H

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
	void setStatParams( int interval, int statcallcount/*, int dyncallcount */);
	void setDynParams( int interval, int count, float duration, float deviation );
	void writeOutput( SUMOTime time );
	void writeSQLOutput( SUMOTime time );
	void setnextexpectData(SUMOTime time);
	int getExpectDynCallCount(){return dyncallcount;};
	void decrementDynCallCount(){--dyncallcount;};

private:
    int Cell_Id;
    int Calls_In;
    int Calls_Out;
    int Dyn_Calls_In;
    int Dyn_Calls_Out;
    int Sum_Calls;
    int Intervall;
    int last_time;
	int statcallcount;
	int dyncallcount;
	float dynduration;
	float dyndeviation;

    struct DynParam {
        int count;
        float duration;
        float deviation;
    };

	std::vector<Call> _Calls;
	std::vector<Call>::iterator icalls;
	std::vector<std::pair<int, /*std::pair<int, int>*/ int > > vexpectStatCount;
	std::vector<std::pair<int, /*std::pair<int, int>*/ int > >::iterator itCount;
	std::vector<std::pair<int, DynParam/*std::pair<float, float>*/ > > vexpectDuration;
	std::vector<std::pair<int, DynParam/*std::pair<float, float>*/ > >::iterator itDuration;
};
#endif

