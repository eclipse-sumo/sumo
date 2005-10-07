#ifndef FileHelpers_h
#define FileHelpers_h
/***************************************************************************
                          FileHelpers.h
			  Functions for an easier usage of files
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.7  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/01/26 07:24:19  dkrajzew
// added the possibility to check the format (xml/csv) of a file; added the possibility to compute the absoulte path from a base path and a relative path
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.7  2002/07/25 09:53:20  dkrajzew
// missing inclusion of fstream added
//
// Revision 1.6  2002/07/25 08:48:29  dkrajzew
// machine-dependent binary data reading added
//
// Revision 1.5  2002/07/11 07:42:58  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.4  2002/07/02 08:28:26  dkrajzew
// checkFileList - a method which checks whether the given list of files is not empty - added
//
// Revision 1.3  2002/06/11 14:38:21  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.1  2002/03/20 08:19:06  dkrajzew
// removeDir - method added
//
// Revision 2.0  2002/02/14 14:43:26  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:18  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fstream>
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class FileHelpers
 * A class holding some static functions for the easier usage of files.
 */
class FileHelpers {
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

    /** removes the directory information from the path */
    static std::string removeDir(const std::string &path);

    /** checks whether the list of files is not empty */
    static bool checkFileList(const std::string &files);

    /** removes the file information from the given path */
    static std::string removeFile(const std::string &path);

    /** returns the second path as a relative path to the first file */
    static std::string getConfigurationRelative(const std::string &configPath,
        const std::string &path);

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


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
