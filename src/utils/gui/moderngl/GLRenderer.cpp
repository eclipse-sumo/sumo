/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GLRenderer.cpp
/// @author  Mirko Barthauer
/// @date    27.01.2025
///
//
/****************************************************************************/
#include "GLRenderer.h"

GLRenderer::GLRenderer() {

}


GLRenderer::~GLRenderer() {
}


bool
GLRenderer::addVAO(GUIGlObjectType type, GLVertexArrayObject* vao) {
    if (myTypeToVAO.find(type) == myTypeToVAO.end()) {
        myTypeToVAO[type] = vao;
        return true;
    }
    return false;
}


void
GLRenderer::checkBufferSizes() {
    // TODO: check for each draw call if the graphics card buffers for vertices etc. are still sufficiently large to 
    //       keep up with the number of faces to be drawn
    for (auto const& vao : myTypeToVAO) {
        // get the number of vertices for each separate VAO = each separate object type


        // in case it is needed: update buffer size
        

    }
}


void GLRenderer::paintGL() {
    // TODO: mutex?
    // render call: loop through VAO according to the drawing order of object types
    for(auto const& vao : myTypeToVAO) {
        // TODO: issue OpenGL draw command such as with vao 
        vao.second->bind();

    }
}
