#ifndef GUIMainWindow_h
#define GUIMainWindow_h
//---------------------------------------------------------------------------//
//                        GUIMainWindow.h -
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
// Revision 1.7  2005/11/09 06:46:34  dkrajzew
// added cursor position output (unfinished)
//
// Revision 1.6  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 12:52:07  dkrajzew
// code style adapted
//
// Revision 1.4  2005/06/14 11:29:51  dksumo
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
#include <vector>
#include <utils/foxtools/FXMutex.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIMainWindow : public FXMainWindow {
public:
	GUIMainWindow(FXApp* a, int glWidth, int glHeight);
	virtual ~GUIMainWindow();
    /// Adds a further child window to the list
    void addChild(FXMDIChild *child, bool updateOnSimStep=true);
    void addChild(FXMainWindow *child, bool updateOnSimStep=true);

    /// removes the given child window from the list
    void removeChild(FXMDIChild *child);
    void removeChild(FXMainWindow  *child);

    void updateChildren();

	FXFont *getBoldFont();

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

	FXGLVisual *getGLVisual() const;

	virtual FXGLCanvas *getBuildGLCanvas() const = 0;

    virtual size_t getCurrentSimTime() const = 0;

    virtual void loadSelection(const std::string &file) const = 0;

    virtual void setStatusBarText(const std::string &) { }

protected:
	std::vector<FXMDIChild*> mySubWindows;
    std::vector<FXMainWindow*> myTrackerWindows;
    /// A lock to make the removal and addition of trackers secure
    FXEX::FXMutex myTrackerLock;

    /// Font used for popup-menu titles
    FXFont *myBoldFont;

    /// The openGL-maximum screen sizes
    int myGLWidth, myGLHeight;

    /// The multi view panel
    FXMDIClient *myMDIClient;


    /// The gl-visual used
    FXGLVisual *myGLVisual;

    FXDockSite *myTopDock, *myBottomDock, *myLeftDock, *myRightDock;

    bool myRunAtBegin;

protected:
	GUIMainWindow() { }

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
