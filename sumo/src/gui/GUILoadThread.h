#ifndef GUILoadThread_h
#define GUILoadThread_h
//---------------------------------------------------------------------------//
//                        GUILoadThread.h -
//  Class describing the thread that performs the loading of a simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.6  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane to come makes it necessary)
//
// Revision 1.4  2003/09/22 14:54:22  dkrajzew
// some refactoring on GUILoadThread-usage
//
// Revision 1.3  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <fx.h>
#include <FXThread.h>
#include <utils/foxtools/FXSingleEventThread.h>
#include <utils/foxtools/FXThreadEvent.h>


 /* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIApplicationWindow;
class MsgRetriever;
class MFXEventQue;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUILoadThread : public FXSingleEventThread
{
    // FOX-declarations
//    FXDECLARE(GUILoadThread)
public:

    /// constructor
    GUILoadThread(GUIApplicationWindow *mw, MFXEventQue &eq,
        FXEX::FXThreadEvent &ev);

    /// destructor
    ~GUILoadThread();

    /// begins the loading of the given file
    void load(const std::string &file);

    /** starts the thread
    	the thread ends after the net has been loaded */
    FXint run();

    /// Retrieves messages from the loading module
    void retrieveMessage(const std::string &msg);

    /// Retrieves warnings from the loading module
    void retrieveWarning(const std::string &msg);

    /// Retrieves error from the loading module
    void retrieveError(const std::string &msg);

private:
    /** @brief Closes the loading process
        This method is called both on success and failure.
        All message callbacks to this instance are removed and the parent
        application is informed about the loading */
    void submitEndAndCleanup(GUINet *net, std::ostream *craw,
        int simStartTime, int simEndTime);

private:
    /// the parent window to inform about the loading
    GUIApplicationWindow *myParent;

    /// the path to load the simulation from
    std::string _file;

    /** @brief The instances of message retriever encapsulations
        Needed to be deleted from the handler later on */
    MsgRetriever *myErrorRetriever, *myMessageRetriever, *myWarningRetreiver;

    MFXEventQue &myEventQue;

    FXEX::FXThreadEvent &myEventThrow;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

