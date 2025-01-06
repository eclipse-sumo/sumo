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

#ifndef __glew_h__
#include <GL/glew.h>
#endif

#include <utils/gui/moderngl/GLBufferStruct.h>


class GLVertexArrayObject {

public:
    GLVertexArrayObject();

    ~GLVertexArrayObject();

    GLuint getID() const;
    GLuint getVertexBufferID() const;
    GLuint getIndexBufferID() const;
    unsigned long long getVertexSize() const;
    unsigned long long getIndexSize() const;

    void setItemSize(const unsigned long long vertexCount, const unsigned long long indexCount);
    void bind() const;
    void unbind() const;

private:
    void resizeBuffers();

private:
    GLuint myID;
    GLuint myVertexBufferID;
    GLuint myIndexBufferID;
    GLuint myShaderID;
    unsigned long long myVertexSize;
    unsigned long long myIndexSize;

};