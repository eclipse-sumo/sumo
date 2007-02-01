/****************************************************************************/
/// @file    GUIThreadFactory.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id: $
///
//
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>
#include "GUIThreadFactory.h"
//#include "GUIApplicationWindow.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// member method definitions
// ===========================================================================
GUIThreadFactory::GUIThreadFactory()
{}


GUIThreadFactory::~GUIThreadFactory()
{}


GUILoadThread *
GUIThreadFactory::buildLoadThread(MFXInterThreadEventClient *mw,
                                  MFXEventQue &eq, FXEX::FXThreadEvent &ev)
{
    return new GUILoadThread(mw, eq, ev);
}


GUIRunThread *
GUIThreadFactory::buildRunThread(MFXInterThreadEventClient *mw,
                                 FXRealSpinDial &simDelay, MFXEventQue &eq,
                                 FXEX::FXThreadEvent &ev)
{
    return new GUIRunThread(mw, simDelay, eq, ev);
}



/****************************************************************************/

