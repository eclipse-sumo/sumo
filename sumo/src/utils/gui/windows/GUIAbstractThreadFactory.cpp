//---------------------------------------------------------------------------//
//                        GUIAbstractThreadFactory.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
// Revision 1.2  2005/06/14 11:29:50  dksumo
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>
#include "GUIAbstractThreadFactory.h"
//#include "GUIMainWindow.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIAbstractThreadFactory::GUIAbstractThreadFactory()
{
}


GUIAbstractThreadFactory::~GUIAbstractThreadFactory()
{
}


GUILoadThread *
GUIAbstractThreadFactory::buildLoadThread(GUIMainWindow *mw,
                                  MFXEventQue &eq, FXEX::FXThreadEvent &ev)
{
    return new GUILoadThread(mw, eq, ev);
}


GUIRunThread *
GUIAbstractThreadFactory::buildRunThread(GUIMainWindow *mw,
                                 FXRealSpinDial &simDelay, MFXEventQue &eq,
                                 FXEX::FXThreadEvent &ev)
{
    return new GUIRunThread(mw, simDelay, eq, ev);
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
