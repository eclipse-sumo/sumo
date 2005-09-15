#ifndef GUIAbstractLoadThread_h
#define GUIAbstractLoadThread_h
//---------------------------------------------------------------------------//
//                        GUIAbstractLoadThread.h -
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
// Revision 1.2  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:57  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.8  2004/07/02 08:28:50  dkrajzew
// some changes needed to derive the threading classes more easily added
//
// Revision 1.7  2004/04/02 11:10:20  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.6  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane to come makes it necessary)
//
// Revision 1.4  2003/09/22 14:54:22  dkrajzew
// some refactoring on GUIAbstractLoadThread-usage
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
#include <config.h>
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
class GUIMainWindow;
class MsgRetriever;
class MFXEventQue;
class GUIEdgeControlBuilder;
class NLEdgeControlBuilder;
class NLJunctionControlBuilder;
class GUINetBuilder;
class OptionsCont;
class GUIVehicleControl;
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIAbstractLoadThread : public FXSingleEventThread
{
public:

    /// constructor
    GUIAbstractLoadThread(GUIMainWindow *mw, MFXEventQue &eq,
        FXEX::FXThreadEvent &ev);

    /// destructor
    virtual ~GUIAbstractLoadThread();

    /// begins the loading of the given file
    virtual void load(const std::string &file);

    /** starts the thread
        the thread ends after the net has been loaded */
    virtual FXint run();

    /// Retrieves messages from the loading module
    void retrieveMessage(const std::string &msg);

    /// Retrieves warnings from the loading module
    void retrieveWarning(const std::string &msg);

    /// Retrieves error from the loading module
    void retrieveError(const std::string &msg);

    const std::string &getFileName() const;

protected:
    virtual bool initOptions();

protected:
    /// the parent window to inform about the loading
    GUIMainWindow *myParent;

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

