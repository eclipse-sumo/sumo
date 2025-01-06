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
/// @file    GLShader.h
/// @author  Mirko Barthauer
/// @date    17.01.2025
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#ifndef __glew_h__
#include <GL/glew.h>
#endif
#include <string>

class GLShader {

public:
    GLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~GLShader();

    GLuint getVertexShaderID() const { return myVertexShaderID; };
    GLuint getFragmentShaderID() const { return myFragmentShaderID; };
    std::string readShaderFile(const std::string& path) const;

private:
    GLuint compileShader(GLuint type, const std::string& source);

private:
    GLuint myVertexShaderID;
    GLuint myFragmentShaderID;
};