/****************************************************************************/
/// @file    GNEViewParent.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 15 Dec 2004
/// @version $Id$
///
// A window that controls the display(s) of the simulation
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
#ifndef GNEViewParent_h
#define GNEViewParent_h


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
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
#include <gui/dialogs/GUIDialog_GLObjChooser.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <gui/GUISUMOViewParent.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUISUMOAbstractView;
class GUIApplicationWindow;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class GNEViewParent
 * This class represents a single view on the application. It is made of a
 * tool-bar containing a field to change the type of display, buttons that
 * allow to choose an artifact and some other view controlling options.
 * The rest of the window is a canvas that contains the display itself
 */
class GNEViewParent : public GUISUMOViewParent
{
    // FOX-declarations
    FXDECLARE(GNEViewParent)
public:
    /// constructor
    GNEViewParent(FXMDIClient* p, FXGLCanvas *share,
                  FXMDIMenu *mdimenu, const FXString& name,
                  GUINet &net, GUIMainWindow *parentWindow, GUISUMOViewParent::ViewType view,
                  FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
                 );

    void init(GUISUMOViewParent::ViewType view, FXGLCanvas *share, GUINet &net);


    /// destructor
    ~GNEViewParent();

    GUIMainWindow *getParent()
    {
        return myParent;
    }

    FXGroupBox *getEditGroupBox();



protected:

    /** builds the edit frame  */
    void buildEditFrame(FXComposite *c);

    /** build the artifact choosing toolbar */
    void buildTrackingTools();

private:

    //Editor Frame Widgets
    FXGroupBox  *groupBox;
    FXTextField *JunctionNameTextField;
    FXTextField *xPosTextField;
    FXTextField *yPosTextField;
    FXTextField *LaneNameTextField;
    FXTextField *LaneStartTextField;
    FXTextField *LaneEndTextField;
    FXTextField *MaxSpeedTextField;
    FXTextField *LengthTextField;



protected:
    GNEViewParent()
    { }

};


#endif

/****************************************************************************/

