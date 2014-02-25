/****************************************************************************/
/// @file    GLHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Some methods which help to draw certain geometrical objects in openGL
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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

#include "GLHelper.h"
#include <utils/geom/GeomHelper.h>
#include <utils/common/StdDefs.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::pair<SUMOReal, SUMOReal> > GLHelper::myCircleCoords;


void APIENTRY combCallback(GLdouble coords[3],
                           GLdouble* vertex_data[4],
                           GLfloat weight[4], GLdouble** dataOut) {
    UNUSED_PARAMETER(weight);
    UNUSED_PARAMETER(*vertex_data);
    GLdouble* vertex;

    vertex = (GLdouble*)malloc(7 * sizeof(GLdouble));

    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    *dataOut = vertex;
}

// ===========================================================================
// method definitions
// ===========================================================================


void
GLHelper::drawFilledPoly(const PositionVector& v, bool close) {
    if (v.size() == 0) {
        return;
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    for (PositionVector::const_iterator i = v.begin(); i != v.end(); i++) {
        const Position& p = *i;
        glVertex2d(p.x(), p.y());
    }
    if (close) {
        const Position& p = *(v.begin());
        glVertex2d(p.x(), p.y());
    }
    glEnd();
}


void
GLHelper::drawFilledPolyTesselated(const PositionVector& v, bool close) {
    if (v.size() == 0) {
        return;
    }
    GLUtesselator* tobj = gluNewTess();
    gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid(APIENTRY*)()) &glVertex3dv);
    gluTessCallback(tobj, GLU_TESS_BEGIN, (GLvoid(APIENTRY*)()) &glBegin);
    gluTessCallback(tobj, GLU_TESS_END, (GLvoid(APIENTRY*)()) &glEnd);
    gluTessCallback(tobj, GLU_TESS_COMBINE, (GLvoid(APIENTRY*)()) &combCallback);
    gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
    gluTessBeginPolygon(tobj, NULL);
    gluTessBeginContour(tobj);
    double* points = new double[(v.size() + int(close)) * 3];

    for (size_t i = 0; i != v.size(); ++i) {
        points[3 * i] = v[(int)i].x();
        points[3 * i + 1] = v[(int)i].y();
        points[3 * i + 2] = 0;
        gluTessVertex(tobj, points + 3 * i, points + 3 * i);
    }
    if (close) {
        const size_t i = v.size();
        points[3 * i] = v[0].x();
        points[3 * i + 1] = v[0].y();
        points[3 * i + 2] = 0;
        gluTessVertex(tobj, points + 3 * i, points + 3 * i);
    }
    gluTessEndContour(tobj);
    gluTessEndPolygon(tobj);
    gluDeleteTess(tobj);
}


