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
// Revision 1.4  2002/07/31 17:30:05  roessel
// Changes since sourceforge cvs request.
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * FileHelpers
 * A class holding some static functions for the easier usage of files.
 */
class FileHelpers {
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
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "FileHelpers.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
