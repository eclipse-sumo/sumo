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
/****************************************************************************/
#include "GLVertexArrayObject.h"
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/gui/moderngl/GLStructs.h>

#define GLVERTEXARRAYOBJECT_GROWFACTOR 2

#define DEBUG_VBO

GLVertexArrayObject::GLVertexArrayObject(unsigned int itemSize, unsigned int vertexSize)
    : myID(0), myVertexBufferID(0), myIndexBufferID(0), myVertexBufferSize(0), myIndexBufferSize(0), myVertexInputSize(0), myItemSize(itemSize) {

    glGenVertexArrays(1, &myID);
#ifdef DEBUG_VBO
    std::cout << "GLVertexArrayObject::GLVertexArrayObject glGenVertexArrays(" << myID << ")" << std::endl;
#endif
    bind();

#ifdef DEBUG_VBO
    std::cout << "GLVertexArrayObject::GLVertexArrayObject ID " << myID << " glGenBuffers/glBindBuffer(" << myVertexBufferID << " | " << myIndexBufferID << ")" << std::endl;
#endif
    resizeBuffers(vertexSize, vertexSize);
    unbind();
}


GLVertexArrayObject::~GLVertexArrayObject() {
    if (myVertexBufferID > 0) {
        glDeleteBuffers(1, &myVertexBufferID);
    }
    if (myIndexBufferID > 0) {
        glDeleteBuffers(1, &myIndexBufferID);
    }
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


unsigned long long
GLVertexArrayObject::getVertexSize() const {
    return myVertexBufferSize;
}


unsigned long long
GLVertexArrayObject::getIndexSize() const {
    return myIndexBufferSize;
}


void
GLVertexArrayObject::setAttributes(const std::vector<GLAttributeDefinition>& attributes) {
    myAttributes.clear();
    unsigned int attributeCount = attributes.size();
    std::vector<unsigned int> sizes;
    unsigned int stride = 0;
    for (auto entry : attributes) {
        unsigned int typeSize = 0;
        switch (entry.type) {
        case GL_FLOAT:
            typeSize = sizeof(float);
            break;
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            typeSize = sizeof(char);
            break;
        }
        sizes.push_back(typeSize * entry.size);
        stride += typeSize * entry.size;
    }
    GLintptr offset = 0;
    for (int i = 0; i < attributeCount; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attributes[i].size, attributes[i].type, attributes[i].normalized, stride, (GLvoid*)offset);
#ifdef DEBUG_VBO
        std::cout << "GLVertexArrayObject::setAttributes glVertexAttribPointer position " << i << " offset " << offset << std::endl;
#endif
        offset += sizes[i];
        myAttributes.push_back(attributes[i]);
    }

}


void
GLVertexArrayObject::resizeBuffers(int newVertexSize, int newIndexSize) {
    // TODO: look into creating a new buffer on GPU and transferring the existing data with glCopyBufferSubData
    // see https://docs.gl/gl3/glCopyBufferSubData // ??? but what if we transmit data for every time step anyway?

    // check if the expected size exceeds the maximum recommended GPU given size
#ifdef DEBUG_VBO
    GLint64 maxVertexCount;
    glGetInteger64v(GL_MAX_ELEMENTS_VERTICES, &maxVertexCount);
    if (maxVertexCount < newVertexSize) {
        std::cout << "GLVertexArrayObject::resizeBuffers should not save more than " << maxVertexCount << " vertices on GPU" << std::endl;
    }
    std::cout << "GLVertexArrayObject::resizeBuffers attempt to resize to accomodate " << newVertexSize << " vertices" << std::endl;
#endif

    // create new ones
    if (myVertexBufferID == 0) {
        glGenBuffers(1, &myVertexBufferID);
    }
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, newVertexSize * myItemSize, nullptr, GL_DYNAMIC_DRAW);
#ifdef _DEBUG
    std::cout << "GLVertexArrayObject::resizeBuffers created new array buffer " << myVertexBufferID << std::endl;
