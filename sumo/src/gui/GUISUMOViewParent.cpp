//---------------------------------------------------------------------------//
//                        GUISUMOViewParent.cpp -
//  A window that controls the display(s) of the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.12  2004/12/12 17:23:58  agaubatz
// Editor Tool Widgets included
//
// Revision 1.11  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.10  2004/08/02 11:55:07  dkrajzew
// added the possibility to take snapshots
//
// Revision 1.9  2004/07/02 08:37:27  dkrajzew
// using global selection storage
//
// Revision 1.8  2004/04/02 11:11:24  dkrajzew
// visualisation whether an item is selected added
//
// Revision 1.7  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.6  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.5  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.4  2003/07/18 12:29:28  dkrajzew
// removed some warnings
//
// Revision 1.3  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.2  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.1  2003/05/20 09:25:14  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.6  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display
//  size added
//
// Revision 1.5  2003/04/14 08:24:56  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept
//  implemented; comments added
//
// Revision 1.4  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/12 16:55:18  dkrajzew
// centering of objects debugged
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSJunction.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>

#include <string>
#include <vector>
#include "GUIGlobals.h"
#include "dialogs/GUIDialog_GLObjChooser.h"
#include "GUIViewTraffic.h"
#include "GUIViewAggregatedLanes.h"
#include "GUIApplicationWindow.h"
#include "GUISUMOViewParent.h"
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/div/GUIIOGlobals.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUISUMOViewParent) GUISUMOViewParentMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTERVIEW,   GUISUMOViewParent::onCmdRecenterView),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWLEGEND,     GUISUMOViewParent::onCmdShowLegend),
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,   GUISUMOViewParent::onCmdMakeSnapshot),
    FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,  GUISUMOViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUISUMOViewParent::onCmdLocateJunction),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUISUMOViewParent::onCmdLocateEdge),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUISUMOViewParent::onCmdLocateVehicle),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUISUMOViewParent::onCmdLocateAdd),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMSTEP,        GUISUMOViewParent::onSimStep),

};

// Object implementation
FXIMPLEMENT(GUISUMOViewParent, FXMDIChild, GUISUMOViewParentMap, ARRAYNUMBER(GUISUMOViewParentMap))


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUISUMOViewParent::GUISUMOViewParent(FXMDIClient* p,
                                     FXGLCanvas *share,FXMDIMenu *mdimenu,
                                     const FXString& name, GUINet &net,
                                     GUIMainWindow *parentWindow,
                                     ViewType view, FXIcon* ic, FXPopup* pup,
                                     FXuint opts,
                                     FXint x, FXint y, FXint w, FXint h)
    : GUIGlChildWindow( p, mdimenu, name, ic, 0, opts, 10, 10, 300, 200 ),
    _zoomingFactor(100),
    _showLegend(true), _allowRotation(false), _chooser(0),
    myParent(parentWindow)
{
    init(view, share, net);
    myParent->addChild(this, false);
}


void
GUISUMOViewParent::init(ViewType view, FXGLCanvas *share, GUINet &net)
{
    // Make MDI Window Menu
    setTracking();
    FXVerticalFrame *contentFrame =
        new FXVerticalFrame(this,
        FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0,0,0,0,0);
    // build the tool bar
    buildToolBar(contentFrame);
    FXHorizontalFrame *glcanvasFrame =
        new FXHorizontalFrame(contentFrame,
        FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0,0,0,0,0);
	switch(view) {
    case MICROSCOPIC_VIEW:
        if(share!=0) {
			buildEditFrame(glcanvasFrame);
            _view =
                new GUIViewTraffic(glcanvasFrame, *myParent, this, net,
                    myParent->getGLVisual(), share);
        } else {
			buildEditFrame(glcanvasFrame);
            _view =
                new GUIViewTraffic(glcanvasFrame, *myParent, this, net,
                    myParent->getGLVisual());
			//if(!(_view->isInEditMode()))
			//	groupBox->hide();
        }
        break;
    case LANE_AGGREGATED_VIEW:
        if(share!=0) {
            _view =
                new GUIViewAggregatedLanes(glcanvasFrame, *myParent, this,
                    net, myParent->getGLVisual(), share);
        } else {
            _view =
                new GUIViewAggregatedLanes(glcanvasFrame, *myParent, this,
                    net, myParent->getGLVisual());
        }
        break;
    default:
        throw 1;
    }
    _view->buildViewToolBars(*this);
}


GUISUMOViewParent::~GUISUMOViewParent()
{
    myParent->removeChild(this);
    delete myToolBar;
//    delete myToolBarDrag;
}

