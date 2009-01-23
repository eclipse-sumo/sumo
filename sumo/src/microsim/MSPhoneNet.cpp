/****************************************************************************/
/// @file    MSPhoneNet.cpp
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// The cellular network (GSM)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include "MSPhoneNet.h"
#include "MSCORN.h"
#include <iostream>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>
#include "MSPhoneCell.h"
#include "MSPhoneLA.h"
#include "MSNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneNet::MSPhoneNet()
{
    OptionsCont &oc = OptionsCont::getOptions();
    percentModus = oc.getBool("device.cell-phone.percent-of-activity");
    lastTime = 0;
}


MSPhoneNet::~MSPhoneNet()
{
    // close outputs
    SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    if (TMOD((currentTime-1),300)!=0) { // !!! compiles, but will fail with time being a real number due to inexact representation
        if (OptionsCont::getOptions().isSet("ss2-cell-output")||OptionsCont::getOptions().isSet("ss2-sql-cell-output")) {
            writeCellOutput(currentTime);
        }
        if (OptionsCont::getOptions().isSet("ss2-la-output")||OptionsCont::getOptions().isSet("ss2-sql-la-output")) {
            writeLAOutput(currentTime);
        }
    }
    if (OptionsCont::getOptions().isSet("ss2-sql-cell-output")) {
        OutputDevice::getDeviceByOption("ss2-sql-cell-output") << ";\n";
    }
    if (OptionsCont::getOptions().isSet("ss2-sql-la-output")) {
        OutputDevice::getDeviceByOption("ss2-sql-la-output") << ";\n";
    }
    // delete cells and las
    std::map< int, MSPhoneCell* >::iterator cit;
    for (cit = myMMSPhoneCells.begin(); cit != myMMSPhoneCells.end(); cit++) {
        delete cit->second;
    }
    std::map< int, MSPhoneLA* >::iterator lit;
    for (lit = myMMSPhoneLAs.begin(); lit != myMMSPhoneLAs.end(); lit++) {
        delete lit->second;
    }
}


MSPhoneCell*
MSPhoneNet::getMSPhoneCell(int id)
{
    if (myMMSPhoneCells.find(id) != myMMSPhoneCells.end()) {
        return myMMSPhoneCells[id];
    }
    return 0;
}


