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

# @file    translation.py
# @author  Mirko Barthauer
# @date    2023-05-10

from __future__ import print_function
import gettext
import os

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LOCALEDIR = os.path.join(SUMO_HOME, 'data', 'locale')
CURRENT_LANG = None


def addLanguageOption(parser):
    parser.add_option("--language", type=str, default="en", help="Defines the language of the GUI")


def setLanguage(lang):
    global CURRENT_LANG
    try:
        translation = gettext.translation("sumo", localedir=LOCALEDIR, languages=[lang])
        translation.install()
        CURRENT_LANG = translation
    except:  # noqa
        CURRENT_LANG = None


def TL(msgid):
    if CURRENT_LANG is None:
        return msgid
    return CURRENT_LANG.gettext(msgid)


def TLF(msgid, *args):
    msg = TL(msgid)
    varCount = min(msg.count('%'), len(args))
    for val in args[:varCount]:
        msg = msg.replace('%', str(val), 1)
    return msg


def TLC(context, msgid):
    if CURRENT_LANG is None:
        return msgid
    return CURRENT_LANG.pgettext(context, msgid)
