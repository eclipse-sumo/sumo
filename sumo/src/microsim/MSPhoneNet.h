#ifndef MSPHONENET_H
#define MSPHONENET_H

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

#include <map>
#include <string>
#include "MSPhoneCell.h"
#include "MSPhoneLA.h"
#include "MSCORN.h"
#include <fstream>
//#include "utils/importio/LineReader.h"
#include "utils/options/OptionsCont.h"
#include "utils/options/OptionsSubSys.h"


class MSPhoneNet{
public:
    MSPhoneNet();
    ~MSPhoneNet();
    MSPhoneLA* getMSPhoneLA( int id );
    void addMSPhoneLA( int id , int dir);
    void remMSPhoneLA( int id );
    MSPhoneCell* getMSPhoneCell( int id );
	MSPhoneCell* getcurrentVehicleCell( std::string id);
	MSPhoneLA* getcurrentVehicleLA( std::string id);
    void addMSPhoneCell( int id );
	void addMSPhoneCell( int id , int la);
    void remMSPhoneCell( int id );
	void connectLA2Cell( int cell_ID, int la_ID );
	void writeOutput( SUMOTime time );
	void setCellStatData(SUMOTime time);
private:
    std::map< int, MSPhoneCell* > _mMSPhoneCells;
    std::map< int, MSPhoneLA* > _mMSPhoneLAs;
	std::map< int, int > _mCell2LA;
	std::map< int, MSPhoneCell* >::iterator cit;
	std::map< int, MSPhoneLA* >::iterator lit;
	unsigned int _LAIntervall;
	unsigned int _CellIntervall;
	SUMOTime _currTime;
};
#endif
