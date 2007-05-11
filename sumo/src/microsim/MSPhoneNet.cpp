/****************************************************************************/
/// @file    MSPhoneNet.cpp
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// The cellular network (GSM)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include "MSPhoneNet.h"
#include "MSCORN.h"
#include <iostream>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>
#include "MSPhoneCell.h"
#include "MSPhoneLA.h"
#include "MSNet.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneNet::MSPhoneNet()
{
	OptionsCont &oc = OptionsSubSys::getOptions();
	percentModus = oc.getBool("device.cell-phone.percent-of-activity");
	lastTime = 0;
}


MSPhoneNet::~MSPhoneNet()
{
	// close outputs
	SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
	if((currentTime-1)%300!=0)
	{
		if(MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2)!=0||MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2_SQL)!=0) {
			writeCellOutput(currentTime);
		}
		if(MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2)!=0||MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2_SQL)!=0) {
			writeLAOutput(currentTime);
		}
	}
	if(MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2_SQL)!=0) {
		MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2_SQL)->getOStream() << ";" << endl;
	}
	if(MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2_SQL)!=0) {
		MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2_SQL)->getOStream() << ";" << endl;
	}
	// delete cells and las
	std::map< int, MSPhoneCell* >::iterator cit;
	for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
		delete cit->second;
	}
	std::map< int, MSPhoneLA* >::iterator lit;
	for (lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++) {
		delete lit->second;
	}
}


MSPhoneCell*
MSPhoneNet::getMSPhoneCell(int id)
{
	if (_mMSPhoneCells.find(id) != _mMSPhoneCells.end()) {
		return _mMSPhoneCells[id];
	}
	return 0;
}


MSPhoneCell*
MSPhoneNet::getCurrentVehicleCell(const std::string &id)
{
	MSPhoneCell * ret = 0;
	std::map< int, MSPhoneCell* >::iterator cit;
	for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end() ; cit++) {
		if (cit->second->hasCPhone(id)) {
			ret =  cit->second;
			break;
		}
	}
	return ret;
}


MSPhoneLA*
MSPhoneNet::getCurrentVehicleLA(const std::string &id)
{
	MSPhoneLA * ret = 0;
	std::map< int, MSPhoneLA* >::iterator lit;
	for (lit=_mMSPhoneLAs.begin(); lit!=_mMSPhoneLAs.end(); lit++) {
		if (lit->second->hasCall(id)) {
			ret = lit->second;
			break;
		}
	}
	return ret;
}


void
MSPhoneNet::addMSPhoneCell(int id)
{
	std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find(id);
	if (cit == _mMSPhoneCells.end()) {
		MSPhoneCell* c = new MSPhoneCell(id);
		_mMSPhoneCells[id] = c;
	}
}


void
MSPhoneNet::addMSPhoneCell(int id, int la)
{
	std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find(id);
	if (cit == _mMSPhoneCells.end()) {
		MSPhoneCell* c = new MSPhoneCell(id);
		_mMSPhoneCells[id] = c;
	}
	std::map< int, MSPhoneLA* >::iterator lit = _mMSPhoneLAs.find(la);
	if (lit == _mMSPhoneLAs.end()) {
		MSPhoneLA* l = new MSPhoneLA(la, 0);
		_mMSPhoneLAs[la] = l;
	}
	_mCell2LA[id] = la;
}


void
MSPhoneNet::connectLA2Cell(int cell_ID, int la_ID)
{
	if (_mMSPhoneCells.find(cell_ID) != _mMSPhoneCells.end() && _mMSPhoneLAs.find(la_ID) != _mMSPhoneLAs.end()) {
		_mCell2LA[cell_ID] = la_ID;
	}
}


void
MSPhoneNet::remMSPhoneCell(int id)
{
	std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find(id);
	if (cit != _mMSPhoneCells.end()) {
		delete cit->second;
		_mMSPhoneCells.erase(id);
	}
}


MSPhoneLA*
MSPhoneNet::getMSPhoneLA(int id)
{
	std::map<int, int>::iterator it;
	it = _mCell2LA.find(id);
	assert(it!=_mCell2LA.end());
	if (it != _mCell2LA.end()) {
		if (_mMSPhoneLAs.find(it->second) != _mMSPhoneLAs.end()) {
			return _mMSPhoneLAs[it->second];
		}
		return 0;
	}
	return 0;
}


void
MSPhoneNet::addMSPhoneLA(int id, int dir)
{
	MSPhoneLA* la = new MSPhoneLA(id, dir);
	_mMSPhoneLAs[id] = la;
}


void
MSPhoneNet::remMSPhoneLA(int id)
{
	std::map< int, MSPhoneLA* >::iterator lit = _mMSPhoneLAs.find(id);
	if (lit != _mMSPhoneLAs.end()) {
		delete lit->second;
		_mMSPhoneLAs.erase(id);
	}
}


SUMOTime
MSPhoneNet::writeCellOutput(SUMOTime t)
{
	// cell output / sql cell output
	OutputDevice *od1 = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2);
	OutputDevice *od2 = MSNet::getInstance()->getOutputDevice(MSNet::OS_CELL_TO_SS2_SQL);
	if ( od1 != 0 || od2 != 0 )
	{
		std::map< int, MSPhoneCell* >::iterator cit;
		for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
			cit->second->writeOutput(t);
		}
	}
	return 300;
}


SUMOTime
MSPhoneNet::writeLAOutput(SUMOTime t)
{
	// la output / sql la output
	OutputDevice *od1 = MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2);
	OutputDevice *od2 = MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2_SQL);
	/*if ( od1 != 0 || od2 != 0 )
	{
		std::map< int, MSPhoneLA* >::iterator lit;
		for (lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++) {
			lit->second->writeOutput(t);
		}
	}*/

	int intervall = t - lastTime;
	if ( od1 != 0 || od2 != 0 )
	{
		std::map< std::string, int >::const_iterator coit;
		for ( coit = myLAChanges.begin(); coit != myLAChanges.end(); coit++ )
		{
			if ( od1 != 0 )
			{
				std::string timestr="1970-01-01 " + StringUtils::toTimeString(t);
				od1->getOStream()
					<< "03;" << timestr << ';' << coit->first << ";0;" << coit->second
					<< ";0;" << intervall << "\n";
			}
			if ( od2 != 0 )
			{
				std::string timestr="1970-01-01 " + StringUtils::toTimeString(t);
				if (od2->getBoolMarker("hadFirstCall")) {
					od2->getOStream() << "," << endl;
				} else {
					od2->setBoolMarker("hadFirstCall", true);
				}
				od2->getOStream()
					<< "(NULL, \' \', '" << timestr << "'," << coit->first << ";0;" << coit->second 
					<< ";0;" << intervall << ")";
			}
		}
	}
	lastTime = t;
	myLAChanges.clear();
	return 300;
}


void 
MSPhoneNet::setDynamicCalls(SUMOTime time)
{
	std::map< int, MSPhoneCell* >::iterator cit;
	for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
		cit->second->setDynamicCalls(time);
	}
}

void
MSPhoneNet::addLAChange(const std::string & pos_id)
{
	std::map< std::string, int >::iterator it = myLAChanges.find(pos_id);
	if (it == myLAChanges.end()) {
		myLAChanges.insert(make_pair(pos_id, 1));
	} else {
		myLAChanges[pos_id]++;
	}
}



/****************************************************************************/

