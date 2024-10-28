/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    StringUtils.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    unknown
///
// Some static methods for string processing
/****************************************************************************/
#pragma once
#include <config.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <xercesc/util/XMLString.hpp>
#include <utils/common/StdDefs.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class StringUtils
 * @brief Some static methods for string processing
 */
class StringUtils {

public:

    /// @brief Removes trailing and leading whitechars
    static std::string prune(const std::string& str);

    /// @brief Removes trailing zeros (at most 'max')
    static std::string pruneZeros(const std::string& str, int max);

    /// @brief Transfers the content to lower case
    static std::string to_lower_case(const std::string& str);

    /// @brief Transfers from Latin 1 (ISO-8859-1) to UTF-8
    static std::string latin1_to_utf8(std::string str);

    /// @brief Converts german "Umlaute" to their latin-version
    static std::string convertUmlaute(std::string str);

    /// @brief Replaces all occurrences of the second string by the third string within the first string
    static std::string replace(std::string str, const std::string& what, const std::string& by);

    /// @brief Replaces an environment variable with its value (similar to bash); syntax for a variable is ${NAME}
    static std::string substituteEnvironment(const std::string& str, const std::chrono::time_point<std::chrono::system_clock>* const timeRef = nullptr);

    ///@brief  Checks whether a given string starts with the prefix
    static bool startsWith(const std::string& str, const std::string prefix);

    /// @brief Checks whether a given string ends with the suffix
    static bool endsWith(const std::string& str, const std::string suffix);

    //// @brief pads the given string with padding character up to the given total length
    static std::string padFront(const std::string& str, int length, char padding);

    /**
     * @brief Replaces the standard escapes by their XML entities.
     *
     * The strings &, <, >, ", and ' are replaced by &amp;, &lt;, &gt;, &quot;, and &apos;
     *
     * @param[in] orig The original string
     * @param[in] maskDoubleHyphen Whether -- in input shall be converted to &#45;&#45; (semantically equivalent but allowed in XML comments)
     * @return the string with the escaped sequences
     */
    static std::string escapeXML(const std::string& orig, const bool maskDoubleHyphen = false);

    /// @brief An empty string
    static std::string emptyString;

    /// @brief encode url (stem from http://bogomip.net/blog/cpp-url-encoding-and-decoding/)
    static std::string urlEncode(const std::string& url, const std::string encodeWhich = "");

    /// @brief decode url (stem from http://bogomip.net/blog/cpp-url-encoding-and-decoding/)
    static std::string urlDecode(const std::string& encoded);

    /// @brief char to hexadecimal
    static std::string charToHex(unsigned char c);

    /// @brief hexadecimal to char
    static unsigned char hexToChar(const std::string& str);

    /**@brief converts a string into the integer value described by it by calling the char-type converter, which
     * @throw an EmptyData - exception if the given string is empty
     * @throw NumberFormatException - exception when the string does not contain an integer
     */
    static int toInt(const std::string& sData);

    /// @brief converts a string into the integer value described by it
    /// @return the default value if the data is empty
    static int toIntSecure(const std::string& sData, int def);

    /**@brief converts a string into the long value described by it by calling the char-type converter, which
     * @throw an EmptyData - exception if the given string is empty
     * @throw NumberFormatException - exception when the string does not contain a long integer
     */
    static long long int toLong(const std::string& sData);

    /**@brief converts a string with a hex value into the integer value described by it by calling the char-type converter
     * @throw an EmptyData - exception if the given string is empty
     * @throw a NumberFormatException - exception when the string does not contain an integer
     */
    static int hexToInt(const std::string& sData);

    /**@brief converts a string into the double value described by it by calling the char-type converter
     * @throw an EmptyData - exception if the given string is empty
     * @throw a NumberFormatException - exception when the string does not contain a double
     */
    static double toDouble(const std::string& sData);

    /// @brief converts a string into the integer value described by it
    /// @return the default value if the data is empty
    static double toDoubleSecure(const std::string& sData, const double def);

    /**@brief converts a string into the bool value described by it by calling the char-type converter
     * @return true if the sData is one of the following (case insensitive): '1', 'x', 'true', 'yes', 'on', 't'
     * @return false if the sData is one of the following (case insensitive): '0', '-', 'false', 'no', 'off', 'f'
     * @throw EmptyData - exception if the given string is empty
     * @throw BoolFormatException in any other case
     */
    static bool toBool(const std::string& sData);

    /// @brief parse a (network) version string
    static MMVersion toVersion(const std::string& sData);

    /// @brief parse a distance, length or width value with a unit
    static double parseDist(const std::string& sData);

    /// @brief parse a speed value with a unit
    static double parseSpeed(const std::string& sData, const bool defaultKmph = true);

    /**@brief converts a 0-terminated XMLCh* array (usually UTF-16, stemming from Xerces) into std::string in UTF-8
     * @throw an EmptyData - exception if the given pointer is 0
     */
    static inline std::string transcode(const XMLCh* const data) {
        return transcode(data, (int)XERCES_CPP_NAMESPACE::XMLString::stringLen(data));
    }

    /**@brief converts a 0-terminated XMLCh* array (usually UTF-16, stemming from Xerces) into std::string in UTF-8 considering the given length
     * @throw EmptyData if the given pointer is 0
     */
    static std::string transcode(const XMLCh* const data, int length);

    /// @brief convert a string from the local codepage to UTF-8
    static std::string transcodeFromLocal(const std::string& localString);

    /// @brief convert a string from UTF-8 to the local codepage
    static std::string transcodeToLocal(const std::string& utf8String);

    /// @brief remove leading whitespace from string
    static std::string trim_left(const std::string s, const std::string& t = " \t\n");

    /// @brief remove trailing whitespace from string
    static std::string trim_right(const std::string s, const std::string& t = " \t\n");

    /// @brief remove leading and trailing whitespace
    static std::string trim(const std::string s, const std::string& t = " \t\n");

    /// @brief remove leading and trailing whitespace
    static std::string wrapText(const std::string s, int width);

    /// @brief must be called when shutting down the xml subsystem
    static void resetTranscoder();

    /// @brief adds a new formatted message
    // variadic function
    template<typename T, typename... Targs>
    static const std::string format(const std::string& format, T value, Targs... Fargs) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(gPrecision);
        _format(format.c_str(), os, value, Fargs...);
        return os.str();
    }

private:
    static void _format(const char* format, std::ostringstream& os) {
        os << format;
    }

    /// @brief adds a new formatted message
    // variadic function
    template<typename T, typename... Targs>
    static void _format(const char* format, std::ostringstream& os, T value, Targs... Fargs) {
        for (; *format != '\0'; format++) {
            if (*format == '%') {
                os << value;
                _format(format + 1, os, Fargs...); // recursive call
                return;
            }
            os << *format;
        }
    }

    static XERCES_CPP_NAMESPACE::XMLLCPTranscoder* myLCPTranscoder;
};
