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
// Revision 1.2  2003/07/07 08:44:43  dkrajzew
// a new interface for a joined output of messages added
//
// Revision 1.1  2003/06/18 11:22:56  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>


class MsgRetriever;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MsgHandler
 */
class MsgHandler {
public:
    enum MsgType {
        MT_MESSAGE,
        MT_WARNING,
        MT_ERROR
    };


    static MsgHandler *getMessageInstance();

    static MsgHandler *getWarningInstance();

    static MsgHandler *getErrorInstance();

    static void cleanupOnEnd();

    /// adds a new error to the list
    void inform(const std::string &error);

    /// closes a sublist of information
    void finalizeInform(const std::string &msg) {
        inform(msg);
    }

    /// Clears information whether an error occured previously
    void clear();

    ///
    void addRetriever(MsgRetriever *retriever);

    void removeRetriever(MsgRetriever *retriever);

    void report2cout(bool value);

    bool wasInformed() const;

private:
    /// standard constructor
    MsgHandler(MsgType type);

    /// destructor
    ~MsgHandler();

private:
    static MsgHandler *myErrorInstance;

    static MsgHandler *myWarningInstance;

    static MsgHandler *myMessageInstance;

private:
    MsgType myType;

    /// information wehther an error occured at all
    bool      myWasInformed;

    bool myReport2COUT;

    /// Definition of the list of retrievers to inform
    typedef std::vector<MsgRetriever*> RetrieverVector;

    /// The list of retrievers that shall be informed about new messages or errors
    RetrieverVector myRetrievers;

private:
    /** invalid copy constructor */
    MsgHandler(const MsgHandler &s);

    /** invalid assignment operator */
    MsgHandler &operator=(const MsgHandler &s);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MsgHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

