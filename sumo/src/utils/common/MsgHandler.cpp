/****************************************************************************/
/// @file    MsgHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process and gives them further to output
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include "MsgHandler.h"
#include "MsgRetriever.h"
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include "AbstractMutex.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// global variable definitions
// ===========================================================================
bool gSuppressWarnings = false;
bool gSuppressMessages = false;


// ===========================================================================
// static member variables
// ===========================================================================
MsgHandler *MsgHandler::myErrorInstance = 0;
MsgHandler *MsgHandler::myWarningInstance = 0;
MsgHandler *MsgHandler::myMessageInstance = 0;
bool MsgHandler::myAmProcessingProcess = false;
OutputDevice *MsgHandler::myLogFile = 0;
AbstractMutex *MsgHandler::myLock = 0;


// ===========================================================================
// method definitions
// ===========================================================================
MsgHandler *
MsgHandler::getMessageInstance()
{
    if (myMessageInstance==0) {
        myMessageInstance = new MsgHandler(MT_MESSAGE);
    }
    return myMessageInstance;
}


MsgHandler *
MsgHandler::getWarningInstance()
{
    if (myWarningInstance==0) {
        myWarningInstance = new MsgHandler(MT_WARNING);
    }
    return myWarningInstance;
}


MsgHandler *
MsgHandler::getErrorInstance()
{
    if (myErrorInstance==0) {
        myErrorInstance = new MsgHandler(MT_ERROR);
    }
    return myErrorInstance;
}


void
MsgHandler::inform(string msg, bool addType)
{
    if (myLock!=0) {
        myLock->lock();
    }
    msg = build(msg, addType);
    // report to cout if wished
    if (myReport2COUT) {
        if (myAmProcessingProcess) {
            cout << endl;
        }
        cout << msg << endl;
    }
    // report to cerr if wished
    if (myReport2CERR) {
        if (myAmProcessingProcess) {
            cerr << endl;
        }
        cerr << msg << endl;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    myAmProcessingProcess = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::progressMsg(string msg, bool addType)
{
    if (myLock!=0) {
        myLock->lock();
    }
    msg = build(msg, addType);
    // report to cout if wished
    if (myReport2COUT) {
        if (myAmProcessingProcess) {
            cout << endl;
        }
        cout << msg << (char) 13;
    }
    // report to cerr if wished
    if (myReport2CERR) {
        if (myAmProcessingProcess) {
            cout << endl;
        }
        cerr << msg << (char) 13;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    myAmProcessingProcess = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::beginProcessMsg(std::string msg, bool addType)
{
    if (myLock!=0) {
        myLock->lock();
    }
    msg = build(msg, addType);
    // report to cout if wished
    if (myReport2COUT) {
        cout << msg << ' ';
        cout.flush();
        myAmProcessingProcess = true;
    }
    // report to cerr if wished
    if (myReport2CERR) {
        cerr << msg << ' ';
        cerr.flush();
        myAmProcessingProcess = true;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg + " ");
        myAmProcessingProcess = true;
    }
    // set the information that something occured
    myWasInformed = true;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::endProcessMsg(std::string msg)
{
    if (myLock!=0) {
        myLock->lock();
    }
    // report to cout if wished
    if (myReport2COUT) {
        cout << msg << endl;
        myAmProcessingProcess = false;
    }
    // report to cerr if wished
    if (myReport2CERR) {
        cerr << msg << endl;
        myAmProcessingProcess = false;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg);
        myAmProcessingProcess = false;
    }
    // set the information that something occured
    myWasInformed = true;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::clear()
{
    if (myLock!=0) {
        myLock->lock();
    }
    myWasInformed = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::addRetriever(MsgRetriever *retriever)
{
    if (myLock!=0) {
        myLock->lock();
    }
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i==myRetrievers.end()) {
        myRetrievers.push_back(retriever);
    }
    // check whether the message shall be generated
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("suppress-warnings")
                            ? OptionsCont::getOptions().getBool("suppress-warnings")
                            : false;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = false;
    }
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::removeRetriever(MsgRetriever *retriever)
{
    if (myLock!=0) {
        myLock->lock();
    }
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i!=myRetrievers.end()) {
        myRetrievers.erase(i);
    }
    // check whether the message shall be generated
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("suppress-warnings")
                            ? OptionsCont::getOptions().getBool("suppress-warnings")
                            : myRetrievers.size()==0;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = !(myRetrievers.size()==0||myReport2COUT);
    }
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::report2cout(bool value)
{
    if (myLock!=0) {
        myLock->lock();
    }
    myReport2COUT = value;
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("suppress-warnings")
                            ? OptionsCont::getOptions().getBool("suppress-warnings")
                            : !myReport2COUT;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = myRetrievers.size()==0&&!myReport2COUT;
    }
    cout.setf(ios::fixed ,ios::floatfield);
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::report2cerr(bool value)
{
    if (myLock!=0) {
        myLock->lock();
    }
    myReport2CERR = value;
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("suppress-warnings")
                            ? OptionsCont::getOptions().getBool("suppress-warnings")
                            : !myReport2CERR;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = myRetrievers.size()==0&&!myReport2COUT;
    }
    cerr.setf(ios::fixed ,ios::floatfield);
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::initOutputOptions(bool gui)
{
    OptionsCont& oc = OptionsCont::getOptions();
    getMessageInstance()->report2cout(!gui && oc.getBool("verbose"));
    getWarningInstance()->report2cerr(!gui && !oc.getBool("suppress-warnings"));
    // build the logger if possible
    if (oc.isSet("log-file")) {
        try {
            myLogFile = &OutputDevice::getDevice(oc.getString("log-file"));
            getErrorInstance()->addRetriever(myLogFile);
            getWarningInstance()->addRetriever(myLogFile);
            getMessageInstance()->addRetriever(myLogFile);
        } catch (IOError &) {
            myLogFile = 0;
            throw ProcessError("Could not build logging file '" + oc.getString("log-file") + "'");
        }
    }
}


void
MsgHandler::cleanupOnEnd()
{
    if (myLock!=0) {
        myLock->lock();
    }
    myLogFile = 0;
    delete myMessageInstance;
    myMessageInstance = 0;
    delete myWarningInstance;
    myWarningInstance = 0;
    delete myErrorInstance;
    myErrorInstance = 0;
    if (myLock!=0) {
        myLock->unlock();
    }
}


MsgHandler::MsgHandler(MsgType type)
        : myType(type), myWasInformed(false), myReport2COUT(type==MT_MESSAGE),
        myReport2CERR(type!=MT_MESSAGE)
{}


MsgHandler::~MsgHandler()
{
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
    myLock = lock ;
}



/****************************************************************************/

