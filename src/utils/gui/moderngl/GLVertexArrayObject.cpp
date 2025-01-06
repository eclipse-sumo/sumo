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
/// @file    GLVertexArrayObject.cpp
/// @author  Mirko Barthauer
/// @date    27.01.2025
///
//
/****************************************************************************/
#include "GLVertexArrayObject.h"


GLVertexArrayObject::GLVertexArrayObject():
    myID(0), myVertexBufferID(0), myIndexBufferID(0), myVertexSize(10000), myIndexSize(10000) {

    glGenVertexArrays(1, &myID);
    bind();

    // start with an assumed number of possible objects and max vertices
    glGenBuffers(1, &myVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferID);
    glGenBuffers(1, &myIndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexBufferID);
    resizeBuffers();
    unbind();
}


GLVertexArrayObject::~GLVertexArrayObject() {
    glDeleteVertexArrays(1, &myID);
}


GLuint
GLVertexArrayObject::getID() const {
    return myID;
}


GLuint
GLVertexArrayObject::getVertexBufferID() const {
    return myVertexBufferID;
}


GLuint
GLVertexArrayObject::getIndexBufferID() const {
    return myIndexBufferID;
}


unsigned long long
GLVertexArrayObject::getVertexSize() const {
    return myVertexSize;
}


unsigned long long
GLVertexArrayObject::getIndexSize() const {
    return myIndexSize;
}


void
GLVertexArrayObject::resizeBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, myVertexSize * GLBufferStruct::getSize(), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myIndexSize * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
}


void
GLVertexArrayObject::setItemSize(const unsigned long long vertexCount, const unsigned long long indexCount) {
    bind();
    myVertexSize = vertexCount;
    myIndexSize = indexCount;
    resizeBuffers();
    unbind();
}


void GLVertexArrayObject::bind() const {
    glBindVertexArray(myID);
}


void GLVertexArrayObject::unbind() const {
    glBindVertexArray(0);
}
