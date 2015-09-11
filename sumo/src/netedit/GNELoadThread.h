/****************************************************************************/
/// @file    GNELoadThread.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The thread that performs the loading of a Netedit-net (adapted from
// GUILoadThread)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNELoadThread_h
#define GNELoadThread_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <utils/foxtools/FXSingleEventThread.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/common/MsgHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MFXInterThreadEventClient;
class GNENet;
class GUIEvent;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNELoadThread
 */
class GNELoadThread : public FXSingleEventThread {
public:
    /// constructor
    GNELoadThread(FXApp* app, MFXInterThreadEventClient* mw, MFXEventQue<GUIEvent*>& eq,
                  FXEX::FXThreadEvent& ev);

    /// destructor
    virtual ~GNELoadThread();

    /** starts the thread
        the thread ends after the net has been loaded */
    FXint run();

    /** @brief begins the loading of a netconvert configuration or a a network
     * @param[in] file The network or configuration-file to be loaded
     * @param[in] isNet whether file is a network file
     * @param[in] optionsReady whether options are already initialized
     */
    void loadConfigOrNet(const std::string& file, bool isNet, bool optionsReady = false, bool newNet = false);

    /// Retrieves messages from the loading module
    void retrieveMessage(const MsgHandler::MsgType type, const std::string& msg);

    /// @brief clears and initializes the OptionsCont
    static void fillOptions(OptionsCont& oc);

    /// @brief sets required options for proper functioning
    static void setDefaultOptions(OptionsCont& oc);

protected:
    bool initOptions();


    /** @brief Closes the loading process
     *
     * This method is called both on success and failure.
     * All message callbacks to this instance are removed and the parent
     * application is informed about the loading */
    void submitEndAndCleanup(GNENet* net, const std::string& guiSettingsFile);

protected:
    /// the parent window to inform about the loading
    MFXInterThreadEventClient* myParent;

    /// the path to load the simulation from
    std::string myFile;

    /** @brief The instances of message retriever encapsulations
        Needed to be deleted from the handler later on */
    OutputDevice* myErrorRetriever, *myMessageRetriever, *myWarningRetriever;

    MFXEventQue<GUIEvent*>& myEventQue;

    FXEX::FXThreadEvent& myEventThrow;

    /// Information whether only the network shall be loaded
    bool myLoadNet;

    /// @brief if true, options will not be read from myFile
    bool myOptionsReady;

    /// @brief if true, a new net is created
    bool myNewNet;
};


#endif

/****************************************************************************/
