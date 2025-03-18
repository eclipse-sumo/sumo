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
/// @file    GLTransformStack.cpp
/// @author  Mirko Barthauer
/// @date    03.03.2025
///
//
/****************************************************************************/
#pragma once
#include <config.h>
#include <iostream>

#include "GLTransformStack.h"
#include "gtc/matrix_transform.hpp"
// #define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

// ===========================================================================
// static member definitions
// ===========================================================================

GLTransformStack GLTransformStack::myInstance = GLTransformStack();

// ===========================================================================
// method definitions
// ===========================================================================


GLTransformStack&
GLTransformStack::getTransformStack() {
    return myInstance;
}


GLTransformStack::GLTransformStack() {
}


GLTransformStack::~GLTransformStack() {
}


void
GLTransformStack::reset() {
    myStack.clear();
}


void
GLTransformStack::pushMatrix() {
    if (myStack.size() > 0) {
        myStack.push_back(glm::mat4(myStack.back()));
    } else {
        myStack.push_back(glm::mat4(1.f));
    }
}


void
GLTransformStack::popMatrix() {
    myStack.pop_back();
}


void
GLTransformStack::translate(const glm::vec3& t) {
    *myStack.rbegin() = glm::translate(*myStack.rbegin(), t);
}


void
GLTransformStack::rotate(const float angle, const glm::vec3& axis) {
    // TODO: how???
    // subtract current pivot point and add it again after the transformation
    glm::mat4 currentMatrix = *myStack.rbegin();
    currentMatrix = glm::rotate(currentMatrix, angle, axis);

#ifdef _DEBUG
    std::cout << "GLTransformStack::rotate by " << angle << " deg = " << angle << ",\n\tmatrix " << glm::to_string(currentMatrix) << std::endl;
#endif

    *myStack.rbegin() = currentMatrix;
}


void
GLTransformStack::scale(const glm::vec3& s) {
    *myStack.rbegin() = glm::scale(*myStack.rbegin(), s);
}


void
GLTransformStack::scale(const double s) {
    scale(glm::vec3(s));
}


glm::vec3
GLTransformStack::applyTransform(const glm::vec3& v) const {
    if (myStack.size() == 0) {
        return v;
    }
    glm::vec4 result = myStack.back() * glm::vec4(v, 1.);

#ifdef _DEBUG
    // plot stored matrices and the currently cached one, input and result of the transformation
    std::cout << "GLTransformStack::applyTransform(" << glm::to_string(v) << ")" << std::endl;
    std::cout << "\tcached matrix: " << glm::to_string(myStack.back()) << std::endl;
    std::cout << "\tresult vector: " << glm::to_string(result) << std::endl;
#endif

    return glm::vec3(result.x / result.w, result.y / result.w, result.z / result.w);
    //return (myCurrentTransform * glm::vec4(v, 1.)).xyz;
}