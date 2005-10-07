#ifndef GUIGlChildWindow_h
#define GUIGlChildWindow_h
//---------------------------------------------------------------------------//
//                        GUIGlChildWindow.h -
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
// Revision 1.5  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:52:07  dkrajzew
// code style adapted
//
// Revision 1.3  2005/06/14 11:29:51  dksumo
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <fx3d.h>

#include "GUISUMOAbstractView.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIGlChildWindow : public FXMDIChild {
public:
    GUIGlChildWindow( FXMDIClient* p, FXMDIMenu *mdimenu, const FXString& name,
		FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
        );

	virtual ~GUIGlChildWindow();

	virtual FXGLCanvas *getBuildGLCanvas() const;

	virtual bool showLegend() const = 0;
	virtual bool allowRotation() const = 0;

    FXToolBar &getToolBar(GUISUMOAbstractView &v);

protected:
    /// the view
    GUISUMOAbstractView *_view;

    /// The tool bar
    FXToolBar *myToolBar;

protected:
	GUIGlChildWindow() { }

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
