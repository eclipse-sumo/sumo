/****************************************************************************/
/// @file    BinaryInputDevice.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Encapsulates binary reading operations on a file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <fstream>


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
    BinaryInputDevice(const std::string &name) throw();


    /// @brief Destructor
    ~BinaryInputDevice() throw();


    /** @brief Returns whether the file can be used (is good())
     *
     * @return Whether the file is good
     */
    bool good() const throw();


    /** @brief Reads an int from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the int from
     * @param[in] i The int to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, int &i) throw();


    /** @brief Reads an unsigned int from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the unsigned int from
     * @param[in] i The unsigned int to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, unsigned int &i) throw();


    /** @brief Reads a SUMOReal from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the SUMOReal from
     * @param[in] i The SUMOReal to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, SUMOReal &f) throw();


    /** @brief Reads a bool from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the bool from
     * @param[in] i The bool to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, bool &b) throw();


    /** @brief Reads a string from the file (input operator)
     *
     * Reads the length of the string as an unsigned int, first.
     *  Reads then the specified number of chars into "myBuffer".
     * Please note that the buffer has a fixed size - longer strings
     *  will cause an error.
     *
     * @param[in, out] os The BinaryInputDevice to read the string from
     * @param[in] i The string to store the read value into
     * @return The used BinaryInputDevice for further processing
     * @todo Use either a buffer with a flexible size or report an error if the buffer is too small!
     */
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, std::string &s) throw();


    /** @brief Reads a long from the file (input operator)
     *
     * @param[in, out] os The BinaryInputDevice to read the long from
     * @param[in] i The int to store the read value into
     * @return The used BinaryInputDevice for further processing
     */
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, long &l) throw();


private:
    /// @brief The encapsulated stream
    std::ifstream myStream;

    /// @brief The buffer used for string parsing
    char myBuffer[1000];

};


#endif

/****************************************************************************/