MSPhoneCell*
MSPhoneNet::getCurrentVehicleCell(const std::string &id)
{
    MSPhoneCell * ret = 0;
    std::map< int, MSPhoneCell* >::iterator cit;
    for (cit = myMMSPhoneCells.begin(); cit != myMMSPhoneCells.end() ; cit++) {
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
    for (lit=myMMSPhoneLAs.begin(); lit!=myMMSPhoneLAs.end(); lit++) {
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
    std::map< int, MSPhoneCell* >::iterator cit = myMMSPhoneCells.find(id);
    if (cit == myMMSPhoneCells.end()) {
        MSPhoneCell* c = new MSPhoneCell(id);
        myMMSPhoneCells[id] = c;
    }
}


void
MSPhoneNet::addMSPhoneCell(int id, int la)
{
    std::map< int, MSPhoneCell* >::iterator cit = myMMSPhoneCells.find(id);
    if (cit == myMMSPhoneCells.end()) {
        MSPhoneCell* c = new MSPhoneCell(id);
        myMMSPhoneCells[id] = c;
    }
    std::map< int, MSPhoneLA* >::iterator lit = myMMSPhoneLAs.find(la);
    if (lit == myMMSPhoneLAs.end()) {
        MSPhoneLA* l = new MSPhoneLA(la, 0);
        myMMSPhoneLAs[la] = l;
    }
    myMCell2LA[id] = la;
}


void
MSPhoneNet::connectLA2Cell(int cell_ID, int la_ID)
{
    if (myMMSPhoneCells.find(cell_ID) != myMMSPhoneCells.end() && myMMSPhoneLAs.find(la_ID) != myMMSPhoneLAs.end()) {
        myMCell2LA[cell_ID] = la_ID;
    }
}


void
MSPhoneNet::remMSPhoneCell(int id)
{
    std::map< int, MSPhoneCell* >::iterator cit = myMMSPhoneCells.find(id);
    if (cit != myMMSPhoneCells.end()) {
        delete cit->second;
        myMMSPhoneCells.erase(id);
    }
}


MSPhoneLA*
MSPhoneNet::getMSPhoneLA(int id)
{
    std::map<int, int>::iterator it;
    it = myMCell2LA.find(id);
    assert(it!=myMCell2LA.end());
    if (it != myMCell2LA.end()) {
        if (myMMSPhoneLAs.find(it->second) != myMMSPhoneLAs.end()) {
            return myMMSPhoneLAs[it->second];
        }
        return 0;
    }
    return 0;
}


void
MSPhoneNet::addMSPhoneLA(int id, int dir)
{
    MSPhoneLA* la = new MSPhoneLA(id, dir);
    myMMSPhoneLAs[id] = la;
}


void
MSPhoneNet::remMSPhoneLA(int id)
{
    std::map< int, MSPhoneLA* >::iterator lit = myMMSPhoneLAs.find(id);
    if (lit != myMMSPhoneLAs.end()) {
        delete lit->second;
        myMMSPhoneLAs.erase(id);
    }
}


SUMOTime
MSPhoneNet::writeCellOutput(SUMOTime t)
{
    // cell output / sql cell output
    if (OptionsCont::getOptions().isSet("ss2-cell-output")||OptionsCont::getOptions().isSet("ss2-sql-cell-output")) {
        std::map< int, MSPhoneCell* >::iterator cit;
        for (cit = myMMSPhoneCells.begin(); cit != myMMSPhoneCells.end(); cit++) {
            cit->second->writeOutput(t);
        }
    }
    return 300;
}


SUMOTime
MSPhoneNet::writeLAOutput(SUMOTime t)
{
    // la output / sql la output
    /*OutputDevice *od1 = MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2);
    OutputDevice *od2 = MSNet::getInstance()->getOutputDevice(MSNet::OS_LA_TO_SS2_SQL);
    if ( od1 != 0 || od2 != 0 )
    {
    	std::map< int, MSPhoneLA* >::iterator lit;
    	for (lit = myMMSPhoneLAs.begin(); lit != myMMSPhoneLAs.end(); lit++) {
    		lit->second->writeOutput(t);
    	}
    }*/

    SUMOTime intervall = t - lastTime;
    if (OptionsCont::getOptions().isSet("ss2-la-output")||OptionsCont::getOptions().isSet("ss2-sql-la-output")) {
        std::map< std::string, int >::const_iterator coit;
        for (coit = myLAChanges.begin(); coit != myLAChanges.end(); coit++) {
            if (OptionsCont::getOptions().isSet("ss2-la-output")) {
                std::string timestr= OptionsCont::getOptions().getString("device.cell-phone.sql-date");
                timestr = timestr + " " + StringUtils::toTimeString((int) t);
                OutputDevice::getDeviceByOption("ss2-la-output")
                << "03;" << timestr << ';' << coit->first << ";0;" << coit->second
                << ";0;" << intervall << "\n";
            }
            if (OptionsCont::getOptions().isSet("ss2-sql-la-output")) {
                OutputDevice& od = OutputDevice::getDeviceByOption("ss2-sql-la-output");
                std::string timestr= OptionsCont::getOptions().getString("device.cell-phone.sql-date");
                timestr = timestr + " " + StringUtils::toTimeString((int) t);
                if (od.getBoolMarker("hadFirstCall")) {
                    od << ",\n";
                } else {
                    od.setBoolMarker("hadFirstCall", true);
                }
                od
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
    for (cit = myMMSPhoneCells.begin(); cit != myMMSPhoneCells.end(); cit++) {
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

