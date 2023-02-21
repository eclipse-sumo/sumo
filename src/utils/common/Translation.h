/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2003-2023 German Aerospace Center (DLR) and others.
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
/// @file    Translation.h
/// @author  Michael Behrisch
/// @date    2023-01-24
///
// Translation macros
/****************************************************************************/
#pragma once
#include <config.h>
#ifdef HAVE_INTL
#include <libintl.h>
#endif
#include "StringUtils.h"


// ===========================================================================
// global definitions
// ===========================================================================
#ifdef HAVE_INTL
static inline const char*
my_pgettext(const char* msg_ctxt_id, const char* msgid) {
    const char* translation = dcgettext(NULL, msg_ctxt_id, LC_MESSAGES);
    if (translation == msg_ctxt_id) {
        return msgid;
    }
    return translation;
}


#define TL(string) gettext(string)
#define TLC(context, string) my_pgettext(context "\004" string, string)
#define TLF(string, ...) StringUtils::format(gettext(string), __VA_ARGS__)
#else
#define TL(string) (string)
#define TLC(context, string) (string)
#define TLF(string, ...) StringUtils::format(string, __VA_ARGS__)
#endif
