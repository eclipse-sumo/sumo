/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIPolygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    June 2006
/// @version $Id$
///
// The GUI-version of a polygon
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "GUIPolygon.h"
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>

//#define GUIPolygon_DEBUG_DRAW_VERTICES

// ===========================================================================
// method definitions
// ===========================================================================
GUIPolygon::GUIPolygon(const std::string& id, const std::string& type,
                       const RGBColor& color, const PositionVector& shape, bool geo,
                       bool fill, double layer, double angle, const std::string& imgFile):
    SUMOPolygon(id, type, color, shape, geo, fill, layer, angle, imgFile),
    GUIGlObject_AbstractAdd("poly", GLO_POLYGON, id),
    myDisplayList(0),
    myLineWidth(1) // m

{}


GUIPolygon::~GUIPolygon() {}



GUIGLObjectPopupMenu*
GUIPolygon::getPopUpMenu(GUIMainWindow& app,
                         GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app, false);
    FXString t(myType.c_str());
    new FXMenuCommand(ret, "(" + t + ")", 0, 0, 0);
    new FXMenuSeparator(ret);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret, false);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIPolygon::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 3 + (int)getMap().size());
    // add items
    ret->mkItem("type", false, myType);
    ret->mkItem("layer", false, toString(getLayer()));
    ret->closeBuilding(this);
    return ret;
}


Boundary
GUIPolygon::getCenteringBoundary() const {
    Boundary b;
    b.add(myShape.getBoxBoundary());
    b.grow(2);
    return b;
}


void APIENTRY beginCallback(GLenum which) {
    glBegin(which);
}

void APIENTRY errorCallback(GLenum errorCode) {
    const GLubyte* estring;

    estring = gluErrorString(errorCode);
    fprintf(stderr, "Tessellation Error: %s\n", estring);
    exit(0);
}

void APIENTRY endCallback(void) {
    glEnd();
}

void APIENTRY vertexCallback(GLvoid* vertex) {
    glVertex3dv((GLdouble*) vertex);
}

void APIENTRY combineCallback(GLdouble coords[3],
                              GLdouble* vertex_data[4],
                              GLfloat weight[4], GLdouble** dataOut) {
    UNUSED_PARAMETER(weight);
    UNUSED_PARAMETER(*vertex_data);
    GLdouble* vertex;

    vertex = (GLdouble*) malloc(7 * sizeof(GLdouble));

    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    *dataOut = vertex;
}


GLfloat INV_POLY_TEX_DIM = 1.0 / 256.0;
GLfloat xPlane[] = {INV_POLY_TEX_DIM, 0.0, 0.0, 0.0};
GLfloat yPlane[] = {0.0, INV_POLY_TEX_DIM, 0.0, 0.0};

void
GUIPolygon::drawGL(const GUIVisualizationSettings& s) const {
    if (s.polySize.getExaggeration(s) == 0) {
        return;
    }
    Boundary boundary = myShape.getBoxBoundary();
    if (s.scale * MAX2(boundary.getWidth(), boundary.getHeight()) < s.polySize.minSize) {
        return;
    }
    if (getFill()) {
        if (myShape.size() < 3) {
            return;
        }
    } else {
        if (myShape.size() < 2) {
            return;
        }
    }
    AbstractMutex::ScopedLocker locker(myLock);
    //if (myDisplayList == 0 || (!getFill() && myLineWidth != s.polySize.getExaggeration(s))) {
    //    storeTesselation(s.polySize.getExaggeration(s));
    //}
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getLayer());
    glRotated(-getNaviDegree(), 0, 0, 1);
    // set color depending of selection
    if (gSelected.isSelected(GLO_POLYGON, getGlID())) {
        GLHelper::setColor(RGBColor(0, 0, 204));
    } else {
        GLHelper::setColor(getColor());
    }

    int textureID = -1;
    if (getFill()) {
        const std::string& file = getImgFile();
        if (file != "") {
            textureID = GUITexturesHelper::getTextureID(file, true);
        }
    }
    // init generation of texture coordinates
    if (textureID >= 0) {
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST); // without DEPTH_TEST vehicles may be drawn below roads
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // http://www.gamedev.net/topic/133564-glutesselation-and-texture-mapping/
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv(GL_S, GL_OBJECT_PLANE, xPlane);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, yPlane);
    }
    // recall tesselation
    //glCallList(myDisplayList);
    performTesselation(myLineWidth * s.polySize.getExaggeration(s));
    // de-init generation of texture coordinates
    if (textureID >= 0) {
        glEnable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    }
#ifdef GUIPolygon_DEBUG_DRAW_VERTICES
    GLHelper::debugVertices(myShape, 80 / s.scale);
#endif
    glPopMatrix();
    const Position namePos = myShape.getPolygonCenter();
    drawName(namePos, s.scale, s.polyName);
    if (s.polyType.show) {
        GLHelper::drawText(myType, namePos + Position(0, -0.6 * s.polyType.size / s.scale),
                           GLO_MAX, s.polyType.size / s.scale, s.polyType.color);
    }
    glPopName();
}


void
GUIPolygon::setShape(const PositionVector& shape) {
    AbstractMutex::ScopedLocker locker(myLock);
    SUMOPolygon::setShape(shape);
    //storeTesselation(myLineWidth);
}


void
GUIPolygon::performTesselation(double lineWidth) const {
    if (getFill()) {
        // draw the tesselated shape
        double* points = new double[myShape.size() * 3];
        GLUtesselator* tobj = gluNewTess();
        gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid(APIENTRY*)()) &glVertex3dv);
        gluTessCallback(tobj, GLU_TESS_BEGIN, (GLvoid(APIENTRY*)()) &beginCallback);
        gluTessCallback(tobj, GLU_TESS_END, (GLvoid(APIENTRY*)()) &endCallback);
        //gluTessCallback(tobj, GLU_TESS_ERROR, (GLvoid (APIENTRY*) ()) &errorCallback);
        gluTessCallback(tobj, GLU_TESS_COMBINE, (GLvoid(APIENTRY*)()) &combineCallback);
        gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
        gluTessBeginPolygon(tobj, NULL);
        gluTessBeginContour(tobj);
        for (int i = 0; i != (int)myShape.size(); ++i) {
            points[3 * i]  = myShape[(int) i].x();
            points[3 * i + 1]  = myShape[(int) i].y();
            points[3 * i + 2]  = 0;
            gluTessVertex(tobj, points + 3 * i, points + 3 * i);
        }
        gluTessEndContour(tobj);

        gluTessEndPolygon(tobj);
        gluDeleteTess(tobj);
        delete[] points;

    } else {
        GLHelper::drawLine(myShape);
        GLHelper::drawBoxLines(myShape, lineWidth);
    }
    //std::cout << "OpenGL says: '" << gluErrorString(glGetError()) << "'\n";
}


void
GUIPolygon::storeTesselation(double lineWidth) const {
    if (myDisplayList > 0) {
        glDeleteLists(myDisplayList, 1);
    }
    myDisplayList = glGenLists(1);
    if (myDisplayList == 0) {
        throw ProcessError("GUIPolygon::storeTesselation() could not create display list");
    }
    glNewList(myDisplayList, GL_COMPILE);
    performTesselation(lineWidth);
    glEndList();
}


/****************************************************************************/

