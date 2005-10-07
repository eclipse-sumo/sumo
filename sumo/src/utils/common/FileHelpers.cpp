/***************************************************************************
                          FileHelpers.cpp
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
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
// initial commit for some utility classes common to most propgrams of the
//  sumo-package
//
// Revision 1.6  2002/07/25 08:48:29  dkrajzew
// machine-dependent binary data reading added
//
// Revision 1.5  2002/07/11 07:42:58  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.4  2002/07/02 08:28:26  dkrajzew
// checkFileList - a method which checks whether the given list of files is not
//  empty - added
//
// Revision 1.3  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/11 13:43:35  dkrajzew
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

#include <string>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include "FileHelpers.h"
#include "StringTokenizer.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;



/* =========================================================================
 * constants definitions
 * ======================================================================= */
#define BUF_MAX 1000


/* =========================================================================
 * static member definitions
 * ======================================================================= */
char gBuf[BUF_MAX];


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
FileHelpers::exists(string path)
{
    // !!! not really well
    while(path.at(path.length()-1)=='/'||path.at(path.length()-1)=='\\')
        path.erase(path.end()-1);
    if(path.length()==0) return false;
    struct stat st;
    bool ret = ( stat( path.c_str(), &st ) == 0 );
    return ret;
}


std::string
FileHelpers::removeDir(const std::string &path)
{
    size_t beg = path.find_last_of("\\/");
    if(beg==string::npos||beg==0)
        return path;
    return path.substr(beg);
}


bool
FileHelpers::checkFileList(const std::string &files)
{
    StringTokenizer st(files, ';');
    return st.size()!=0;
}


std::string
FileHelpers::removeFile(const std::string &path)
{
    size_t beg = path.find_last_of("\\/");
    if(beg==string::npos||beg==0)
        return "";
    return path.substr(0, beg+1);
}


std::string
FileHelpers::getConfigurationRelative(const std::string &configPath,
                                      const std::string &path)
{
    string retPath = removeFile(configPath);
    return retPath + path;
}


bool
FileHelpers::isAbsolute(const std::string &path)
{
    // check UNIX - absolute paths
    if(path.length()>0&&path[0]=='/') {
        return true;
    }
    // check Windows - absolute paths
    if(path.length()>1&&path[1]==':') {
        return true;
    }
    return false;
}


int
FileHelpers::readInt(std::istream &strm, bool intelFile)
{
    unsigned char buf[4];
    strm.read((char*) &buf, sizeof(char)*4);
#ifdef SUN
    if(!intelFile) {
        return (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|(buf[0]);
    } else {
        return (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
    }
#else
    if(!intelFile) {
        return (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
    } else {
        return (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|(buf[0]);
    }
#endif
}


unsigned int
FileHelpers::readUInt(std::istream &strm, bool intelFile)
{
    unsigned char buf[4];
    strm.read((char*) &buf, sizeof(char)*4);
#ifdef SUN
    if(!intelFile) {
        return (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|(buf[0]);
    } else {
        return (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
    }
#else
    if(!intelFile) {
        return (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
    } else {
        return (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|(buf[0]);
    }
#endif
}


SUMOReal
FileHelpers::readFloat(std::istream &strm, bool intelFile)
{
    SUMOReal flt;
    strm.read((char*) &flt, sizeof(char)*4);
#ifdef SUN
    if(!intelFile) {
        SUMOReal flt2;
        ((char*) &flt2)[0] = ((char*) &flt)[3];
        ((char*) &flt2)[1] = ((char*) &flt)[2];
        ((char*) &flt2)[2] = ((char*) &flt)[1];
        ((char*) &flt2)[3] = ((char*) &flt)[0];
        return flt2;
    } else {
        return flt;
    }
#else
    if(!intelFile) {
        return flt;
    } else {
        return flt;
        /*
        ((char*) &flt2)[0] = ((char*) &flt)[3];
        ((char*) &flt2)[1] = ((char*) &flt)[2];
        ((char*) &flt2)[2] = ((char*) &flt)[1];
        ((char*) &flt2)[3] = ((char*) &flt)[0];
        return flt2;
        */
    }
#endif
}

unsigned char
FileHelpers::readByte(std::istream &strm)
{
    unsigned char chr;
    strm >> chr;
    return chr;
}


std::string
FileHelpers::readString(std::istream &strm, bool intelFile)
{
    int size = readInt(strm, intelFile);
    if(size<BUF_MAX) {
        strm.read((char*) &gBuf, sizeof(char)*size);
        gBuf[size] = 0;
        return std::string(gBuf);
    }
    throw 1;
}


std::ostream &
FileHelpers::writeInt(std::ostream &strm, int value)
{
    strm.write((char*) &value, sizeof(char)*4);
    return strm;
}


std::ostream &
FileHelpers::writeUInt(std::ostream &strm, unsigned int value)
{
    strm.write((char*) &value, sizeof(char)*4);
    return strm;
}


std::ostream &
FileHelpers::writeFloat(std::ostream &strm, SUMOReal value)
{
    strm.write((char*) &value, sizeof(char)*4);
    return strm;
}


std::ostream &
FileHelpers::writeByte(std::ostream &strm, unsigned char value)
{
    strm.write((char*) &value, sizeof(char)*1);
    return strm;
}


std::ostream &
FileHelpers::writeString(std::ostream &strm, const std::string &value)
{
    int size = value.length();
    const char *cstr = value.c_str();
    writeUInt(strm, size);
    strm.write((char*) cstr, sizeof(char)*size);
    return strm;
}


FileHelpers::FileType
FileHelpers::checkFileType(const std::string &filename)
{
    ifstream strm(filename.c_str());
    if(!strm.good()) {
        return INVALID;
    }
    string l;
    strm >> l;
    if(l[0]=='<') {
        return XML;
    } else {
        return CSV;
    }
}


std::string
FileHelpers::checkForRelativity(std::string filename,
                                const std::string &basePath)
{
     if(!isAbsolute(filename)) {
         filename = getConfigurationRelative(basePath, filename);
     }
     return filename;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
