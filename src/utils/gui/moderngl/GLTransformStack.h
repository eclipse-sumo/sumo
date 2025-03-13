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
/// @file    GLTransformStack.h
/// @author  Mirko Barthauer
/// @date    03.03.2025
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <stack>
#define GLM_SWIZZLE
#include <glm.hpp>

// TODO: using a singleton pattern 

// ===========================================================================
// class declarations
// ===========================================================================

// ===========================================================================
// class definitions
// ===========================================================================
class GLTransformStack {

public:
    /// @brief Retrieves the options
    static GLTransformStack& getTransformStack();

    GLTransformStack();
    ~GLTransformStack();

    void reset();
    void pushMatrix();
    void popMatrix();
    void translate(const glm::vec3& t);
    void rotate(const float angle, const glm::vec3& axis);
    void scale(const glm::vec3& s);
    void scale(const double s);
    glm::vec4 applyTransform(glm::vec3& v) const;

private:
    void updateCurrentMatrix();

    /// @brief default copy constructor, but private
    GLTransformStack(const GLTransformStack& s) = default;

    /// @brief invalid assignment operator
    GLTransformStack& operator=(const GLTransformStack& s) = delete;

private:
    static GLTransformStack myInstance;

    std::stack<glm::mat4x4> myStack;
    glm::mat4x4 myCurrentTransform;
};