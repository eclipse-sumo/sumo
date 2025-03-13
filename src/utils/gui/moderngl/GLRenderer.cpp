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

#include <utils/gui/moderngl/GLBufferStruct.h>

// ===========================================================================
// static member definitions
// ===========================================================================
std::map<std::string, std::pair<GLuint, GLuint>> GLRenderer::myShaders;

// ===========================================================================
// method definitions
// ===========================================================================

GLRenderer::GLRenderer(): myProgramID(0) {
#ifdef _DEBUG
    std::cout << "GLRenderer::GLRenderer glCreateProgram()" << std::endl;
#endif
}


GLRenderer::~GLRenderer() {
    if (myConfigurations.size() > 0) {
        for (auto configuration : myConfigurations) {
            glDeleteProgram(configuration.second.programID);
#ifdef _DEBUG
            std::cout << "GLRenderer::~GLRenderer glDeleteProgram(" << configuration.second.programID << ")" << std::endl;
#endif
        }
    }

}


void
GLRenderer::addConfiguration(const std::string& name, const std::string& shaderName, const unsigned int itemSize) {
    if (myConfigurations.find(name) != myConfigurations.end()) {
        throw ProcessError("The GLConfiguration '" + name + "' has already been defined.");
    }
    if (myShaders.find(shaderName) == myShaders.end()) {
        throw ProcessError("The requested shader definition '" + shaderName + "' has not been defined.");
    }
    GLuint programID = glCreateProgram();
    glAttachShader(programID, myShaders[shaderName].first);
    glAttachShader(programID, myShaders[shaderName].second);
    glLinkProgram(programID);
    glValidateProgram(programID);

#ifdef _DEBUG
    std::cout << "GLRenderer::addConfiguration validate program " << std::endl;
#endif
    glDeleteShader(myShaders[shaderName].first);
    glDeleteShader(myShaders[shaderName].second);

    //throw exception if linking failed
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        glDeleteProgram(programID);
        throw ProcessError("Program linking failed.");
    }

    // add configuration struct
    myConfigurations[name] = { shaderName, programID, std::make_shared<GLVertexArrayObject>(itemSize, 30000) };

#ifdef _DEBUG
    std::cout << "GLRenderer::addConfiguration added configuration '" << name << "'" << std::endl;
#endif
}


void
GLRenderer::activateConfiguration(const std::string& name) {
    if (myConfigurations.find(name) == myConfigurations.end()) {
        throw ProcessError("The GL configuration '" + name + "' is not known.");
    }
    myCurrentConfiguration = name;
    myProgramID = myConfigurations[myCurrentConfiguration].programID;
    glUseProgram(myProgramID);
    myConfigurations[myCurrentConfiguration].vao->bind();
#ifdef _DEBUG
    std::cout << "GLRenderer::addConfiguration activated configuration '" << name << "' with programID " << myProgramID << std::endl;
#endif
}


std::shared_ptr<GLVertexArrayObject>
GLRenderer::getVAO() {
    return myConfigurations[myCurrentConfiguration].vao;
}


void
GLRenderer::deactivateCurrentConfiguration() {
    glUseProgram(0);
    myConfigurations[myCurrentConfiguration].vao->unbind();
    myCurrentConfiguration = "";
    myProgramID = 0;
}


bool
GLRenderer::addShader(const std::string& name, const GLShader& shader) {
    if (myShaders.find(name) != myShaders.end()) {
        return false;
    }
    myShaders[name] = std::make_pair<GLuint, GLuint>(shader.getVertexShaderID(), shader.getFragmentShaderID());
    return true;
}


void
GLRenderer::setVertexAttributes(const std::vector<std::pair<GLint, unsigned int>>& attributeDefinitions) {
    if (myCurrentConfiguration == "") {
        // TODO: issue a warning that no VAO is set
        return;
    }
    unsigned int attributeCount = attributeDefinitions.size();
    std::vector<unsigned int> sizes;
    unsigned int stride = 0;
    for (auto entry : attributeDefinitions) {
        unsigned int typeSize = 0;
        switch (entry.first) {
        case GL_FLOAT:
            typeSize = sizeof(float);
            break;
        case GL_BYTE:
            typeSize = sizeof(char);
            break;
        }
        sizes.push_back(typeSize * entry.second);
        stride += typeSize * entry.second;
    }
    GLintptr offset = 0;
    for (int i = 0; i < attributeCount; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attributeDefinitions[i].second, attributeDefinitions[i].first, GL_FALSE, stride, (GLvoid*)offset);
#ifdef _DEBUG
        std::cout << "GLRenderer::setVertexAttributes glVertexAttribPointer position " << i << " offset " << offset << std::endl;
#endif
        offset += sizes[i];
    }
}


void
GLRenderer::checkBufferSizes() {
    int vertices = GLHelper::getVertexCounterModern();
    if (vertices > myConfigurations[myCurrentConfiguration].vao->getVertexSize()) {
        myConfigurations[myCurrentConfiguration].vao->setItemSize(vertices, vertices);
    }
}


void
GLRenderer::clearBuffer() {
    if (!myCurrentConfiguration.empty()) {
        myConfigurations[myCurrentConfiguration].vao->clearBuffer();
    }
}


GLuint
GLRenderer::getUniformID(const std::string& key) {
    if (myUniforms.find(key) == myUniforms.end()) {
        GLint result = glGetUniformLocation(myProgramID, key.c_str());
        if (result < 0) {
            throw ProcessError("The OpenGL shader uniform variable " + key + " could not be accessed.");
        } else {
            myUniforms[key] = glGetUniformLocation(myProgramID, key.c_str());
        }
    }
    return myUniforms[key];
}


bool
GLRenderer::setVertexData(std::vector<GLBufferStruct>& data) {
    return myConfigurations[myCurrentConfiguration].vao->setVertexData(data);
}


void
GLRenderer::setUniform(const std::string& key, const float value) {
    glUniform1f(getUniformID(key), value);
}


void
GLRenderer::setUniform(const std::string& key, const float v1, const float v2, const float v3) {
    glUniform3f(getUniformID(key), v1, v2, v3);
}


void
GLRenderer::setUniform(const std::string& key, const float v1, const float v2, const float v3, const float v4) {
    glUniform4f(getUniformID(key), v1, v2, v3, v4);
}


void
GLRenderer::setUniform(const std::string& key, const glm::mat4& mat) {
    glUniformMatrix4fv(getUniformID(key), 1, GL_FALSE, &mat[0][0]);
}


void GLRenderer::paintGL() {
    myConfigurations[myCurrentConfiguration].vao->drawGL();
}
