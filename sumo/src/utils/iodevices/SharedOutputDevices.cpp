//---------------------------------------------------------------------------//
//                        OutputDevice_COUT.cpp -
//  An output device that encapsulates cout
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2004/08/02 13:01:16  dkrajzew
// documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <fstream>
#include <string>
#include "SharedOutputDevices.h"
#include "OutputDevice.h"
#include "OutputDevice_File.h"
#include <utils/common/UtilExceptions.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
SharedOutputDevices *SharedOutputDevices::myInstance = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
SharedOutputDevices *
SharedOutputDevices::getInstance()
{
    if(myInstance==0) {
        myInstance = new SharedOutputDevices();
    }
    return myInstance;
}


SharedOutputDevices::~SharedOutputDevices()
{
    for(FileMap::iterator i=myOutputFiles.begin(); i!=myOutputFiles.end(); ++i) {
        delete (*i).second;
    }
    myOutputFiles.clear();
    myInstance = 0;
}


OutputDevice *
SharedOutputDevices::getOutputFile(const std::string &name)
{
    FileMap::iterator i = myOutputFiles.find(name);
    if(i!=myOutputFiles.end()) {
        return (*i).second;
    }
    std::ofstream *strm = new std::ofstream(name.c_str());
    if(!strm->good()) {
        delete strm;
        throw ProcessError();
    }
    OutputDevice *dev = new OutputDevice_File(strm);
    myOutputFiles[name] = dev;
    return dev;
}


SharedOutputDevices::SharedOutputDevices()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
