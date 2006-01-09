#ifndef MsgHandler_h
#define MsgHandler_h
/***************************************************************************
                          MsgHandler.h  -
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
// Revision 1.9  2006/01/09 13:30:45  dkrajzew
// debugging error handling
//
// Revision 1.8  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:58:33  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.4  2003/12/04 13:07:35  dkrajzew
// interface changed to allow message building on the fly
//
// Revision 1.3  2003/11/26 09:51:10  dkrajzew
// changes to allow more than a single function of an object to
//  retrieve messages
//
// Revision 1.2  2003/07/07 08:44:43  dkrajzew
// a new interface for a joined output of messages added
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
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MsgRetriever;
class AbstractMutex;


/* =========================================================================
 * global variable definitions
 * ======================================================================= */
extern bool gSuppressWarnings;
extern bool gSuppressMessages;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MsgHandler
 */
class MsgHandler {
public:
    /**
     * @enum MsgType
     * An enumeration to differ between different types of messages
     * (errors, warning and information)
     */
    enum MsgType {
        /// The message is only something to show
        MT_MESSAGE,
        /// The message is a warning
        MT_WARNING,
        /// The message is an error
        MT_ERROR
    };

    /// Returns the instance to add normal messages to
    static MsgHandler *getMessageInstance();

    /// Returns the instance to add warnings to
    static MsgHandler *getWarningInstance();

    /// Returns the instance to add errors to
    static MsgHandler *getErrorInstance();

    /// Removes pending handler
    static void cleanupOnEnd();

    /// adds a new error to the list
    void inform(std::string error, bool addType=true);

    /// closes a sublist of information
    void finalizeInform(std::string msg) {
        inform(msg);
    }

    /// Clears information whether an error occured previously
    void clear();

    /// Adds a further retriever to the instance responsible for a certain msg type
    void addRetriever(MsgRetriever *retriever);

    /// Removes the retriever from the
    void removeRetriever(MsgRetriever *retriever);

    /// Sets the information whether stdout shall be used as output device, too
    void report2cout(bool value);

    /// Sets the information whether stderr shall be used as output device
    void report2cerr(bool value);

    /// Returns the information whether any messages were added
    bool wasInformed() const;

    /** @brief Sets the lock to use
        The lock will not be deleted */
    void assignLock(AbstractMutex *lock);

private:
    /// standard constructor
    MsgHandler(MsgType type);

    /// destructor
    ~MsgHandler();

private:
    /// The instance to handle errors
    static MsgHandler *myErrorInstance;

    /// The instance to handle warnings
    static MsgHandler *myWarningInstance;

    /// The instance to handle normal messages
    static MsgHandler *myMessageInstance;

private:
    /// The type of the instance
    MsgType myType;

    /// information wehther an error occured at all
    bool myWasInformed;

    /// Information whether stdout shall be used as output device, too
    bool myReport2COUT;

    /// Information whether stderr shall be used as output device, too
    bool myReport2CERR;

    /// Definition of the list of retrievers to inform
    typedef std::vector<MsgRetriever*> RetrieverVector;

    /// The list of retrievers that shall be informed about new messages or errors
    RetrieverVector myRetrievers;

    /** @brief The lock if any has to be used
        The lock will not be deleted */
    AbstractMutex *myLock;

private:
    /** invalid copy constructor */
    MsgHandler(const MsgHandler &s);

    /** invalid assignment operator */
    MsgHandler &operator=(const MsgHandler &s);

};


/* =========================================================================
 * global definitions
 * ======================================================================= */
#define WRITE_WARNING(command) if(!gSuppressWarnings) { MsgHandler::getWarningInstance()->inform(command); }
#define WRITE_MESSAGE(command) if(!gSuppressMessages) { MsgHandler::getMessageInstance()->inform(command); }


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

