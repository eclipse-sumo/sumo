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
/// @file    GLRenderer.h
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

#include <glm/glm.hpp>

#include <map>
#include <vector>
#include <memory>
#include <string>

struct GLAttributeDefinition;
struct GLBufferStruct;
class GLVertexArrayObject;
class GLShader;

struct GLConfiguration {
    std::string shaderName;
    GLuint programID;
    std::shared_ptr<GLVertexArrayObject> vao;
};

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    void addConfiguration(const std::string& name, const std::string& shaderName, const unsigned int itemSize);
    void activateConfiguration(const std::string& name);
    std::shared_ptr<GLVertexArrayObject> getVAO();
    void deactivateCurrentConfiguration();
    bool addShader(const std::string& name, const GLShader& shader);
    void setVertexAttributes(const std::vector<GLAttributeDefinition>& attributeDefinitions);
    void checkBufferSizes();
    void clearBuffer();
    GLuint getUniformID(const std::string& key);
    bool setVertexData(std::vector<GLBufferStruct>& data, GLenum type);
    void setUniform(const std::string& key, const float value);
    void setUniform(const std::string& key, const float v1, const float v2, const float v3);
    void setUniform(const std::string& key, const float v1, const float v2, const float v3, const float v4);
    void setUniform(const std::string& key, const glm::mat4& mat);
    void paintGL();

private:
    std::map<std::string, GLuint> myUniforms;
    std::map<std::string, GLConfiguration> myConfigurations;
    std::string myCurrentConfiguration;
    GLuint myProgramID;

    static std::map<std::string, std::pair<GLuint, GLuint>> myShaders;
};