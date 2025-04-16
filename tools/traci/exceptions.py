# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    exceptions.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09

from __future__ import print_function
from __future__ import absolute_import
import functools
import warnings


def deprecated(new_name=None):
    """This is a decorator which can be used to mark functions
    as deprecated. It will result in a warning being emitted
    when the function is used."""
    def Inner(func):
        @functools.wraps(func)
        def new_func(*args, **kwargs):
            if new_name is None:
                msg = "Call to deprecated function %s." % (func.__name__)
            else:
                msg = "Call to deprecated function %s, use %s instead." % (func.__name__, new_name)
            warnings.warn(msg, stacklevel=2)
            return func(*args, **kwargs)
        return new_func
    return Inner


def alias_param(param, alias, deprecate=True):
    """
    Decorator for aliasing a param in a function
    """
    if isinstance(param, str):
        subst = [(param, alias)]
    else:
        subst = list(zip(param, alias))

    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            for par, ali in subst:
                if ali in kwargs:
                    kwargs[par] = kwargs[ali]
                    del kwargs[ali]
                    if deprecate:
                        warnings.warn("Use of deprecated parameter %s in function %s, use %s instead." %
                                      (ali, func.__name__, par), stacklevel=2)
            return func(*args, **kwargs)
        return wrapper
    return decorator


class TraCIException(Exception):

    """Exception class for all TraCI errors which keep the connection intact"""

    def __init__(self, desc, command=None, errorType=None):
        Exception.__init__(self, desc)
        self._command = command
        self._type = errorType

    def getCommand(self):
        return self._command

    def getType(self):
        return self._type


class FatalTraCIError(Exception):

    """Exception class for all TraCI errors which do not allow for continuation"""

    def __init__(self, desc):
        Exception.__init__(self, desc)
