/****************************************************************************/
/// @file    GUILoadThread.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class describing the thread that performs the loading of a simulation
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
#ifndef GUILoadThread_h
#define GUILoadThread_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <FXThread.h>
#include <utils/gui/windows/GUIAbstractLoadThread.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIEdgeControlBuilder;
class OptionsCont;
class GUIVehicleControl;
class GUINet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILoadThread
 */
class GUILoadThread : public GUIAbstractLoadThread
{
public:
    /// constructor
    GUILoadThread(MFXInterThreadEventClient *mw, MFXEventQue &eq,
                  FXEX::FXThreadEvent &ev);

    /// destructor
    virtual ~GUILoadThread();

    /** starts the thread
        the thread ends after the net has been loaded */
    FXint run();

protected:
    virtual GUIEdgeControlBuilder *buildEdgeBuilder();
    virtual GUIVehicleControl *buildVehicleControl();
    virtual bool initOptions();


    /** @brief Closes the loading process
     *
     * This method is called both on success and failure.
     * All message callbacks to this instance are removed and the parent
     * application is informed about the loading */
    void submitEndAndCleanup(GUINet *net, SUMOTime simStartTime, SUMOTime simEndTime);

};


#endif

/****************************************************************************/

