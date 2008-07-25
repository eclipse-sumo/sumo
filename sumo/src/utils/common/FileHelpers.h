/****************************************************************************/
/// @file    FileHelpers.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Functions for an easier usage of files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef FileHelpers_h
#define FileHelpers_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FileHelpers
 * @brief Functions for an easier usage of files and paths
 */
class FileHelpers
{
public:
    /// @name file access functions
    //@{

    /** @brief Checks whether the given file exists
     *
     * @param[in] path The path to the file that shall be examined
     * @return Whether the named file exists
     */
    static bool exists(std::string path);
    //@}



    /// @name file path evaluating functions
    //@{

    /** @brief Removes the file information from the given path
     *
     * @param[in] path The path to the file to return the folder it is located in
     * @return The directory of the named file
     */
    static std::string getFilePath(const std::string &path);


    /** @brief Returns the second path as a relative path to the first file
     *
     * Given the position of the configuration file, and the information where a second
     *  file is relative to the configuration file's position, we want to known where
     *  this second file can be found. This method gets the path to the configuration file
     *  (including the configuration file name) and the path to get the relative position
     *  of and returns this relative position.
     *
     * @param[in] configPath The path the configuration file (including the config's file name)
     * @param[in] path The path to the references file (relativ to configuration path)
     * @return The file's position (relative to curent working directory)
     */
    static std::string getConfigurationRelative(const std::string &configPath,
            const std::string &path);


    /** @brief Returns the information whether the given name represents a socket
     *
     * A file name is meant to describe a socket address if a colon is found at a position
     *  larger than one.
     *
     * @param[in] name The name of a file
     * @return Whether the name names a socket
     */
    static bool isSocket(const std::string &name);


    /** @brief Returns the information whether the given path is absolute
     *
     * A path is meant to be absolute, if
     * @arg it is a socket
     * @arg it starts with a "/" (Linux)
     * @arg it has a ':' at the second position (Windows)
     *
     * @param[in] path The path to examine
     * @return Whether the path is absolute
     */
    static bool isAbsolute(const std::string &path);


    /** @brief Returns the path from a configuration so that it is accessable from the current working directory
     *
     * If the path is absolute, it is returned. Otherwise, the file's position
     *  is computed regarding the configuration path (see getConfigurationRelative).
     *
     * @see isAbsolute
     * @see getConfigurationRelative
     * @param[in] filename The path to the file to be examined
     * @param[in] basePath The path the configuration file (including the config's file name)
     * @return The file's position
     */
    static std::string checkForRelativity(std::string filename,
                                          const std::string &basePath);
    //@}



    /// @name binary reading/writing functions
    //@{

    /** @brief Writes an integer binary
     *
     * Issues:
     * @arg An "int" is always assumed to be 4 bytes long
     *
     * @param[in, out] strm The stream to write into
     * @param[in] value The integer to write
     * @return Reference to the stream
     */
    static std::ostream &writeInt(std::ostream &strm, int value);


    /** @brief Writes an unsigned integer binary
     *
     * Issues:
     * @arg An "unsigned int" is always assumed to be 4 bytes long
     *
     * @param[in, out] strm The stream to write into
     * @param[in] value The unsigned integer to write
     * @return Reference to the stream
     */
    static std::ostream &writeUInt(std::ostream &strm, unsigned int value);


    /** @brief Writes a float binary
     *
     * Issues:
     * @arg A "float" is always assumed to be 4 bytes long
     *
     * @param[in, out] strm The stream to write into
     * @param[in] value The float to write
     * @return Reference to the stream
     */
    static std::ostream &writeFloat(std::ostream &strm, SUMOReal value);


    /** @brief Writes a byte binary
     *
     * @param[in, out] strm The stream to write into
     * @param[in] value The byte to write
     * @return Reference to the stream
     */
    static std::ostream &writeByte(std::ostream &strm, unsigned char value);


    /** @brief Writes a string binary
     *
     * Writes the length of the string, first, using writeInt. Writes then the string's
     *  characters.
     *
     * @see writeInt
     * @param[in, out] strm The stream to write into
     * @param[in] value The string to write
     * @return Reference to the stream
     */
    static std::ostream &writeString(std::ostream &strm, const std::string &value);
    //@}

};


#endif

/****************************************************************************/

