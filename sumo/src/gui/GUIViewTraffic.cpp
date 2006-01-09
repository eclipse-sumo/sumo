//---------------------------------------------------------------------------//
//                        GUIViewTraffic.cpp -
//  A view on the simulation; this view is a microscopic one
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
// Revision 1.43  2006/01/09 11:50:20  dkrajzew
// new visualization settings implemented
//
// Revision 1.42  2005/11/09 06:33:15  dkrajzew
// removed unneeded stuff
//
// Revision 1.41  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.40  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.39  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.38  2005/07/12 12:10:14  dkrajzew
// further visualisation options added
//
// Revision 1.37  2005/05/04 07:50:15  dkrajzew
// ported to fox1.4
//
// Revision 1.36  2004/12/15 09:20:17  dkrajzew
// made guisim independent of giant/netedit
//
// Revision 1.35  2004/12/13 15:34:47  dkrajzew
// window resize on controls show/hide implemented
//
// Revision 1.34  2004/12/12 17:23:58  agaubatz
// Editor Tool Widgets included
//
// Revision 1.33  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.32  2004/08/02 11:55:35  dkrajzew
// using coloring schemes stored in a container
//
// Revision 1.31  2004/07/02 08:31:35  dkrajzew
// detector drawer now also draw other additional items; removed some memory
//  leaks;
//  some further drawing options (mainly for the online-router added)
//
// Revision 1.30  2004/06/17 13:06:55  dkrajzew
// Polygon visualisation added
//
// Revision 1.29  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.28  2004/02/16 13:56:27  dkrajzew
// renamed some buttons and toolitips
//
// Revision 1.27  2004/02/05 16:29:31  dkrajzew
// memory leaks removed
//
// Revision 1.26  2003/12/09 11:23:07  dkrajzew
// some memory leaks removed
//
// Revision 1.25  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.24  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two SUMOReals implemented
//
// Revision 1.23  2003/10/15 11:37:50  dkrajzew
// old row-drawer replaced by new ones; setting of name information seems to
//  be necessary
//
// Revision 1.22  2003/10/02 14:55:56  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.21  2003/09/23 14:25:13  dkrajzew
// possibility to visualise detectors using different geometry complexities
//  added
//
// Revision 1.20  2003/09/05 14:55:11  dkrajzew
// lighter drawer implementations
//
// Revision 1.19  2003/08/14 13:44:14  dkrajzew
// tls/row - drawer added
//
// Revision 1.18  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.17  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.16  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.15  2003/06/06 10:33:47  dkrajzew
// changes due to moving the popup-menus into a subfolder
//
// Revision 1.14  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.13  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.12  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged;
// additional parameter of maximum display size added
//
// Revision 1.11  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed;
// new gl-object concept implemented; comments added
//
// Revision 1.10  2003/04/07 10:15:16  dkrajzew
// glut reinserted
//
// Revision 1.9  2003/04/04 15:13:20  roessel
// Commented out #include <glut.h>
// Added #include <qcursor.h>
//
// Revision 1.8  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged;
// comments added; tootip button added
//
// Revision 1.7  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.6  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.5  2003/03/12 16:55:19  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/03/03 15:10:20  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
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

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSCORN.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include "GUISUMOViewParent.h"
#include "drawerimpl/GUIVehicleDrawer_FGwTasTriangle.h"
#include "drawerimpl/GUIVehicleDrawer_FGnTasTriangle.h"
#include "drawerimpl/GUIVehicleDrawer_SGwTasTriangle.h"
#include "drawerimpl/GUIVehicleDrawer_SGnTasTriangle.h"
#include "drawerimpl/GUIJunctionDrawer_nT.h"
#include "drawerimpl/GUIJunctionDrawer_wT.h"
#include "drawerimpl/GUIDetectorDrawer_SGnT.h"
#include "drawerimpl/GUIDetectorDrawer_SGwT.h"
#include "drawerimpl/GUIDetectorDrawer_FGnT.h"
#include "drawerimpl/GUIDetectorDrawer_FGwT.h"
#include "drawerimpl/GUIROWDrawer_SGnT.h"
#include "drawerimpl/GUIROWDrawer_FGnT.h"
#include "drawerimpl/GUIROWDrawer_SGwT.h"
#include "drawerimpl/GUIROWDrawer_FGwT.h"
#include <utils/gui/drawer/GUILaneDrawer_SGwT.h>
#include <utils/gui/drawer/GUILaneDrawer_SGnT.h>
#include <utils/gui/drawer/GUILaneDrawer_FGwT.h>
#include <utils/gui/drawer/GUILaneDrawer_FGnT.h>
#include "GUIViewTraffic.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/drawer/GUIColorer_LaneBySelection.h>
#include <utils/gui/drawer/GUIColorer_ShadeByFunctionValue.h>
#include "GUIColorer_LaneByPurpose.h"
#include <utils/gui/div/GUIExcp_VehicleIsInvisible.h>
#include <utils/glutils/polyfonts.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/foxtools/MFXImageHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIViewTraffic) GUIViewTrafficMap[]={
    FXMAPFUNC(SEL_COMMAND,  MID_COLOURSCHEMECHANGE,   GUIViewTraffic::onCmdChangeColorScheme),

    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS,   GUIViewTraffic::onCmdShowToolTips),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWGRID,       GUIViewTraffic::onCmdShowGrid),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWFULLGEOM,   GUIViewTraffic::onCmdShowFullGeom),
};

