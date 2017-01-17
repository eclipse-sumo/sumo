/****************************************************************************/
/// @file    StringUtils.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    unknown
/// @version $Id$
///
// Some static methods for string processing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <cstdio>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include "StringUtils.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
std::string StringUtils::emptyString;


// ===========================================================================
// method definitions
// ===========================================================================
std::string
StringUtils::prune(const std::string& str) {
    const std::string::size_type endpos = str.find_last_not_of(" \t\n\r");
    if (std::string::npos != endpos) {
        const int startpos = (int)str.find_first_not_of(" \t\n\r");
        return str.substr(startpos, endpos - startpos + 1);
    }
    return "";
}


std::string
StringUtils::to_lower_case(std::string str) {
    for (int i = 0; i < (int)str.length(); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 'a' - 'A';
        }
    }
    return str;
}


std::string
StringUtils::latin1_to_utf8(std::string str) {
    // inspired by http://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
    std::string result;
    for (int i = 0; i < (int)str.length(); i++) {
        const unsigned char c = str[i];
        if (c < 128) {
            result += c;
        } else {
            result += (char)(0xc2 + (c > 0xbf));
            result += (char)((c & 0x3f) + 0x80);
        }
    }
    return result;
}


std::string
StringUtils::convertUmlaute(std::string str) {
    str = replace(str, "\xE4", "ae");
    str = replace(str, "\xC4", "Ae");
    str = replace(str, "\xF6", "oe");
    str = replace(str, "\xD6", "Oe");
    str = replace(str, "\xFC", "ue");
    str = replace(str, "\xDC", "Ue");
    str = replace(str, "\xDF", "ss");
    str = replace(str, "\xC9", "E");
    str = replace(str, "\xE9", "e");
    str = replace(str, "\xC8", "E");
    str = replace(str, "\xE8", "e");
    return str;
}



std::string
StringUtils::replace(std::string str, const char* what,
                     const char* by) {
    const std::string what_tmp(what);
    const std::string by_tmp(by);
    std::string::size_type idx = str.find(what);
    const int what_len = (int)what_tmp.length();
    if (what_len > 0) {
        const int by_len = (int)by_tmp.length();
        while (idx != std::string::npos) {
            str = str.replace(idx, what_len, by);
            idx = str.find(what, idx + by_len);
        }
    }
    return str;
}


std::string
StringUtils::toTimeString(int time) {
    std::ostringstream oss;
    if (time < 0) {
        oss << "-";
        time = -time;
    }
    char buffer[10];
    sprintf(buffer, "%02i:", (time / 3600));
    oss << buffer;
    time = time % 3600;
    sprintf(buffer, "%02i:", (time / 60));
    oss << buffer;
    time = time % 60;
    sprintf(buffer, "%02i", time);
    oss << buffer;
    return oss.str();
}


bool
StringUtils::startsWith(const std::string& str, const std::string prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}


bool
StringUtils::endsWith(const std::string& str, const std::string suffix) {
    if (str.length() >= suffix.length()) {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    } else {
        return false;
    }
}


std::string
StringUtils::escapeXML(const std::string& orig, const bool maskDoubleHyphen) {
    std::string result = replace(orig, "&", "&amp;");
    result = replace(result, ">", "&gt;");
    result = replace(result, "<", "&lt;");
    result = replace(result, "\"", "&quot;");
    if (maskDoubleHyphen) {
        result = replace(result, "--", "&#45;&#45;");
    }
    for (char invalid = '\1'; invalid < ' '; invalid++) {
        result = replace(result, std::string(1, invalid).c_str(), "");
    }
    return replace(result, "'", "&apos;");
}


std::string
StringUtils::urlEncode(const std::string& toEncode, const std::string encodeWhich) {
    std::ostringstream out;

    for (int i = 0; i < (int)toEncode.length(); ++i) {
        const char t = toEncode.at(i);

        if ((encodeWhich != "" && encodeWhich.find(t) == std::string::npos) ||
                (encodeWhich == "" &&
                 ((t >= 45 && t <= 57) ||       // hyphen, period, slash, 0-9
                  (t >= 65 && t <= 90) ||        // A-Z
                  t == 95 ||                     // underscore
                  (t >= 97 && t <= 122) ||       // a-z
                  t == 126))                     // tilde
           ) {
            out << toEncode.at(i);
        } else {
            out << charToHex(toEncode.at(i));
        }
    }

    return out.str();
}

std::string
StringUtils::urlDecode(const std::string& toDecode) {
    std::ostringstream out;

    for (int i = 0; i < (int)toDecode.length(); ++i) {
        if (toDecode.at(i) == '%') {
            std::string str(toDecode.substr(i + 1, 2));
            out << hexToChar(str);
            i += 2;
        } else {
            out << toDecode.at(i);
        }
    }

    return out.str();
}

std::string
StringUtils::charToHex(unsigned char c) {
    short i = c;

    std::stringstream s;

    s << "%" << std::setw(2) << std::setfill('0') << std::hex << i;

    return s.str();
}

unsigned char
StringUtils::hexToChar(const std::string& str) {
    short c = 0;

    if (!str.empty()) {
        std::istringstream in(str);

        in >> std::hex >> c;

        if (in.fail()) {
            throw std::runtime_error("stream decode failure");
        }
    }

    return static_cast<unsigned char>(c);
}


/****************************************************************************/