void
GUISUMOViewParent::buildEditFrame(FXComposite *c)
{
	groupBox=new FXGroupBox(c,"Title Left",
		GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_Y);
	new FXLabel(groupBox,"Junction(s)");
	FXMatrix *myMatrix1=new FXMatrix(groupBox,2,FRAME_THICK|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);	
	new FXLabel(myMatrix1,"Name:");
	JunctionNameTextField =new FXTextField(myMatrix1,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix1,"X-Position:");
	xPosTextField =new FXTextField(myMatrix1,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix1,"Y-Position");
	yPosTextField =new FXTextField(myMatrix1,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix1,"Mode");
	FXHorizontalFrame *buttonFrame =
		new FXHorizontalFrame(myMatrix1,
		LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
	new FXToggleButton(buttonFrame,"\t\tClick to change junction mode (currently in priority mode).","\t\tClick to change junction mode (currently in trafficlights mode).",GUIIconSubSys::getIcon(ICON_EXTRACT)/*icon1*/,GUIIconSubSys::getIcon(ICON_DILATE)/*icon2*/,NULL,0,ICON_BEFORE_TEXT|JUSTIFY_LEFT|FRAME_RAISED|FRAME_THICK);
	new FXButton(buttonFrame,"\t\ttest\status bar?",GUIIconSubSys::getIcon(ICON_EXTRACT),NULL,0,ICON_BEFORE_TEXT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK);
	new FXButton(myMatrix1,"Add Junction\t\Add Junction.",NULL,NULL,0,ICON_BEFORE_TEXT|LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_RAISED|FRAME_THICK);
	new FXButton(myMatrix1,"Delete Junction\t\Delete Junction.",NULL,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_RAISED|FRAME_THICK);

	new FXLabel(groupBox,"Lane(s)");
	FXMatrix *myMatrix2=new FXMatrix(groupBox,2,FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);	
	new FXLabel(myMatrix2,"Name:");
	LaneNameTextField =new FXTextField(myMatrix2,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix2,"Lane Start:");
	LaneStartTextField =new FXTextField(myMatrix2,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix2,"Lane End");
	LaneEndTextField =new FXTextField(myMatrix2,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix2,"Max. Speed");
	MaxSpeedTextField =new FXTextField(myMatrix2,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXLabel(myMatrix2,"Length");
	LengthTextField =new FXTextField(myMatrix2,12,this,
		NULL,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
	new FXButton(myMatrix2,"Add Lane\ttest\Add Lane.",NULL,NULL,0,ICON_BEFORE_TEXT|LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_RAISED|FRAME_THICK);
	new FXButton(myMatrix2,"Delete Lane\ttest\Delete Lane.",NULL,NULL,0,ICON_BEFORE_TEXT|LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_RAISED|FRAME_THICK);
}


FXGroupBox*
GUISUMOViewParent::getEditGroupBox()
{
	return groupBox;
}


void
GUISUMOViewParent::buildToolBar(FXComposite *c)
{
    myToolBar = new FXToolBar(c,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);

    // build the view settings
        // recenter view
    new FXButton(myToolBar,
        "\tRecenter View\tRecenter view to the simulated Area.",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_RECENTERVIEW,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        // show legend
    new MFXCheckableButton(_showLegend,
        myToolBar,"\tShow Legend\tToggle whether the Legend shall be shown.",
        GUIIconSubSys::getIcon(ICON_SHOWLEGEND), this, MID_SHOWLEGEND,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        // make snapshot
    new FXButton(myToolBar,
        "\tMake Snapshot\tMakes a snapshot of the view.",
        GUIIconSubSys::getIcon(ICON_CAMERA), this, MID_MAKESNAPSHOT,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        // allow rotation
/*    new MFXCheckableButton(_allowRotation,
        myToolBar,"\tAllow Rotation\tToggle whether Scene rotation is allowed.",
        GUIIconSubSys::getIcon(ICON_ALLOWROTATION), this, MID_ALLOWROTATION,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
*/
}


void
GUISUMOViewParent::create()
{
    FXMDIChild::create();
    _view->create();
}


long
GUISUMOViewParent::onCmdRecenterView(FXObject*,FXSelector,void*)
{
    _zoomingFactor = 100;
    _view->recenterView();
    _view->update();
    return 1;
}


long
GUISUMOViewParent::onCmdShowLegend(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _showLegend = button->amChecked();
    _view->update();
    return 1;
}


long
GUISUMOViewParent::onCmdMakeSnapshot(FXObject*sender,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this, "Save Snapshot");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.bmp");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(!opendialog.execute()){
        return 1;
    }
    gCurrentFolder = opendialog.getDirectory().text();
    string file = string(opendialog.getFilename().text());
    FXColor *buf = _view->getSnapshot();
    // Save the image.
    FXFileStream stream;
    stream.open(file.c_str(), FXStreamSave);
    fxsaveBMP(stream, buf, _view->getWidth(), _view->getHeight());
    FXFREE(&buf);
    return 1;
}


long
GUISUMOViewParent::onCmdAllowRotation(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _allowRotation = button->amChecked();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateJunction(FXObject *sender,FXSelector,void*)
{
    _view->getLocatorPopup(*this)->popdown();
    _view->getLocatorPopup(*this)->killFocus();
    _view->getLocatorPopup(*this)->lower();
    _view->getLocatorPopup(*this)->update();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_JUNCTION, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateEdge(FXObject *sender,FXSelector,void*)
{
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_EDGE, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateVehicle(FXObject *sender,FXSelector,void*)
{
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_VEHICLE, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateAdd(FXObject *sender,FXSelector,void*)
{
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_ADDITIONAL, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


double
GUISUMOViewParent::getZoomingFactor() const
{
    return _zoomingFactor;
}


void
GUISUMOViewParent::setZoomingFactor(double val)
{
    _zoomingFactor = val;
}


void
GUISUMOViewParent::setView(GUIGlObject *o)
{
    _view->centerTo(o);
}


bool
GUISUMOViewParent::showLegend() const
{
    return _showLegend;
}


bool
GUISUMOViewParent::allowRotation() const
{
    return _allowRotation;
}


int
GUISUMOViewParent::getMaxGLWidth() const
{
    return myParent->getMaxGLWidth();
}

int
GUISUMOViewParent::getMaxGLHeight() const
{
    return myParent->getMaxGLHeight();
}


long
GUISUMOViewParent::onSimStep(FXObject*sender,FXSelector,void*)
{
    _view->update();
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


