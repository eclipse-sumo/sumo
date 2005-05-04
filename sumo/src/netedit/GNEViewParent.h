#ifndef GNEViewParent_h
#define GNEViewParent_h
//---------------------------------------------------------------------------//
//                        GNEViewParent.h -
//  A window that controls the display(s) of the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 15 Dec 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2005/05/04 08:37:26  dkrajzew
// ported to fox1.4
//
// Revision 1.1  2004/12/15 09:20:19  dkrajzew
// made guisim independent of giant/netedit
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <fx.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
#include <gui/dialogs/GUIDialog_GLObjChooser.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <gui/GUISUMOViewParent.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class GUISUMOAbstractView;
class GUIApplicationWindow;


/* =========================================================================
 * class declarations
 * ======================================================================= */
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
    GNEViewParent( FXMDIClient* p, FXGLCanvas *share,
        FXMDIMenu *mdimenu, const FXString& name,
        GUINet &net, GUIMainWindow *parentWindow, GUISUMOViewParent::ViewType view,
        FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
        );

    void init(GUISUMOViewParent::ViewType view, FXGLCanvas *share, GUINet &net);


    /// destructor
    ~GNEViewParent();

    GUIMainWindow *getParent() { return myParent; }

	FXGroupBox *getEditGroupBox();



protected:
    /** builds the toolbar  */
    void buildToolBar(FXComposite *c);

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
    GNEViewParent() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

