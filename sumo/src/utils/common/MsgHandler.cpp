/***************************************************************************
                          MsgHandler.cpp -
    Retrieves messages about the process and gives them further to output
    units
                             -------------------
    project              : SUMO
    begin                : Tue, 17 Jun 2003
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.8  2005/10/17 09:22:36  dkrajzew
// memory leaks removed
//
// Revision 1.7  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:58:33  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.3  2003/12/04 13:07:35  dkrajzew
// interface changed to allow message building on the fly
//
// Revision 1.2  2003/06/24 14:40:22  dkrajzew
// Error/Warning-prefix added to all messages; endlines are now prompted
//  correctly within log-files
//
// Revision 1.1  2003/06/18 11:22:56  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include "MsgHandler.h"
#include "MsgRetriever.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "AbstractMutex.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * global variable definitions
 * ======================================================================= */
bool gSuppressWarnings = false;
bool gSuppressMessages = false;


/* =========================================================================
 * static member variables
 * ======================================================================= */
MsgHandler *MsgHandler::myErrorInstance = 0;
MsgHandler *MsgHandler::myWarningInstance = 0;
MsgHandler *MsgHandler::myMessageInstance = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MsgHandler *
MsgHandler::getMessageInstance()
{
    if(myMessageInstance==0) {
        myMessageInstance = new MsgHandler(MT_MESSAGE);
    }
    return myMessageInstance;
}


MsgHandler *
MsgHandler::getWarningInstance()
{
    if(myWarningInstance==0) {
        myWarningInstance = new MsgHandler(MT_WARNING);
    }
    return myWarningInstance;
}


MsgHandler *
MsgHandler::getErrorInstance()
{
    if(myErrorInstance==0) {
        myErrorInstance = new MsgHandler(MT_ERROR);
    }
    return myErrorInstance;
}


void
MsgHandler::inform(std::string error)
{
    if(myLock!=0) {
        myLock->lock();
    }
    switch(myType) {
    case MT_MESSAGE:
        break;
    case MT_WARNING:
        error = "Warning: " + error;
        break;
    case MT_ERROR:
        error = "Error: " + error;
        break;
    default:
        throw 1;
    }
    // report to cour if wished
    if(myReport2COUT) {
        cout << error << endl;
    }
    // inform all other receivers
    for(RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(error);
    }
    // set the information that something occured
    myWasInformed = true;
    if(myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::clear()
{
    myWasInformed = false;
}


void
MsgHandler::addRetriever(MsgRetriever *retriever)
{
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if(i==myRetrievers.end()) {
        myRetrievers.push_back(retriever);
    }
    // check whether the message shall be generated
    if(myType==MT_WARNING) {
        gSuppressWarnings = OptionsSubSys::getOptions().exists("suppress-warnings")
            ? OptionsSubSys::getOptions().getBool("suppress-warnings")
            : false;
    } else if(myType==MT_MESSAGE) {
        gSuppressMessages = false;
    }
}


void
MsgHandler::removeRetriever(MsgRetriever *retriever)
{
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if(i!=myRetrievers.end()) {
        myRetrievers.erase(i);
    }
    // check whether the message shall be generated
    // check whether the message shall be generated
    if(myType==MT_WARNING) {
        gSuppressWarnings = OptionsSubSys::getOptions().exists("suppress-warnings")
            ? OptionsSubSys::getOptions().getBool("suppress-warnings")
            : myRetrievers.size()==0;
    } else if(myType==MT_MESSAGE) {
        gSuppressMessages =
            !(myRetrievers.size()==0||myReport2COUT);
    }
}


void
MsgHandler::report2cout(bool value)
{
    myReport2COUT = value;
    if(myType==MT_WARNING) {
        gSuppressWarnings = OptionsSubSys::getOptions().exists("suppress-warnings")
            ? OptionsSubSys::getOptions().getBool("suppress-warnings")
            : !myReport2COUT;
    } else if(myType==MT_MESSAGE) {
        gSuppressMessages =
            !(myRetrievers.size()==0||myReport2COUT);
    }
}


void
MsgHandler::cleanupOnEnd()
{
    delete myMessageInstance;
    myMessageInstance = 0;
    delete myWarningInstance;
    myWarningInstance = 0;
    delete myErrorInstance;
    myErrorInstance = 0;
}


MsgHandler::MsgHandler(MsgType type)
    : myType(type), myWasInformed(false), myReport2COUT(true),
    myLock(0)
{
}


MsgHandler::~MsgHandler()
{
//    delete myLock;
}


bool
MsgHandler::wasInformed() const
{
    return myWasInformed;
}


void
MsgHandler::assignLock(AbstractMutex *lock)
{
    assert(myLock==0);
    myLock = lock;
}
