/****************************************************************************/
/// @file    GUIViewTraffic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A view on the simulation; this view is a microscopic one
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utility>
#include <cmath>
#include <limits>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include "GUISUMOViewParent.h"
#include "GUIViewTraffic.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <foreign/rtree/SUMORTree.h>
#include <utils/gui/div/GLHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIViewTraffic::GUIViewTraffic(FXComposite *p,
                               GUIMainWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis,
                               FXGLCanvas *share)
        : GUISUMOAbstractView(p, app, parent, net.getVisualisationSpeedUp(), glVis, share),
        myTrackedID(-1) {
}


GUIViewTraffic::~GUIViewTraffic() {
}


void
GUIViewTraffic::buildViewToolBars(GUIGlChildWindow &v) {
    // build coloring tools
    {
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            v.getColoringSchemesCombo().appendItem((*i).c_str());
            if ((*i) == myVisualizationSettings->name) {
                v.getColoringSchemesCombo().setCurrentItem(v.getColoringSchemesCombo().getNumItems()-1);
            }
        }
        v.getColoringSchemesCombo().setNumVisible(5);
    }
    // for junctions
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Junction\tLocate a junction within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), &v, MID_LOCATEJUNCTION,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for edges
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Street\tLocate a street within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEEDGE), &v, MID_LOCATEEDGE,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for vehicles
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Vehicle\tLocate a vehicle within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), &v, MID_LOCATEVEHICLE,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for tls
    new FXButton(v.getLocatorPopup(),
                 "\tLocate TLS\tLocate a tls within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATETLS), &v, MID_LOCATETLS,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for additional stuff
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Additional\tLocate an additional structure within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEADD), &v, MID_LOCATEADD,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for shapes
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Shape\tLocate a shape within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATESHAPE), &v, MID_LOCATESHAPE,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
}


bool
GUIViewTraffic::setColorScheme(const std::string &name) {
	if(!gSchemeStorage.contains(name)) {
		return false;
	}
    if (myVisualizationChanger!=0) {
        if (myVisualizationChanger->getCurrentScheme()!=name) {
            myVisualizationChanger->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(name.c_str());
    update();
	return true;
}


int
GUIViewTraffic::doPaintGL(int mode, SUMOReal scale) {
    // init view settings
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    GLdouble sxmin = myCX - myX1;
    GLdouble sxmax = myCX + myX1;
    GLdouble symin = myCY - myY1;
    GLdouble symax = myCY + myY1;

    // compute lane width
    SUMOReal lw = m2p(3.0) * scale;
    // draw decals (if not in grabbing mode)
    if (!myUseToolTips) {
        drawDecals();
        if (myVisualizationSettings->showGrid) {
            paintGLGrid();
        }
    }

    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float minB[2];
    float maxB[2];
    minB[0] = sxmin;
    minB[1] = symin;
    maxB[0] = sxmax;
    maxB[1] = symax;
    myVisualizationSettings->needsGlID = myUseToolTips;
    myVisualizationSettings->scale = lw;
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    int hits2 = myGrid->Search(minB, maxB, *myVisualizationSettings);
    //
    glTranslated(0, 0, -.01);
    for (std::map<GUIGlObject*, int>::iterator i=myAdditionallyDrawn.begin(); i!=myAdditionallyDrawn.end(); ++i) {
        (i->first)->drawGLAdditional(this, *myVisualizationSettings);
    }
    glTranslated(0, 0, .01);
    glPopMatrix();
    /*
    // draw legends
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(1.-.2, 1.-.5, 0.);
    glScaled(.2, .5, 1.);
    GUIColoringSchemesMap<GUILaneWrapper> &sm = GUIViewTraffic::getLaneSchemesMap(); //!!!
    sm.getColorer(myVisualizationSettings->laneEdgeMode)->drawLegend();
    */
    return hits2;
}


void
GUIViewTraffic::startTrack(int id) {
    myTrackedID = id;
}


void
GUIViewTraffic::stopTrack() {
    myTrackedID = -1;
}


int
GUIViewTraffic::getTrackedID() const {
    return myTrackedID;
}


void
GUIViewTraffic::showViewschemeEditor() {
    if (myVisualizationChanger==0) {
        myVisualizationChanger =
            new GUIDialog_ViewSettings(
            this, myVisualizationSettings,
            &myDecals, &myDecalsLock);
        myVisualizationChanger->create();
    } else {
        myVisualizationChanger->setCurrent(myVisualizationSettings);
    }
    myVisualizationChanger->show();
}


void
GUIViewTraffic::onGamingClick(Position2D pos) {
    MSTLLogicControl &tlsControl = MSNet::getInstance()->getTLSControl();
    const std::vector<MSTrafficLightLogic*> &logics = tlsControl.getAllLogics();
    MSTrafficLightLogic *minTll = 0;
    SUMOReal minDist = std::numeric_limits<SUMOReal>::infinity();
    for (std::vector<MSTrafficLightLogic*>::const_iterator i=logics.begin(); i!=logics.end(); ++i) {
        // get the logic
        MSTrafficLightLogic *tll = (*i);
        if (tlsControl.isActive(tll)) {
            // get the links
            const MSTrafficLightLogic::LaneVector &lanes = tll->getLanesAt(0);
            if (lanes.size()>0) {
                const Position2D &endPos = lanes[0]->getShape().getEnd();
                if (endPos.distanceTo(pos) < minDist) {
                    minDist = endPos.distanceTo(pos);
                    minTll = tll;
                }
            }
        }
    }
    if (minTll != 0) {
        const MSTLLogicControl::TLSLogicVariants &vars = tlsControl.get(minTll->getID());
        const std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
        if (logics.size() > 1) {
            MSSimpleTrafficLightLogic *l = (MSSimpleTrafficLightLogic*) logics[0];
            for (unsigned int i = 0; i < logics.size()-1; i++) {
                if (minTll->getProgramID() == logics[i]->getProgramID()) {
                    l = (MSSimpleTrafficLightLogic*) logics[i+1];
                    tlsControl.switchTo(minTll->getID(), l->getProgramID());
                }
            }
            if (l == logics[0]) {
                tlsControl.switchTo(minTll->getID(), l->getProgramID());
            }
            l->changeStepAndDuration(tlsControl, MSNet::getInstance()->getCurrentTimeStep(), 0, l->getPhase(0).duration);
            update();
        }
    }
}


/****************************************************************************/
