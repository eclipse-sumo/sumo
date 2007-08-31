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
 * A class holding some static functions for the easier usage of files.
 */
class FileHelpers
{
public:
    /**
     * enum FileType
     * A set of known filetypes
     */
    enum FileType {
        /// no file at all...
        INVALID,
        /// XML (eXtensible Markup Language) - derivates
        XML,
        /// CSV (Comma Separated Values) - derivates
        CSV
    };

public:
    /** checks whether the given file exists */
    static bool exists(std::string path);

    /** removes the file information from the given path */
    static std::string removeFile(const std::string &path);

    /** returns the second path as a relative path to the first file */
    static std::string getConfigurationRelative(const std::string &configPath,
            const std::string &path);

    /** returns the information whether the given name represents a socket */
    static bool isSocket(const std::string &name);

    /** returns the information whether the given path is absolute */
    static bool isAbsolute(const std::string &path);

    /** reads an integer from the given stream regarding the byte order */
    static int readInt(std::istream &strm, bool intelFile=true);

    /** reads an integer from the given stream regarding the byte order */
    static unsigned int readUInt(std::istream &strm, bool intelFile=true);

    /** reads a SUMOReal */
    static SUMOReal readFloat(std::istream &strm, bool intelFile=true);

    /** reads a byte */
    static unsigned char readByte(std::istream &strm);

    /** reads a string */
    static std::string readString(std::istream &strm, bool intelFile=true);

    static std::ostream &writeInt(std::ostream &strm, int value);
    static std::ostream &writeUInt(std::ostream &strm, unsigned int value);
    static std::ostream &writeFloat(std::ostream &strm, SUMOReal value);
    static std::ostream &writeByte(std::ostream &strm, unsigned char value);
    static std::ostream &writeString(std::ostream &strm, const std::string &value);

    /// Checks whether the given file is a xml or csv file
    static FileType checkFileType(const std::string &filename);

    /// Check for relativity
    static std::string checkForRelativity(std::string filename,
                                          const std::string &basePath);


};


#endif

/****************************************************************************/

