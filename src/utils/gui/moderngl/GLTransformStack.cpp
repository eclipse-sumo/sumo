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
#include "GLTransformStack.h"
#include "gtc/matrix_transform.hpp"

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


GLTransformStack::GLTransformStack(): myCurrentTransform(glm::mat4(1.f)) {
    myStack.push(myCurrentTransform);
}


GLTransformStack::~GLTransformStack() {
}


void
GLTransformStack::reset() {
    while (myStack.size() > 0) {
        myStack.pop();
    }
    myCurrentTransform = glm::mat4(1.f);
}


void
GLTransformStack::pushMatrix() {
    myStack.push(glm::mat4(1.f));
}


void
GLTransformStack::popMatrix() {
    myStack.pop();
    updateCurrentMatrix();
}


void
GLTransformStack::translate(const glm::vec3& t) {
    myStack.push(glm::translate(glm::mat4(1.f), t));
    updateCurrentMatrix();
}


void
GLTransformStack::rotate(const float angle, const glm::vec3& axis) {
    // TODO: how???
    myStack.push(glm::rotate(glm::mat4(1.f), glm::radians(angle), axis));
    updateCurrentMatrix();
}


void
GLTransformStack::scale(const glm::vec3& s) {
    myStack.push(glm::scale(glm::mat4(1.0f), s));
    updateCurrentMatrix();
}


void
GLTransformStack::scale(const double s) {
    scale(glm::vec3(s));
}


glm::vec4
GLTransformStack::applyTransform(glm::vec3& v) const {
    return (myCurrentTransform * glm::vec4(v, 1.));
}


void
GLTransformStack::updateCurrentMatrix() {
    myCurrentTransform = myStack.top() * myCurrentTransform;
}
