/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    StringUtils.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    unknown
///
// Some static methods for string processing
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <regex>
#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#else
#include <unistd.h>
#endif
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/TranscodingException.hpp>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include "StringUtils.h"


// ===========================================================================
// static member definitions
// ===========================================================================
std::string StringUtils::emptyString;
XERCES_CPP_NAMESPACE::XMLLCPTranscoder* StringUtils::myLCPTranscoder = nullptr;


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
StringUtils::pruneZeros(const std::string& str, int max) {
    const std::string::size_type endpos = str.find_last_not_of("0");
    if (endpos != std::string::npos && str.back() == '0') {
        std::string res = str.substr(0, MAX2((int)str.size() - max, (int)endpos + 1));
        return res;
    }
    return str;
}

std::string
StringUtils::to_lower_case(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](char c) {
        return (char)::tolower(c);
    });
    return s;
}


std::string
StringUtils::latin1_to_utf8(std::string str) {
    // inspired by http://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
    std::string result;
    for (const auto& c : str) {
        const unsigned char uc = (unsigned char)c;
        if (uc < 128) {
            result += uc;
        } else {
            result += (char)(0xc2 + (uc > 0xbf));
            result += (char)((uc & 0x3f) + 0x80);
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
StringUtils::replace(std::string str, const std::string& what, const std::string& by) {
    std::string::size_type idx = str.find(what);
    const int what_len = (int)what.length();
    if (what_len > 0) {
        const int by_len = (int)by.length();
        while (idx != std::string::npos) {
            str = str.replace(idx, what_len, by);
            idx = str.find(what, idx + by_len);
        }
    }
    return str;
}


std::string
StringUtils::substituteEnvironment(const std::string& str, const std::chrono::time_point<std::chrono::system_clock>* const timeRef) {
    std::string s = str;
    if (timeRef != nullptr) {
        const std::string::size_type localTimeIndex = str.find("${LOCALTIME}");
        const std::string::size_type utcIndex = str.find("${UTC}");
        const bool isUTC = utcIndex != std::string::npos;
        if (localTimeIndex != std::string::npos || isUTC) {
            const time_t rawtime = std::chrono::system_clock::to_time_t(*timeRef);
            char buffer [80];
            struct tm* timeinfo = isUTC ? gmtime(&rawtime) : localtime(&rawtime);
            strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S.", timeinfo);
            auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(*timeRef);
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(*timeRef - seconds);
            const std::string micro = buffer + toString(microseconds.count());
            if (isUTC) {
                s.replace(utcIndex, 6, micro);
            } else {
                s.replace(localTimeIndex, 12, micro);
            }
        }
    }
    const std::string::size_type pidIndex = str.find("${PID}");
    if (pidIndex != std::string::npos) {
#ifdef WIN32
        s.replace(pidIndex, 6, toString(::GetCurrentProcessId()));
#else
        s.replace(pidIndex, 6, toString(::getpid()));
#endif
    }
    if (std::getenv("SUMO_LOGO") == nullptr) {
        s = replace(s, "${SUMO_LOGO}", "${SUMO_HOME}/data/logo/sumo-128x138.png");
    }
    const std::string::size_type tildeIndex = str.find("~");
    if (tildeIndex == 0) {
        s.replace(0, 1, "${HOME}");
    }
    s = replace(s, ",~", ",${HOME}");
#ifdef WIN32
    if (std::getenv("HOME") == nullptr) {
        s = replace(s, "${HOME}", "${USERPROFILE}");
    }
#endif

    // Expression for an environment variables, e.g. ${NAME}
    // Note: - R"(...)" is a raw string literal syntax to simplify a regex declaration
    //       - .+? looks for the shortest match (non-greedy)
    //       - (.+?) defines a "subgroup" which is already stripped of the $ and {, }
    std::regex envVarExpr(R"(\$\{(.+?)\})");

    // Are there any variables in this string?
    std::smatch match;
    std::string strIter = s;

    // Loop over the entire value string and look for variable names
    while (std::regex_search(strIter, match, envVarExpr)) {
        std::string varName = match[1];

        // Find the variable in the environment and its value
        std::string varValue;
        if (std::getenv(varName.c_str()) != nullptr) {
            varValue = std::getenv(varName.c_str());
        }

        // Replace the variable placeholder with its value in the original string
        s = std::regex_replace(s, std::regex("\\$\\{" + varName + "\\}"), varValue);

        // Continue the loop with the remainder of the string
        strIter = match.suffix();
    }
    return s;
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
StringUtils::padFront(const std::string& str, int length, char padding) {
    return std::string(MAX2(0, length - (int)str.size()), padding) + str;
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
            throw NumberFormatException(str + " could not be interpreted as hex");
        }
    }
    return static_cast<unsigned char>(c);
}


int
StringUtils::toInt(const std::string& sData) {
    long long int result = toLong(sData);
    if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) {
        throw NumberFormatException(toString(result) + " int overflow");
    }
    return (int)result;
}


int
StringUtils::toIntSecure(const std::string& sData, int def) {
    if (sData.length() == 0) {
        return def;
    }
    return toInt(sData);
}


long long int
StringUtils::toLong(const std::string& sData) {
    const char* const data = sData.c_str();
    if (data == 0 || data[0] == 0) {
        throw EmptyData();
    }
    char* end;
    errno = 0;
#ifdef WIN32
    long long int ret = _strtoi64(data, &end, 10);
#else
    long long int ret = strtoll(data, &end, 10);
#endif
    if (errno == ERANGE) {
        errno = 0;
        throw NumberFormatException("(long long integer range) " + sData);
    }
    if ((int)(end - data) != (int)strlen(data)) {
        throw NumberFormatException("(long long integer format) " + sData);
    }
    return ret;
}


int
StringUtils::hexToInt(const std::string& sData) {
    if (sData.length() == 0) {
        throw EmptyData();
    }
    size_t idx = 0;
    int result;
    try {
        if (sData[0] == '#') { // for html color codes
            result = std::stoi(sData.substr(1), &idx, 16);
            idx++;
        } else {
            result = std::stoi(sData, &idx, 16);
        }
    } catch (...) {
        throw NumberFormatException("(hex integer format) " + sData);
    }
    if (idx != sData.length()) {
        throw NumberFormatException("(hex integer format) " + sData);
    }
    return result;
}


double
StringUtils::toDouble(const std::string& sData) {
    if (sData.size() == 0) {
        throw EmptyData();
    }
    try {
        size_t idx = 0;
        const double result = std::stod(sData, &idx);
        if (idx != sData.size()) {
            throw NumberFormatException("(double format) " + sData);
        } else {
            return result;
        }
    } catch (...) {
        // invalid_argument or out_of_range
        throw NumberFormatException("(double) " + sData);
    }
}


double
StringUtils::toDoubleSecure(const std::string& sData, const double def) {
    if (sData.length() == 0) {
        return def;
    }
    return toDouble(sData);
}


bool
StringUtils::toBool(const std::string& sData) {
    if (sData.length() == 0) {
        throw EmptyData();
    }
    const std::string s = to_lower_case(sData);
    if (s == "1" || s == "yes" || s == "true" || s == "on" || s == "x" || s == "t") {
        return true;
    }
    if (s == "0" || s == "no" || s == "false" || s == "off" || s == "-" || s == "f") {
        return false;
    }
    throw BoolFormatException(s);
}

MMVersion
StringUtils::toVersion(const std::string& sData) {
    std::vector<std::string> parts = StringTokenizer(sData, ".").getVector();
    return MMVersion(toInt(parts.front()), toDouble(parts.back()));
}

std::string
StringUtils::transcode(const XMLCh* const data, int length) {
    if (data == 0) {
        throw EmptyData();
    }
    if (length == 0) {
        return "";
    }
#if _XERCES_VERSION < 30100
    char* t = XERCES_CPP_NAMESPACE::XMLString::transcode(data);
    std::string result(t);
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
#else
    try {
        XERCES_CPP_NAMESPACE::TranscodeToStr utf8(data, "UTF-8");
        return reinterpret_cast<const char*>(utf8.str());
    } catch (XERCES_CPP_NAMESPACE::TranscodingException&) {
        return "?";
    }
#endif
}


std::string
StringUtils::transcodeFromLocal(const std::string& localString) {
#if _XERCES_VERSION > 30100
    try {
        if (myLCPTranscoder == nullptr) {
            myLCPTranscoder = XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgTransService->makeNewLCPTranscoder(XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);
        }
        if (myLCPTranscoder != nullptr) {
            return transcode(myLCPTranscoder->transcode(localString.c_str()));
        }
    } catch (XERCES_CPP_NAMESPACE::TranscodingException&) {}
#endif
    return localString;
}


std::string
StringUtils::transcodeToLocal(const std::string& utf8String) {
#if _XERCES_VERSION > 30100
    try {
        if (myLCPTranscoder == nullptr) {
            myLCPTranscoder = XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgTransService->makeNewLCPTranscoder(XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);
        }
        if (myLCPTranscoder != nullptr) {
            XERCES_CPP_NAMESPACE::TranscodeFromStr utf8(reinterpret_cast<const XMLByte*>(utf8String.c_str()), utf8String.size(), "UTF-8");
            return myLCPTranscoder->transcode(utf8.str());
        }
    } catch (XERCES_CPP_NAMESPACE::TranscodingException&) {}
#endif
    return utf8String;
}


std::string
StringUtils::trim_left(const std::string s, const std::string& t) {
    std::string result = s;
    result.erase(0, s.find_first_not_of(t));
    return result;
}

std::string
StringUtils::trim_right(const std::string s, const std::string& t) {
    std::string result = s;
    result.erase(s.find_last_not_of(t) + 1);
    return result;
}

std::string
StringUtils::trim(const std::string s, const std::string& t) {
    return trim_right(trim_left(s, t), t);
}

void
StringUtils::resetTranscoder() {
    myLCPTranscoder = nullptr;
}

/****************************************************************************/
