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
#include <iostream>

#define GLVERTEXARRAYOBJECT_MINRESERVE 4000


GLVertexArrayObject::GLVertexArrayObject(unsigned int vertexSize, unsigned int itemSize):
    myID(0), myVertexBufferID(0), myIndexBufferID(0), myVertexBufferSize(0), myIndexBufferSize(0), myVertexInputSize(vertexSize), myItemSize(itemSize), myGeometryType(GL_TRIANGLES){

    glGenVertexArrays(1, &myID);
#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::GLVertexArrayObject glGenVertexArrays(" << myID << ")" << std::endl;
#endif
    bind();

#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::GLVertexArrayObject ID " << myID << " glGenBuffers/glBindBuffer(" << myVertexBufferID << " | " << myIndexBufferID << ")" << std::endl;
#endif
    resizeBuffers(myVertexInputSize, myVertexInputSize);
    unbind();
}


GLVertexArrayObject::~GLVertexArrayObject() {
    glDeleteVertexArrays(1, &myID);
}


GLVertexArrayObject::GLVertexArrayObject(const GLVertexArrayObject& copy): 
    myID(0), myVertexBufferID(0), myIndexBufferID(0), myVertexBufferSize(copy.getVertexSize()), myIndexBufferSize(copy.getIndexSize()), myItemSize(10000) {

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


GLenum
GLVertexArrayObject::getGeometryType() const {
    return myGeometryType;
}


unsigned long long
GLVertexArrayObject::getVertexSize() const {
    return myVertexBufferSize;
}


unsigned long long
GLVertexArrayObject::getIndexSize() const {
    return myIndexBufferSize;
}


void
GLVertexArrayObject::resizeBuffers(int newVertexSize, int newIndexSize) {
    // TODO: look into creating a new buffer on GPU and transferring the existing data with glCopyBufferSubData
    // see https://docs.gl/gl3/glCopyBufferSubData // ??? but what if we transmit data for every time step anyway?

    // delete old ones
    if (myVertexBufferID > 0) {
        glDeleteBuffers(1, &myVertexBufferID);
    }
    if (myIndexBufferID > 0) {
        glDeleteBuffers(1, &myIndexBufferID);
    }

    // create new ones
    glGenBuffers(1, &myVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, newVertexSize * myItemSize, nullptr, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &myIndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newIndexSize * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

    myVertexBufferSize = newVertexSize;
    myIndexBufferSize = newIndexSize;

#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::resizeBuffers to vertex size " << myVertexBufferSize << std::endl;
#endif
}


void
GLVertexArrayObject::setGeometryType(GLenum type) {
    myGeometryType = type;
}


void
GLVertexArrayObject::setItemSize(const unsigned long long vertexCount, const unsigned long long indexCount) {
    if (vertexCount - myVertexBufferSize <= 0) {
        resizeBuffers(vertexCount + GLVERTEXARRAYOBJECT_MINRESERVE, indexCount + GLVERTEXARRAYOBJECT_MINRESERVE);
    }
}


bool
GLVertexArrayObject::addVertexData(std::vector<GLBufferStruct>& data) {
    unsigned long long offset = myVertexInputSize;

    const long byteSize = data.size() * sizeof(GLBufferStruct);
    unsigned long long addSize = data.size();
    glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, &data[0]);
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    if (byteSize > bufferSize) {
        glDeleteBuffers(1, &myVertexBufferID);
        // TODO: Log the error
        return false;
    }
    // demo index buffer
    std::vector<unsigned int> indexBufferVals;
    for (int i = 0; i < addSize; ++i) {
        indexBufferVals.push_back(i);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * addSize, &indexBufferVals[0]);
    myVertexInputSize += addSize;
#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::addVertexData of " << myVertexInputSize << " vertices starting at offset " << offset << std::endl;
#endif
    return true;
}


bool
GLVertexArrayObject::setVertexData(std::vector<GLBufferStruct>& data) {
    // TODO: to be replaced by addVertexData with offset 0
    return addVertexData(data);

    /*
    const long byteSize = data.size() * sizeof(GLBufferStruct);
    myVertexInputSize = data.size();
    glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, &data[0]);
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    if (byteSize > bufferSize) {
        glDeleteBuffers(1, &myVertexBufferID);
        // TODO: Log the error
        return false;
    }
    // demo index buffer
    std::vector<unsigned int> indexBufferVals;
    for (int i = 0; i < myVertexInputSize; ++i) {
        indexBufferVals.push_back(i);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * myVertexInputSize, &indexBufferVals[0]);

#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::setVertexData of " << myVertexInputSize << " vertices" << std::endl;
#endif
    return true;
    */
}


void
GLVertexArrayObject::drawGL() const {
    glDrawElements(myGeometryType, myVertexInputSize, GL_UNSIGNED_INT, nullptr);
#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::drawGL glDrawArrays(GL_TRIANGLES, 0, " << myVertexInputSize <<");" << std::endl;
#endif
}


void GLVertexArrayObject::bind() const {
    glBindVertexArray(myID);
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexBufferID);
#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::bind glBindVertexArray(" << myID << ")" << std::endl;
#endif
}


void GLVertexArrayObject::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::unbind glBindVertexArray(0)" << std::endl;
#endif
}