#endif

    if (myIndexBufferID == 0) {
        glGenBuffers(1, &myIndexBufferID);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newIndexSize * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
#ifdef DEBUG_COND
    std::cout << "GLVertexArrayObject::resizeBuffers created new element array buffer " << myIndexBufferID << std::endl;
#endif

    myVertexBufferSize = newVertexSize;
    myIndexBufferSize = newIndexSize;

#ifdef DEBUG_VBO
    // check if the buffer could be acquired
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    if (bufferSize < newVertexSize * myItemSize) {
        std::cout << "GLVertexArrayObject::resizeBuffers created a smaller buffer than requested" << std::endl;
    } else {
        std::cout << "GLVertexArrayObject::resizeBuffers to vertex size " << myVertexBufferSize << std::endl;
    }
#endif
}


void
GLVertexArrayObject::setItemSize(const unsigned long long vertexCount, const unsigned long long indexCount) {
    long long diff = myVertexBufferSize - vertexCount;
    if (diff < 0) {
        resizeBuffers(vertexCount * GLVERTEXARRAYOBJECT_GROWFACTOR, indexCount * GLVERTEXARRAYOBJECT_GROWFACTOR);
    }
}


void
GLVertexArrayObject::clearBuffer() {
#ifdef DEBUG_VBO
        std::cout << "GLVertexArrayObject::clearBuffer " << myGeometries.size() << " geometries begin" << std::endl;
#endif
    glBufferSubData(GL_ARRAY_BUFFER, 0, myVertexInputSize * myItemSize, nullptr);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, myVertexInputSize * sizeof(unsigned int), nullptr);
    myVertexInputSize = 0;
    myGeometries.clear();
#ifdef DEBUG_COND
        std::cout << "GLVertexArrayObject::clearBuffer " << myGeometries.size() << " geometries end" << std::endl;
#endif
}


bool
GLVertexArrayObject::addVertexData(std::vector<GLBufferStruct>& data, GLenum geometryType) {
    unsigned long long offset = myVertexInputSize;
    unsigned long long addSize = data.size();
    if (addSize > myVertexBufferSize) {
        setItemSize(addSize, addSize);
    }
    const long byteSize = addSize * myItemSize;
    glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, &data[0]);
    // demo index buffer
    std::vector<unsigned int> indexBufferVals;
    for (int i = 0; i < addSize; ++i) {
        indexBufferVals.push_back(i);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * addSize, &indexBufferVals[0]);
    myVertexInputSize += addSize;
    myGeometries.push_back({ geometryType, addSize });
#ifdef DEBUG_COND
    std::cout << "GLVertexArrayObject::addVertexData of " << addSize << " vertices starting at offset " << offset << std::endl;
#endif
    return true;
}


void
GLVertexArrayObject::drawGL() const {
    unsigned long long offset = 0;

#ifdef DEBUG_VBO
    std::cout << "GLVertexArrayObject::drawGL draw " << myGeometries.size() << " geometries " << std::endl;
#endif
    for (auto entry : myGeometries) {
        glDrawElements((GLenum)entry.first, entry.second, GL_UNSIGNED_INT, (GLvoid*)offset);
#ifdef DEBUG_VBO
        std::cout << "GLVertexArrayObject::drawGL glDrawElements(" << entry.first << ", " << offset << ", " << entry.second << ");" << std::endl;
#endif
        offset += entry.second;
    }
}


void GLVertexArrayObject::bind() const {
    glBindVertexArray(myID);
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexBufferID);
#ifdef DEBUG_VBO
    std::cout << "GLVertexArrayObject::bind glBindVertexArray(" << myID << ")" << std::endl;
#endif
}


void GLVertexArrayObject::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#ifdef DEBUG_VBO
    std::cout << "GLVertexArrayObject::unbind glBindVertexArray(0)" << std::endl;
#endif
}
