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
/// @file    GLVertexArrayObject.h
/// @author  Mirko Barthauer
/// @date    27.01.2025
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#ifndef __glew_h__
#include <GL/glew.h>
#endif

struct GLBufferStruct;
struct GLAttributeDefinition;

class GLVertexArrayObject {

public:
    GLVertexArrayObject(unsigned int itemSize, unsigned int vertexSize = 0);
    ~GLVertexArrayObject();
    GLVertexArrayObject(const GLVertexArrayObject& copy);

    GLuint getID() const;
    GLuint getVertexBufferID() const;
    GLuint getIndexBufferID() const;
    GLenum getGeometryType() const;
    unsigned long long getVertexSize() const;
    unsigned long long getIndexSize() const;
    void setAttributes(const std::vector<GLAttributeDefinition>& attributes);
    void setGeometryType(GLenum type);
    void setItemSize(const unsigned long long vertexCount, const unsigned long long indexCount);
    void clearBuffer();
    bool addVertexData(std::vector<GLBufferStruct>& data, GLenum geometryType = GL_TRIANGLES);
    void drawGL() const;
    void bind() const;
    void unbind() const;

private:
    void resizeBuffers(int newVertexSize, int newIndexSize);

private:
    GLuint myID;
    GLuint myVertexBufferID;
    GLuint myIndexBufferID;
    unsigned int myItemSize;
    unsigned long long myVertexBufferSize;
    unsigned long long myIndexBufferSize;
    unsigned long long myVertexInputSize;
    std::vector< GLAttributeDefinition> myAttributes;
    std::vector <std::pair<GLenum, unsigned long long>> myGeometries;
};