FXIMPLEMENT(GUIViewTraffic,GUISUMOAbstractView,GUIViewTrafficMap,ARRAYNUMBER(GUIViewTrafficMap))


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIViewTraffic::GUIViewTraffic(FXComposite *p,
                               GUIMainWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis)
	: GUISUMOAbstractView(p, app, parent, net._grid, glVis),
    myTrackedID(-1), myUseFullGeom(true),
    _edges2Show(0), _junctions2Show(0), _additional2Show(0),
	_net(&net)
{
    init(net);
}


GUIViewTraffic::GUIViewTraffic(FXComposite *p,
                               GUIMainWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis,
                               FXGLCanvas *share)
    : GUISUMOAbstractView(p, app, parent, net._grid, glVis, share),
    myTrackedID(-1), myUseFullGeom(true),
    _edges2Show(0), _junctions2Show(0), _additional2Show(0),
	_net(&net)
{
    init(net);
}


void
GUIViewTraffic::init(GUINet &net)
{
    // build the artifact-instances-to-draw - tables
    _edges2ShowSize = (MSEdge::dictSize()>>5) + 1;
    _edges2Show = new size_t[_edges2ShowSize];
    clearUsetable(_edges2Show, _edges2ShowSize);
    _junctions2ShowSize = (MSJunction::dictSize()>>5) + 1;
    _junctions2Show = new size_t[_junctions2ShowSize];
    clearUsetable(_junctions2Show, _junctions2ShowSize);
    _additional2ShowSize = (GUIGlObject_AbstractAdd::getObjectList().size()>>5) + 1;
    _additional2Show = new size_t[_additional2ShowSize];
    clearUsetable(_additional2Show, _additional2ShowSize);
    // build the drawers
    myVehicleDrawer[0] =
        new GUIVehicleDrawer_SGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[1] =
        new GUIVehicleDrawer_SGwTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[2] =
        new GUIVehicleDrawer_FGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[3] =
        new GUIVehicleDrawer_FGwTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[4] =
        new GUIVehicleDrawer_SGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[5] =
        new GUIVehicleDrawer_SGwTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[6] =
        new GUIVehicleDrawer_FGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[7] =
        new GUIVehicleDrawer_FGwTasTriangle(_net->myEdgeWrapper);
    myLaneDrawer[0] = new GUILaneDrawer_SGnT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[1] = new GUILaneDrawer_SGwT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[2] = new GUILaneDrawer_FGnT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[3] = new GUILaneDrawer_FGwT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[4] = new GUILaneDrawer_SGnT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[5] = new GUILaneDrawer_SGwT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[6] = new GUILaneDrawer_FGnT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myLaneDrawer[7] = new GUILaneDrawer_FGwT<GUIEdge, GUIEdge, GUILaneWrapper>(_net->myEdgeWrapper);
    myJunctionDrawer[0] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[1] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myJunctionDrawer[2] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[3] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myJunctionDrawer[4] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[5] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myJunctionDrawer[6] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[7] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myDetectorDrawer[0] = new GUIDetectorDrawer_SGnT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[1] = new GUIDetectorDrawer_SGwT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[2] = new GUIDetectorDrawer_FGnT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[3] = new GUIDetectorDrawer_FGwT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[4] = new GUIDetectorDrawer_SGnT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[5] = new GUIDetectorDrawer_SGwT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[6] = new GUIDetectorDrawer_FGnT(GUIGlObject_AbstractAdd::getObjectList());
    myDetectorDrawer[7] = new GUIDetectorDrawer_FGwT(GUIGlObject_AbstractAdd::getObjectList());
    myROWDrawer[0] = new GUIROWDrawer_SGnT(_net->myEdgeWrapper);
    myROWDrawer[1] = new GUIROWDrawer_SGwT(_net->myEdgeWrapper);
    myROWDrawer[2] = new GUIROWDrawer_FGnT(_net->myEdgeWrapper);
    myROWDrawer[3] = new GUIROWDrawer_FGwT(_net->myEdgeWrapper);
    myROWDrawer[4] = new GUIROWDrawer_SGnT(_net->myEdgeWrapper);
    myROWDrawer[5] = new GUIROWDrawer_SGwT(_net->myEdgeWrapper);
    myROWDrawer[6] = new GUIROWDrawer_FGnT(_net->myEdgeWrapper);
    myROWDrawer[7] = new GUIROWDrawer_FGwT(_net->myEdgeWrapper);

    // lane coloring
	myLaneColoringSchemes.add("uniform",
        new GUIColorer_SingleColor<GUILaneWrapper>(RGBColor(0, 0, 0)));

	myLaneColoringSchemes.add("by allowed speed (lanewise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, (SUMOReal) (150.0/3.6),
            RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::maxSpeed));

	myLaneColoringSchemes.add("by current density (lanewise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, (SUMOReal) .8,
            RGBColor(0, 1, 0), RGBColor(1, 0, 0),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::myMagic)); // !!!

	myLaneColoringSchemes.add("by first vehicle waiting time (lanewise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, 200,
            RGBColor(0, 1, 0), RGBColor(1, 0, 0),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::firstWaitingTime));

	myLaneColoringSchemes.add("by selection (lanewise)",
		new GUIColorer_LaneBySelection<GUILaneWrapper>());

	myLaneColoringSchemes.add("by purpose (lanewise)",
		new GUIColorer_LaneByPurpose<GUILaneWrapper>());

    myVisualizationSettings = gSchemeStorage.get(gSchemeStorage.getNames()[0]);
}


