/****************************************************************************/
/// @file    GUIAbstractThreadFactory.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
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
#ifndef GUIAbstractThreadFactory_h
#define GUIAbstractThreadFactory_h


// ===========================================================================
// class declarations
// ===========================================================================

class GUILoadThread;
class GUIRunThread;
class MFXInterThreadEventClient;
class MFXEventQue;


// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GUIAbstractThreadFactory
{
public:
    GUIAbstractThreadFactory();
    virtual ~GUIAbstractThreadFactory();
    virtual GUILoadThread *buildLoadThread(MFXInterThreadEventClient *mw,
                                           MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    virtual GUIRunThread *buildRunThread(MFXInterThreadEventClient *mw,
                                         FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);


};


#endif

/****************************************************************************/

