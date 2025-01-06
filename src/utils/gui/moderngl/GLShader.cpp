#include "GLShader.h"
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
/// @file    GLShader.cpp
/// @author  Mirko Barthauer
/// @date    17.01.2025
///
//
/****************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>

#include "GLShader.h"


GLShader::GLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    std::string vertexShaderDef = readShaderFile(vertexShaderPath);
    std::string fragmentShaderDef = readShaderFile(fragmentShaderPath);
    myVertexShaderID = compileShader(GL_VERTEX_SHADER, vertexShaderDef);
    myFragmentShaderID = compileShader(GL_FRAGMENT_SHADER, fragmentShaderDef);
}


GLShader::~GLShader() {
}


std::string GLShader::readShaderFile(const std::string& path) const {
    if (!FileHelpers::isReadable(path)) {
        throw InvalidArgument("Cannot load GL shader definition " + path);
    }
    std::ifstream file(path);
    std::stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
    }
    return ss.str();
}


GLuint GLShader::compileShader(GLuint type, const std::string& source) {
    GLuint shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
#ifdef _DEBUG
    std::cout << "GLShader::compileShader glCompileShader(" << shaderID << ")" << std::endl;
#endif
    int result;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        std::string log;
        log.resize(length);
        glGetShaderInfoLog(shaderID, length, &length, &log[0]);
        std::string typeName = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
        throw ProcessError("Could not compile GL " + typeName + " shader: " + log);
    }
    return shaderID;
}
