#ifndef MSPHONENET_H
#define MSPHONENET_H

#include <map>
#include "MSPhoneCell.h"
#include "MSPhoneLA.h"
#include "MSCORN.h"



class MSPhoneNet{
public:
    MSPhoneNet();
    ~MSPhoneNet();
    MSPhoneLA* getMSPhoneLA( int id );
    void addMSPhoneLA( int id , int dir);
    void remMSPhoneLA( int id );
    MSPhoneCell* getMSPhoneCell( int id );
	MSPhoneCell* getcurrentVehicleCell( std::string id);
    void addMSPhoneCell( int id );
    void remMSPhoneCell( int id );
	void writeOutput( SUMOTime t );
private:
    std::map< int, MSPhoneCell* > _mMSPhoneCells;
    std::map< int, MSPhoneLA* > _mMSPhoneLAs;
	unsigned int _LAIntervall;
	unsigned int _CellIntervall;
};
#endif