void
GLHelper::drawBoxLine(const Position& beg, SUMOReal rot, SUMOReal visLength,
                      SUMOReal width) {
    glPushMatrix();
    glTranslated(beg.x(), beg.y(), 0);
    glRotated(rot, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2d(-width, 0);
    glVertex2d(-width, -visLength);
    glVertex2d(width, -visLength);
    glVertex2d(width, 0);
    glEnd();
    glPopMatrix();
}


void
GLHelper::drawBoxLine(const Position& beg1, const Position& beg2,
                      SUMOReal rot, SUMOReal visLength,
                      SUMOReal width) {
    glPushMatrix();
    glTranslated((beg2.x() + beg1.x())*.5, (beg2.y() + beg1.y())*.5, 0);
    glRotated(rot, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2d(-width, 0);
    glVertex2d(-width, -visLength);
    glVertex2d(width, -visLength);
    glVertex2d(width, 0);
    glEnd();
    glPopMatrix();
}


void
GLHelper::drawBoxLines(const PositionVector& geom,
                       const std::vector<SUMOReal>& rots,
                       const std::vector<SUMOReal>& lengths,
                       SUMOReal width) {
    int e = (int) geom.size() - 1;
    for (int i = 0; i < e; i++) {
        drawBoxLine(geom[i], rots[i], lengths[i], width);
    }
}


void
GLHelper::drawBoxLines(const PositionVector& geom1,
                       const PositionVector& geom2,
                       const std::vector<SUMOReal>& rots,
                       const std::vector<SUMOReal>& lengths,
                       SUMOReal width) {
    int minS = (int) MIN4(rots.size(), lengths.size(), geom1.size(), geom2.size());
    for (int i = 0; i < minS; i++) {
        GLHelper::drawBoxLine(geom1[i], geom2[i], rots[i], lengths[i], width);
    }
}


void
GLHelper::drawBoxLines(const PositionVector& geom, SUMOReal width) {
    int e = (int) geom.size() - 1;
    for (int i = 0; i < e; i++) {
        const Position& f = geom[i];
        const Position& s = geom[i + 1];
        drawBoxLine(f,
                    RAD2DEG(atan2((s.x() - f.x()), (f.y() - s.y()))),
                    f.distanceTo(s),
                    width);
    }
}


void
GLHelper::drawLine(const Position& beg, SUMOReal rot, SUMOReal visLength) {
    glPushMatrix();
    glTranslated(beg.x(), beg.y(), 0);
    glRotated(rot, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2d(0, 0);
    glVertex2d(0, -visLength);
    glEnd();
    glPopMatrix();
}


void
GLHelper::drawLine(const Position& beg1, const Position& beg2,
                   SUMOReal rot, SUMOReal visLength) {
    glPushMatrix();
    glTranslated((beg2.x() + beg1.x())*.5, (beg2.y() + beg1.y())*.5, 0);
    glRotated(rot, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2d(0, 0);
    glVertex2d(0, -visLength);
    glEnd();
    glPopMatrix();
}



void
GLHelper::drawLine(const PositionVector& v) {
    glBegin(GL_LINES);
    int e = (int) v.size() - 1;
    for (int i = 0; i < e; ++i) {
        glVertex2d(v[i].x(), v[i].y());
        glVertex2d(v[i + 1].x(), v[i + 1].y());
    }
    glEnd();
}



void
GLHelper::drawLine(const Position& beg, const Position& end) {
    glBegin(GL_LINES);
    glVertex2d(beg.x(), beg.y());
    glVertex2d(end.x(), end.y());
    glEnd();
}



void
GLHelper::drawFilledCircle(SUMOReal width, int steps) {
    drawFilledCircle(width, steps, 0, 360);
}


void
GLHelper::drawFilledCircle(SUMOReal width, int steps, SUMOReal beg, SUMOReal end) {
    if (myCircleCoords.size() == 0) {
        for (int i = 0; i < 360; i += 10) {
            const SUMOReal x = (SUMOReal) sin(DEG2RAD(i));
            const SUMOReal y = (SUMOReal) cos(DEG2RAD(i));
            myCircleCoords.push_back(std::pair<SUMOReal, SUMOReal>(x, y));
        }
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    std::pair<SUMOReal, SUMOReal> p1 =
        beg == 0 ? myCircleCoords[0] : myCircleCoords[((int) beg / 10) % 36];
    for (int i = (int)(beg / 10); i < steps && (36.0 / (SUMOReal) steps * (SUMOReal) i) * 10 < end; i++) {
        const std::pair<SUMOReal, SUMOReal>& p2 =
            myCircleCoords[(size_t)(36.0 / (SUMOReal) steps * (SUMOReal) i)];
        glBegin(GL_TRIANGLES);
        glVertex2d(p1.first * width, p1.second * width);
        glVertex2d(p2.first * width, p2.second * width);
        glVertex2d(0, 0);
        glEnd();
        p1 = p2;
    }
    const std::pair<SUMOReal, SUMOReal>& p2 =
        end == 360 ? myCircleCoords[0] : myCircleCoords[((int) end / 10) % 36];
    glBegin(GL_TRIANGLES);
    glVertex2d(p1.first * width, p1.second * width);
    glVertex2d(p2.first * width, p2.second * width);
    glVertex2d(0, 0);
    glEnd();
}


void
GLHelper::drawOutlineCircle(SUMOReal width, SUMOReal iwidth, int steps) {
    drawOutlineCircle(width, iwidth, steps, 0, 360);
}


void
GLHelper::drawOutlineCircle(SUMOReal width, SUMOReal iwidth, int steps,
                            SUMOReal beg, SUMOReal end) {
    if (myCircleCoords.size() == 0) {
        for (int i = 0; i < 360; i += 10) {
            SUMOReal x = (SUMOReal) sin(DEG2RAD(i));
            SUMOReal y = (SUMOReal) cos(DEG2RAD(i));
            myCircleCoords.push_back(std::pair<SUMOReal, SUMOReal>(x, y));
        }
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    std::pair<SUMOReal, SUMOReal> p1 =
        beg == 0 ? myCircleCoords[0] : myCircleCoords[((int) beg / 10) % 36];
    for (int i = (int)(beg / 10); i < steps && (36.0 / (SUMOReal) steps * (SUMOReal) i) * 10 < end; i++) {
        const std::pair<SUMOReal, SUMOReal>& p2 =
            myCircleCoords[(size_t)(36.0 / (SUMOReal) steps * (SUMOReal) i)];
        glBegin(GL_TRIANGLES);
        glVertex2d(p1.first * width, p1.second * width);
        glVertex2d(p2.first * width, p2.second * width);
        glVertex2d(p2.first * iwidth, p2.second * iwidth);

        glVertex2d(p2.first * iwidth, p2.second * iwidth);
        glVertex2d(p1.first * iwidth, p1.second * iwidth);
        glVertex2d(p1.first * width, p1.second * width);
        glEnd();
        p1 = p2;
    }
    const std::pair<SUMOReal, SUMOReal>& p2 =
        end == 360 ? myCircleCoords[0] : myCircleCoords[((int) end / 10) % 36];
    glBegin(GL_TRIANGLES);
    glVertex2d(p1.first * width, p1.second * width);
    glVertex2d(p2.first * width, p2.second * width);
    glVertex2d(p2.first * iwidth, p2.second * iwidth);

    glVertex2d(p2.first * iwidth, p2.second * iwidth);
    glVertex2d(p1.first * iwidth, p1.second * iwidth);
    glVertex2d(p1.first * width, p1.second * width);
    glEnd();
}


void
GLHelper::drawTriangleAtEnd(const Line& l, SUMOReal tLength,
                            SUMOReal tWidth) {
    if (l.length() < tLength) {
        tWidth = tWidth * l.length() / tLength;
        tLength = l.length();
    }
    Line rl(l.getPositionAtDistance(l.length() - tLength), l.p2());
    glPushMatrix();
    glTranslated(rl.p1().x(), rl.p1().y(), 0);
    glRotated(-l.atan2DegreeAngle(), 0, 0, 1);
    glBegin(GL_TRIANGLES);
    glVertex2d(0, -tLength);
    glVertex2d(-tWidth, 0);
    glVertex2d(+tWidth, 0);
    glEnd();
    glPopMatrix();
}


void
GLHelper::setColor(const RGBColor& c) {
    glColor4ub(c.red(), c.green(), c.blue(), c.alpha());
}


RGBColor
GLHelper::getColor() {
    GLdouble current[4];
    glGetDoublev(GL_CURRENT_COLOR, current);
    return RGBColor(static_cast<unsigned char>(current[0] * 255. + 0.5),
                    static_cast<unsigned char>(current[1] * 255. + 0.5),
                    static_cast<unsigned char>(current[2] * 255. + 0.5),
                    static_cast<unsigned char>(current[3] * 255. + 0.5));
}


void
GLHelper::drawText(const std::string& text, const Position& pos,
                   const SUMOReal layer, const SUMOReal size,
                   const RGBColor& col, const SUMOReal angle) {
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    setColor(col);
    glTranslated(pos.x(), pos.y(), layer);
    pfSetPosition(0, 0);
    pfSetScale(size);
    SUMOReal w = pfdkGetStringWidth(text.c_str());
    glRotated(180, 1, 0, 0);
    glRotated(angle, 0, 0, 1);
    glTranslated(-w / 2., 0.4, 0);
    pfDrawString(text.c_str());
    glPopMatrix();
}

void
GLHelper::drawTextBox(const std::string& text, const Position& pos,
                      const SUMOReal layer, const SUMOReal size,
                      const RGBColor& txtColor, const RGBColor& bgColor, const RGBColor& borderColor,
                      const SUMOReal angle) {
    SUMOReal boxAngle = angle + 90;
    if (boxAngle > 360) {
        boxAngle -= 360;
    }
    pfSetScale(size);
    const SUMOReal stringWidth = pfdkGetStringWidth(text.c_str());
    const SUMOReal borderWidth = size / 20;
    const SUMOReal boxHeight = size * 0.8;
    const SUMOReal boxWidth = stringWidth + size / 2;
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTranslated(0, 0, layer);
    setColor(borderColor);
    Position left = pos;
    left.sub(boxWidth / 2, -boxHeight / 2.7);
    drawBoxLine(left, boxAngle, boxWidth, boxHeight);
    left.add(borderWidth * 1.5, 0);
    setColor(bgColor);
    glTranslated(0, 0, 0.01);
    drawBoxLine(left, boxAngle, boxWidth - 3 * borderWidth, boxHeight - 2 * borderWidth);
    // actually we should be able to use drawText here. however, there's
    // something about the constant 0.4 offset which causes trouble
    //drawText(text, pos, layer+0.02, size, txtColor, angle);
    setColor(txtColor);
    glTranslated(pos.x(), pos.y(), 0.01);
    pfSetPosition(0, 0);
    pfSetScale(size);
    glRotated(180, 1, 0, 0);
    glRotated(angle, 0, 0, 1);
    glTranslated(-stringWidth / 2., 0, 0);
    pfDrawString(text.c_str());
    glPopMatrix();
}

/****************************************************************************/