GUIViewTraffic::~GUIViewTraffic()
{
    for(size_t i=0; i<8; i++) {
        delete myVehicleDrawer[i];
        delete myLaneDrawer[i];
        delete myJunctionDrawer[i];
        delete myDetectorDrawer[i];
        delete myROWDrawer[i];
    }
    delete[] _edges2Show;
    delete[] _junctions2Show;
    delete[] _additional2Show;
    delete myLocatorPopup;
}


void
GUIViewTraffic::create()
{
    FXGLCanvas::create();
    myLocatorPopup->create();
}


void
GUIViewTraffic::buildViewToolBars(GUIGlChildWindow &v)
{
    FXToolBar &toolbar = v.getToolBar(*this);
    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SINGLE);
    // build coloring tools
    {
        FXComboBox *myColoringSchemes =
            new FXComboBox(&toolbar, 8, this, MID_COLOURSCHEMECHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for(std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            myColoringSchemes->appendItem((*i).c_str());
        }
    }



    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SINGLE);

    // build the locator buttons
    myLocatorPopup = new FXPopup(&toolbar, POPUP_VERTICAL);
        // for junctions
        new FXButton(myLocatorPopup,
            "\tLocate Junction\tLocate a Junction within the Network.",
            GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), &v, MID_LOCATEJUNCTION,
            ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
            // for edges
        new FXButton(myLocatorPopup,
            "\tLocate Street\tLocate a Street within the Network.",
            GUIIconSubSys::getIcon(ICON_LOCATEEDGE), &v, MID_LOCATEEDGE,
            ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
            // for vehicles
        new FXButton(myLocatorPopup,
            "\tLocate Vehicle\tLocate a Vehicle within the Network.",
            GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), &v, MID_LOCATEVEHICLE,
            ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
            // for additional stuff
        new FXButton(myLocatorPopup,
            "\tLocate Additional\tLocate an additional Structure within the Network.",
            GUIIconSubSys::getIcon(ICON_LOCATEADD), &v, MID_LOCATEADD,
            ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    new FXMenuButton(&toolbar,"\tLocate structures",
        GUIIconSubSys::getIcon(ICON_LOCATE), myLocatorPopup,
        MENUBUTTON_RIGHT|LAYOUT_TOP|BUTTON_TOOLBAR|FRAME_RAISED|FRAME_THICK);
    // add viewport button
    new FXButton(&toolbar,
        "\tEdit Viewport...\tOpens a menu which lets you edit the viewport.",
        GUIIconSubSys::getIcon(ICON_EDITVIEWPORT), this, MID_EDITVIEWPORT,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(&toolbar,
        "\tEdit Viewport...\tOpens a menu which lets you edit the viewport.",
        GUIIconSubSys::getIcon(ICON_EDITVIEWPORT), this, MID_EDITVIEW,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    /*
    new FXButton(&toolbar,
        "\tEdit Viewport...\tOpens a menu which lets you edit the viewport.",
        GUIIconSubSys::getIcon(ICON_EDITVIEWPORT), this, MID_EDITVIEW,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
*/
    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SINGLE);

    // add toggle button for grid on/off
    new MFXCheckableButton(false,
        &toolbar,
        "\tToggles Net Grid\tToggle whether the Grid shall be visualised.",
        GUIIconSubSys::getIcon(ICON_SHOWGRID), this, MID_SHOWGRID,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // add toggle button for tool-tips on/off
    new MFXCheckableButton(false,
        &toolbar,
        "\tToggles Tool Tips\tToggle whether Tool Tips shall be shown.",
        GUIIconSubSys::getIcon(ICON_SHOWTOOLTIPS), this, MID_SHOWTOOLTIPS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // add toggle button for full geometry-tips on/off
    new MFXCheckableButton(true,
        &toolbar,
        "\tToggles Geometry\tToggle whether full or simple Geometry shall be used.",
        GUIIconSubSys::getIcon(ICON_SHOWFULLGEOM), this, MID_SHOWFULLGEOM,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
}


long
GUIViewTraffic::onCmdChangeColorScheme(FXObject*,FXSelector sel,void*data)
{
    char *dataC = (char*) data; // !!! unicode
    myVisualizationSettings = gSchemeStorage.get(dataC);
    // lanes
    switch(myLaneColoringSchemes.getColorSetType(myVisualizationSettings.laneEdgeMode)) {
    case CST_SINGLE:
        myLaneColoringSchemes.getColorerInterface(myVisualizationSettings.laneEdgeMode)->resetColor(myVisualizationSettings.singleLaneColor);
        break;
    case CST_MINMAX:
        myLaneColoringSchemes.getColorerInterface(myVisualizationSettings.laneEdgeMode)->resetColor(myVisualizationSettings.minLaneColor, myVisualizationSettings.maxLaneColor);
        break;
    default:
        break;
    }
    // vehicles
    switch(GUIBaseVehicleDrawer::getSchemesMap().getColorSetType(myVisualizationSettings.vehicleMode)) {
    case CST_SINGLE:
        GUIBaseVehicleDrawer::getSchemesMap().getColorerInterface(myVisualizationSettings.vehicleMode)->resetColor(myVisualizationSettings.singleVehicleColor);
        break;
    case CST_MINMAX:
        GUIBaseVehicleDrawer::getSchemesMap().getColorerInterface(myVisualizationSettings.vehicleMode)->resetColor(myVisualizationSettings.minVehicleColor, myVisualizationSettings.maxVehicleColor);
        break;
    default:
        break;
    }
    update();
    return 1;
}


long
GUIViewTraffic::onCmdShowFullGeom(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    myUseFullGeom = button->amChecked();
    update();
    return 1;
}


void
GUIViewTraffic::doPaintGL(int mode, SUMOReal scale)
{
    // init view settings
    glRenderMode(mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // get the viewport settings
    const Boundary &nb = _net->getBoundary();
    SUMOReal x = (nb.getCenter().x() - _changer->getXPos()); // center of view
    SUMOReal xoff = (SUMOReal) 50.0 / _changer->getZoom() * myNetScale
        / _addScl; // offset to right
    SUMOReal y = (nb.getCenter().y() - _changer->getYPos()); // center of view
    SUMOReal yoff = (SUMOReal) 50.0 / _changer->getZoom() * myNetScale
        / _addScl; // offset to top
    // reset the tables of things to show if the viewport has changed
    if(myViewportSettings.differ(x, y, xoff, yoff)) {
        clearUsetable(_edges2Show, _edges2ShowSize);
        clearUsetable(_junctions2Show, _junctions2ShowSize);
        _net->_grid.get(GLO_LANE|GLO_JUNCTION|GLO_DETECTOR, x, y, xoff, yoff,
            _edges2Show, _junctions2Show, _additional2Show);
        myViewportSettings.set(x, y, xoff, yoff);
    }
    // compute lane width
    SUMOReal width = m2p(3.0) * scale;
    size_t drawerToUse = 0;
    // compute which drawer shall be used
    if(myUseFullGeom) {
        drawerToUse = 2;
    }
    if(_useToolTips) {
        drawerToUse += 1;
    }
    // draw
    {
        myDecalsLock.lock();
        for(std::vector<GUISUMOAbstractView::Decal>::iterator l=myDecals.begin(); l!=myDecals.end(); ++l) {
            GUISUMOAbstractView::Decal &d = *l;
            if(!d.initialised) {
                FXImage *i = MFXImageHelper::loadimage(getApp(), d.filename);
                if(i!=0) {
                    d.glID = GUITexturesHelper::add(i);
                    d.initialised = true;
                }
            }
            glPushMatrix();
            glTranslated(d.left, d.top, 0);
            glRotated(d.rot, 0, 0, 1);
            glColor3d(1,1,1);
            GUITexturesHelper::drawTexturedBox(d.glID, 0, 0, d.right-d.left, d.bottom-d.top);
            glPopMatrix();
        }
        myDecalsLock.unlock();
    }

    myJunctionDrawer[drawerToUse]->drawGLJunctions(_junctions2Show, _junctions2ShowSize,
        _junctionColScheme);
    myLaneDrawer[drawerToUse]->drawGLLanes(_edges2Show, _edges2ShowSize, width,
        *myLaneColoringSchemes.getColorer(myVisualizationSettings.laneEdgeMode),
        myVisualizationSettings.laneShowBorders);
    myDetectorDrawer[drawerToUse]->drawGLDetectors(_additional2Show, _additional2ShowSize,
        width, myVisualizationSettings.addExaggeration);
    myROWDrawer[drawerToUse]->drawGLROWs(*_net, _edges2Show, _edges2ShowSize, width,
        myVisualizationSettings.showLane2Lane, myVisualizationSettings.showLinkDecals);
    //
    for(std::vector<VehicleOps>::iterator i=myVehicleOps.begin(); i!=myVehicleOps.end(); ++i) {
        const VehicleOps &vo = *i;
        switch(vo.type) {
        case VO_SHOW_ROUTE:
            {
                if(vo.routeNo>=0) {
                    drawRoute(vo, vo.routeNo, 0.25);
                } else {
                    int noReroutePlus1 =
                        (int) vo.vehicle->getCORNDoubleValue(MSCORN::CORN_VEH_NUMBERROUTE) + 1;
                    for(int i=noReroutePlus1-1; i>=0; i--) {
                        SUMOReal darken =
                            (SUMOReal) 0.4
                            / (SUMOReal) noReroutePlus1
                            * (SUMOReal) i;
                        drawRoute(vo, i, darken);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    // draw the Polygons
    drawShapes(_net->getShapeContainer());
    // draw vehicles only when they're visible
    if(scale*m2p(3)>myVisualizationSettings.minVehicleSize) {
        myVehicleDrawer[drawerToUse]->drawGLVehicles(_edges2Show, _edges2ShowSize,
            *GUIBaseVehicleDrawer::getSchemesMap().getColorer(myVisualizationSettings.vehicleMode),
            myVisualizationSettings.vehicleExaggeration);
    }

/*
    if(!TexturesInitialised) {
        TexturesInitialised = true;
        FXImage *i = MFXImageHelper::loadimage(getApp(), "D:\\koeln_fastlane.gif");
        TextureNo = GUITexturesHelper::add(i);
/*
        FXIcon *i = GUIIconSubSys::getIcon(ICON_APP_BREAKPOINTS);
        glEnable(GL_TEXTURE_2D);
        glGenTextures( 1, &TextureNo );
        glBindTexture(GL_TEXTURE_2D, TextureNo);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, i->getWidth(), i->getHeight(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, i->getData());
            */
//    }

/*
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureNo);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 1.0f);

    glTexCoord2f(0.0f, 0.0f); glVertex2d(0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex2d(150, 0);
    glTexCoord2f(1.0f, 1.0f); glVertex2d(150, 150);
    glTexCoord2f(0.0f, 1.0f); glVertex2d(0, 150);

    glTexCoord2f(0.0f, 0.0f); glVertex2d(200+0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex2d(200+0, 150);
    glTexCoord2f(1.0f, 1.0f); glVertex2d(200+150, 150);
    glTexCoord2f(0.0f, 1.0f); glVertex2d(200+150, 0);

    glDisable(GL_TEXTURE_2D);

    glEnd();
*/
    /*
    glPushMatrix();
    glTranslated(100, 100, 0);
    GUITexturesHelper::drawTexturedBox(TextureNo, 20);
    glPopMatrix();
    */
    // !!!!

    glPopMatrix();
}

RGBColor
GUIViewTraffic::getEdgeColor(GUIEdge *edge) const
{
    switch(edge->getPurpose()) {
    case GUIEdge::EDGEFUNCTION_NORMAL:
        return RGBColor((SUMOReal) 0,   (SUMOReal) 0,   (SUMOReal) 0);
    case GUIEdge::EDGEFUNCTION_SOURCE:
        return RGBColor((SUMOReal) 0,   (SUMOReal) 0.2, (SUMOReal) 0);
    case GUIEdge::EDGEFUNCTION_SINK:
        return RGBColor((SUMOReal) 0.2, (SUMOReal) 0,   (SUMOReal) 0);
    case GUIEdge::EDGEFUNCTION_INTERNAL:
        return RGBColor((SUMOReal) 0,   (SUMOReal) 0,   (SUMOReal) 0.2);
    default:
        break;
    }
    throw 1;
}


void
GUIViewTraffic::track(int id)
{
    myTrackedID = id;
}


void
GUIViewTraffic::doInit()
{
}


void
GUIViewTraffic::drawRoute(const VehicleOps &vo, int routeNo, SUMOReal darken)
{
    /*!!!
    if(_useToolTips) {
        glPushName(vo.vehicle->getGlID());
    }
    RGBColor c =
        myVehicleDrawer[0]->getVehicleColor(*(vo.vehicle), _vehicleColScheme);
    c.darken(darken);
    glColor3d(c.red(), c.green(), c.blue());
    draw(vo.vehicle->getRoute(routeNo));
    if(_useToolTips) {
        glPopName();
    }
    */
}


void
GUIViewTraffic::centerTo(GUIGlObject *o)
{
	if(o->getType()!=GLO_VEHICLE) {
        GUISUMOAbstractView::centerTo(o);
    } else {
        try {
            Position2D pos = _net->getVehiclePosition(o->microsimID());
            Boundary b;
            b.add(pos);
            b.grow(20);
            GUISUMOAbstractView::centerTo(b);
            _changer->otherChange();
            update();
        } catch (GUIExcp_VehicleIsInvisible) {
        }
    }
}


void
GUIViewTraffic::draw(const MSRoute &r)
{
    MSRouteIterator i = r.begin();
    for(; i!=r.end(); ++i) {
        const MSEdge *e = *i;
        const GUIEdge *ge = static_cast<const GUIEdge*>(e);
        const GUILaneWrapper &lane = ge->getLaneGeometry((size_t) 0);
        const DoubleVector &rots = lane.getShapeRotations();
        const DoubleVector &lengths = lane.getShapeLengths();
        const Position2DVector &geom = lane.getShape();
        for(size_t i=0; i<geom.size()-1; i++) {
			GLHelper::drawBoxLine(geom.at(i), rots[i], lengths[i], 1.0);
        }
    }
}


void
GUIViewTraffic::showRoute(GUIVehicle *v, int index)
{
    VehicleOps vo;
    vo.vehicle = v;
    vo.type = VO_SHOW_ROUTE;
    vo.routeNo = index;
    myVehicleOps.push_back(vo);
    update();
}


void
GUIViewTraffic::hideRoute(GUIVehicle *v, int index)
{
    std::vector<VehicleOps>::iterator i =
        find_if(myVehicleOps.begin(), myVehicleOps.end(), vehicle_in_ops_finder(v));
    while(i!=myVehicleOps.end()) {
        if((*i).type==VO_SHOW_ROUTE&&(*i).routeNo==index) {
            i = myVehicleOps.erase(i);
		    update();
            return;
        }
        i = find_if(i+1, myVehicleOps.end(), vehicle_in_ops_finder(v));
    }
    update();
}


bool
GUIViewTraffic::amShowingRouteFor(GUIVehicle *v, int index)
{
    std::vector<VehicleOps>::iterator i =
        find_if(myVehicleOps.begin(), myVehicleOps.end(), vehicle_in_ops_finder(v));
    while(i!=myVehicleOps.end()) {
        if((*i).type==VO_SHOW_ROUTE&&(*i).routeNo==index) {
            return true;
        }
        i = find_if(i+1, myVehicleOps.end(), vehicle_in_ops_finder(v));
    }
    return false;
}


long
GUIViewTraffic::onCmdEditView(FXObject*,FXSelector,void*)
{
    if(myVisualizationChanger==0) {
        myVisualizationChanger =
            new GUIDialog_ViewSettings(
                myApp, this, &myVisualizationSettings,
                &myLaneColoringSchemes, &GUIBaseVehicleDrawer::getSchemesMap(),
                &myDecals, &myDecalsLock);
        myVisualizationChanger->create();
    }
    myVisualizationChanger->show();
    return 1;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


