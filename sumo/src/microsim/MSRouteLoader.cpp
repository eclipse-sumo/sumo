/***************************************************************************
                          MSRouteLoader.cpp
			  A class that performs the loading of routes
                             -------------------
    project              : SUMO
    begin                : Wed, 6 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/03/17 14:15:55  dkrajzew
// first steps of network reinitialisation implemented
//
// Revision 1.1  2003/02/07 10:41:50  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <helpers/PreStartInitialised.h>
#include "MSNet.h"
#include "MSRouteHandler.h"
#include "MSRouteLoader.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSRouteLoader::MSRouteLoader(const std::string &file,
                             MSNet &net)
    : MSRouteHandler(file, false),
    myParser(0), _moreAvailable(true)//, _nextRead(false),
{
    myParser = XMLReaderFactory::createXMLReader();
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/namespaces" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema-full-checking"),
        false );
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/validation"), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/dynamic" ), false );
    myParser->setContentHandler(this);
    myParser->setErrorHandler(this);
}


MSRouteLoader::~MSRouteLoader()
{
    delete myParser;
}


void
MSRouteLoader::init()
{
    myLastDepart = 0;
    _moreAvailable = true;
    myLastReadVehicle = 0;
    if(!myParser->parseFirst(_file.c_str(), myToken)) {
        MsgHandler::getErrorInstance()->inform(
            string("Can not read XML-file '") + _file + string("'."));
        throw ProcessError();
    }
}


void
MSRouteLoader::loadUntil(MSNet::Time time, MSVehicleContainer &into)
{
    // read only when further data is available, no error occured
    //  and vehicles may be found in the between the departure time of
    //  the last read vehicle and the time to read until
    if(!_moreAvailable || time < myLastDepart || errorOccured()) {
        return;
    }

    // if a vehicle was read before the call but was not yet added,
    //  add it now
    if(myLastReadVehicle!=0) {
        into.add(myLastReadVehicle);
        myLastReadVehicle = 0;
    }
    // read vehicles until specified time or the period to read vehicles
    //  until is reached
    while(myParser->parseNext(myToken)) {
        // return when the last read vehicle is beyond the period
        if(myLastDepart>=time) {
            return;
        }
        // otherwise add the last vehicle read (if any)
        if(myLastReadVehicle!=0) {
            into.add(myLastReadVehicle);
            myLastReadVehicle = 0;
        }
    }

    // no data are available anymore
    _moreAvailable = false;
    return;
}


bool
MSRouteLoader::moreAvailable() const
{
    return _moreAvailable && !errorOccured();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSRouteLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:
