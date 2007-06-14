/****************************************************************************/
/// @file    GUIThreadFactory.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
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
#ifndef GUIThreadFactory_h
#define GUIThreadFactory_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>


class GUIApplicationWindow;
class MFXInterThreadEventClient;
class GUILoadThread;
class GUIRunThread;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIThreadFactory 
 */
class GUIThreadFactory
{
public:
    GUIThreadFactory();
    virtual ~GUIThreadFactory();
    virtual GUILoadThread *buildLoadThread(MFXInterThreadEventClient *mw,
                                           MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    virtual GUIRunThread *buildRunThread(MFXInterThreadEventClient *mw,
                                         FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);


};


#endif

/****************************************************************************/

