#ifndef GUIAbstractThreadFactory_h
#define GUIAbstractThreadFactory_h
//---------------------------------------------------------------------------//
//                        GUIAbstractThreadFactory.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
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
// $Log$
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
 * class declarations
 * ======================================================================= */

class GUIMainWindow;
class GUILoadThread;
class GUIRunThread;


/* =========================================================================
 * included modules
 * ======================================================================= */

#include <utils/foxtools/FXRealSpinDial.h>

#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIAbstractThreadFactory {
public:
    GUIAbstractThreadFactory();
    virtual ~GUIAbstractThreadFactory();
    virtual GUILoadThread *buildLoadThread(GUIMainWindow *mw,
        MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    virtual GUIRunThread *buildRunThread(GUIMainWindow *mw,
        FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);


};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
