# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    vClassDialog.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...enums.attributesEnum import attrs
from ...frames.edit.boolAttribute import modifyBoolAttribute, modifyBoolAttributeOverlapped
from ...input.keyboard import typeKey, typeTwoKeys


def modifyVClassDialog_NoDisallowAll(attribute, vClass):
    """
    @brief modify vclass attribute using dialog
    """
    # open dialog
    modifyBoolAttribute(attribute)
    # go to vClass
    for _ in range(vClass):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # go to accept
    for _ in range(attrs.dialog.allowVClass.accept - vClass):
        typeKey('tab')
    typeKey('space')


def modifyVClassDialogOverlapped_NoDisallowAll(attribute, vClass):
    """
    @brief modify vclass attribute using dialog
    """
    # open dialog
    modifyBoolAttributeOverlapped(attribute)
    # go to vClass
    for _ in range(vClass):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # go to accept
    for _ in range(attrs.dialog.allowVClass.accept - vClass):
        typeKey('tab')
    typeKey('space')


def modifyVClassDialog_DisallowAll(attribute, vClass):
    """
    @brief modify vclass attribute using dialog (this disallow all other vClasses)
    """
    # open dialog
    modifyBoolAttribute(attribute)
    # disallow all
    for _ in range(attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    typeKey('space')
    # go to vClass
    for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # accept changes
    for _ in range(attrs.dialog.allowVClass.accept - vClass):
        typeKey('tab')
    typeKey('space')


def modifyVClassDialogOverlapped_DisallowAll(attribute, vClass):
    """
    @brief modify vclass attribute using dialog (this disallow all other vClasses)
    """
    # open dialog
    modifyBoolAttributeOverlapped(attribute)
    # disallow all
    for _ in range(attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    typeKey('space')
    # go to vClass
    for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # press accept
    for _ in range(attrs.dialog.allowVClass.accept - vClass):
        typeKey('tab')
    typeKey('space')


def modifyVClassDialog_Cancel(attribute, vClass):
    """
    @brief modify vclass attribute and cancel
    """
    # open dialog
    modifyBoolAttribute(attribute)
    # disallow all
    for _ in range(attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    typeKey('space')
    # go to vClass
    for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # cancel
    for _ in range(attrs.dialog.allowVClass.cancel - vClass):
        typeKey('tab')
    typeKey('space')


def modifyVClassDialogOverlapped_Cancel(attribute, vClass):
    """
    @brief modify vclass attribute and cancel (overlapped)
    """
    # open dialog
    modifyBoolAttributeOverlapped(attribute)
    # disallow all
    for _ in range(attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    typeKey('space')
    # go to vClass
    for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # cancel
    for _ in range(attrs.dialog.allowVClass.cancel - vClass):
        typeKey('tab')
    typeKey('space')


def modifyVClassDialog_Reset(attribute, vClass):
    """
    @brief modify vclass attribute and reset
    """
    # open dialog
    modifyBoolAttribute(attribute)
    # disallow all
    for _ in range(attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    typeKey('space')
    # go to vClass
    for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # reset
    for _ in range(attrs.dialog.allowVClass.reset - vClass):
        typeKey('tab')
    typeKey('space')
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    typeKey('space')


def modifyVClassDialogOverlapped_Reset(attribute, vClass):
    """
    @brief modify vclass attribute and reset (overlapped)
    """
    # open dialog
    modifyBoolAttributeOverlapped(attribute)
    # disallow all
    for _ in range(attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    typeKey('space')
    # go to vClass
    for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # reset
    for _ in range(attrs.dialog.allowVClass.reset - vClass):
        typeKey('tab')
    typeKey('space')
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    typeKey('space')
