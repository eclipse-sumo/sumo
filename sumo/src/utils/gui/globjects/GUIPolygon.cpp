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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include "GUIPolygon.h"
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIPolygon::GUIPolygon(const std::string& id, const std::string& type,
                       const RGBColor& color, const PositionVector& shape, bool fill,
                       SUMOReal layer, SUMOReal angle, const std::string& imgFile):
    Polygon(id, type, color, shape, fill, layer, angle, imgFile),
    GUIGlObject_AbstractAdd("poly", GLO_POLYGON, id),
    myDisplayList(0),
    myLineWidth(-1)

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
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIPolygon::getParameterWindow(GUIMainWindow&,
                               GUISUMOAbstractView&) {
    return 0;
}


Boundary
GUIPolygon::getCenteringBoundary() const {
    Boundary b;
    b.add(myShape.getBoxBoundary());
    b.grow(10);
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
    // XXX shape should be rotated around its center when initializing the polygon. do we even need this?
    //glRotated(getAngle(), 0, 0, 1);
    GLHelper::setColor(getColor());

    int textureID = -1;
    if (getFill()) {
        const std::string& file = getImgFile();
        if (file != "") {
            textureID = GUITexturesHelper::getTextureID(file);
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
    performTesselation(s.polySize.getExaggeration(s));
    // de-init generation of texture coordinates
    if (textureID >= 0) {
        glEnable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    }
    glPopMatrix();
    drawName(myShape.getPolygonCenter(), s.scale, s.polyName);
    glPopName();
}


void
GUIPolygon::setShape(const PositionVector& shape) {
    AbstractMutex::ScopedLocker locker(myLock);
    Polygon::setShape(shape);
    //storeTesselation(myLineWidth);
}


void
GUIPolygon::performTesselation(SUMOReal lineWidth) const {
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
        for (size_t i = 0; i != myShape.size(); ++i) {
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
        myLineWidth = lineWidth;
        GLHelper::drawLine(myShape);
        GLHelper::drawBoxLines(myShape, myLineWidth);
    }
    //std::cout << "OpenGL says: '" << gluErrorString(glGetError()) << "'\n";
}


void
GUIPolygon::storeTesselation(SUMOReal lineWidth) const {
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

