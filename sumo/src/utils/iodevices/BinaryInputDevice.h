/****************************************************************************/
/// @file    BinaryInputDevice.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2005-09-15
/// @version $Id$
///
// Encapsulates binary reading operations on a file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef BinaryInputDevice_h
#define BinaryInputDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fstream>
#include "BinaryFormatter.h"


// ===========================================================================
// class declarations
// ===========================================================================
class BinaryInputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class BinaryInputDevice
 * @brief Encapsulates binary reading operations on a file
 *
 * This class opens a binary file stream for reading and offers read access
 *  functions on it.
 *
 * Please note that the byte order is undefined. Also the length of each
 *  type is not defined on a global scale and may differ across compilers or
 *  platforms.
 *
 * @todo Recheck whether this class could be replaced by a "normal InputDevice"
 * @todo Maybe some kind of a well-defined types should be used (so that they have a well-defined length)
 * @todo Maybe the byte-order should be defined
 */
class BinaryInputDevice {
public:
    /** @brief Constructor
     *
     * @param[in] name The name of the file to open for reading
     */
    BinaryInputDevice(const std::string& name, const bool isTyped = false, const bool doValidate = false);


    /// @brief Destructor
    ~BinaryInputDevice();


    /** @brief Returns whether the underlying file stream can be used (is good())
     *
     * @return Whether the file is good
     */
    bool good() const;


    /** @brief Returns the next character to be read by an actual parse.
     *
     * @return the next character which will be returned
     */
    int peek();


    /** @brief Reads the defined number of bytes and returns them as a string.
     *
     * @param[in] numBytes The number of bytes to read
     * @return the string read
     */
    std::string read(int numBytes);


    /** @brief Pushes a character back into the stream to be read by the next actual parse.
     *
     * @param[in] c The char to store in the stream
     */
    void putback(char c);


    /** @brief Reads a char from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the char from
     * @param[in] c The char to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, char& c);


    /** @brief Reads an unsigned char from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the char from
     * @param[in] c The char to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, unsigned char& c);


    /** @brief Reads an int from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the int from
     * @param[in] i The int to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, int& i);


    /** @brief Reads a SUMOReal from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the SUMOReal from
     * @param[in] i The SUMOReal to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, SUMOReal& f);


    /** @brief Reads a bool from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the bool from
     * @param[in] i The bool to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, bool& b);


    /** @brief Reads a string from the file (input operator)
     *
     * Reads the length of the string as an int, first.
     *  Reads then the specified number of chars into "myBuffer".
     * Please note that the buffer has a fixed size - longer strings
     *  will cause an error.
     *
     * @param[in, out] os The BinaryInputDevice to read the string from
     * @param[in] s The string to store the read value into
     * @return The used BinaryInputDevice for further processing
     * @todo Use either a buffer with a flexible size or report an error if the buffer is too small!
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, std::string& s);


    /** @brief Reads a string vector from the file (input operator)
     *
     * Reads the length of the vector as an int, first.
     *  Reads then the specified number of strings using the string input operator.
     * Please note that the buffer has a fixed size - longer strings
     *  will cause an error.
     *
     * @param[in, out] os The BinaryInputDevice to read the string from
     * @param[in] v The string vector to store the read value into
     * @return The used BinaryInputDevice for further processing
     * @todo Use either a buffer with a flexible size or report an error if the buffer is too small!
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, std::vector<std::string>& v);


    /** @brief Reads a string vector from the file (input operator)
     *
     * Reads the length of the vector as an int, first.
     *  Reads then the specified number of strings using the string input operator.
     * Please note that the buffer has a fixed size - longer strings
     *  will cause an error.
     *
     * @param[in, out] os The BinaryInputDevice to read the string from
     * @param[in] v The string vector to store the read value into
     * @return The used BinaryInputDevice for further processing
     * @todo Use either a buffer with a flexible size or report an error if the buffer is too small!
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, std::vector<int>& v);


    /** @brief Reads a string vector from the file (input operator)
     *
     * Reads the length of the vector as an int, first.
     *  Reads then the specified number of strings using the string input operator.
     * Please note that the buffer has a fixed size - longer strings
     *  will cause an error.
     *
     * @param[in, out] os The BinaryInputDevice to read the string from
     * @param[in] v The string vector to store the read value into
     * @return The used BinaryInputDevice for further processing
     * @todo Use either a buffer with a flexible size or report an error if the buffer is too small!
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, std::vector< std::vector<int> >& v);


    /** @brief Reads a Position from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the Position from
     * @param[in] p The Position to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice& operator>>(BinaryInputDevice& os, Position& p);

private:
    int checkType(BinaryFormatter::DataType t);

private:
    /// @brief The encapsulated stream
    std::ifstream myStream;

    const bool myAmTyped;

    /// @brief Information whether types shall be checked
    const bool myEnableValidation;

    /// @brief The buffer used for string parsing
    char myBuffer[10000];

};


#endif

/****************************************************************************/

