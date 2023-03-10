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
/// @file    FileHelpers.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
///
// Functions for an easier usage of files
/****************************************************************************/
#include <config.h>

#include <string>
#ifdef WIN32
// this is how fox does it in xincs.h
#include <io.h>
#define access _access
#define R_OK    4       /* Test for read permission.  */
#include <direct.h>
#define getcwd _getcwd // stupid MSFT "deprecation" warning
#else
#include <unistd.h>
#endif
#include <fstream>
#include <sys/stat.h>
#include "FileHelpers.h"
#include "StringTokenizer.h"
#include "StringUtils.h"
#include "MsgHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// file access functions
// ---------------------------------------------------------------------------

bool
FileHelpers::isReadable(std::string path) {
    if (path.length() == 0) {
        return false;
    }
    while (path[path.length() - 1] == '/' || path[path.length() - 1] == '\\') {
        path.erase(path.end() - 1);
    }
    if (path.length() == 0) {
        return false;
    }
    return access(StringUtils::transcodeToLocal(path).c_str(), R_OK) == 0;
}

bool
FileHelpers::isDirectory(std::string path) {
#ifdef _MSC_VER
    struct _stat64 fileInfo;
    if (_stat64(StringUtils::transcodeToLocal(path).c_str(), &fileInfo) != 0) {
#else
    struct stat fileInfo;
    if (stat(StringUtils::transcodeToLocal(path).c_str(), &fileInfo) != 0) {
#endif
        throw ProcessError(TLF("Cannot get file attributes for file '%'!", path));
    }
    return (fileInfo.st_mode & S_IFMT) == S_IFDIR;
}

// ---------------------------------------------------------------------------
// file path evaluating functions
// ---------------------------------------------------------------------------

std::string
FileHelpers::getFilePath(const std::string& path) {
    const auto beg = path.find_last_of("\\/");
    if (beg == std::string::npos) {
        return "";
    }
    return path.substr(0, beg + 1);
}


std::string
FileHelpers::getFileFromPath(std::string path, const bool removeExtension) {
    // first remove extension
    if (removeExtension) {
        const auto begExtension = path.find_last_of(".");
        if (begExtension != std::string::npos) {
            path = path.substr(0, begExtension);
        }
    }
    // now remove path
    const auto begPath = path.find_last_of("\\/");
    if (begPath != std::string::npos) {
        path = path.substr(begPath + 1, path.size());
    }
    return path;
}


std::string
FileHelpers::addExtension(const std::string& path, const std::string& extension) {
    if (path.empty()) {
        return "";
    } else if (extension.empty()) {
        return path;
    } else if (path == extension) {
        return "";
    } else if (path.size() < extension.size()) {
        return path + extension;
    } else {
        // declare two reverse iterator for every string
        std::string::const_reverse_iterator it_path = path.rbegin();
        std::string::const_reverse_iterator it_extension = extension.rbegin();
        // iterate over extension and compare both characters
        while (it_extension != extension.rend()) {
            // if both characters are different, then return path + extension
            if (*it_path != *it_extension) {
                return path + extension;
            }
            it_path++;
            it_extension++;
        }
        // if comparison was successful, then the path has already the extension
        return path;
    }
}


std::string
FileHelpers::getConfigurationRelative(const std::string& configPath, const std::string& path) {
    std::string retPath = getFilePath(configPath);
    return retPath + path;
}


bool
FileHelpers::isSocket(const std::string& name) {
    const std::string::size_type colonPos = name.find(":");
    return (colonPos != std::string::npos) && (colonPos > 1);
}


bool
FileHelpers::isAbsolute(const std::string& path) {
    if (isSocket(path)) {
        return true;
    }
    // check UNIX - absolute paths
    if (path.length() > 0 && path[0] == '/') {
        return true;
    }
    // check Windows - absolute paths
    if (path.length() > 0 && path[0] == '\\') {
        return true;
    }
    if (path.length() > 1 && path[1] == ':') {
        return true;
    }
    if (path == "nul" || path == "NUL") {
        return true;
    }
    return false;
}


std::string
FileHelpers::checkForRelativity(const std::string& filename, const std::string& basePath) {
    if (filename == "stdout" || filename == "STDOUT" || filename == "-") {
        return "stdout";
    }
    if (filename == "stderr" || filename == "STDERR") {
        return "stderr";
    }
    if (filename == "nul" || filename == "NUL") {
        return "/dev/null";
    }
    if (!isAbsolute(filename)) {
        return getConfigurationRelative(basePath, filename);
    }
    return filename;
}


std::string
FileHelpers::getCurrentDir() {
    char buffer[1024];
    char* answer = getcwd(buffer, sizeof(buffer));
    if (answer) {
        return answer;
    }
    return "";
}


std::vector<std::string>
FileHelpers::splitDirs(const std::string& filename) {
    std::vector<std::string> result;
    for (const std::string& d : StringTokenizer(filename, "\\/", true).getVector()) {
        if (d == ".." && !result.empty() && result.back() != "..") {
            result.pop_back();
        } else if ((d == "" && result.empty()) || (d != "" && d != ".")) {
            result.push_back(d);
        }
    }
    return result;
}


std::string
FileHelpers::fixRelative(const std::string& filename, const std::string& basePath, const bool force, std::string curDir) {
    if (filename == "stdout" || filename == "STDOUT" || filename == "-") {
        return "stdout";
    }
    if (filename == "stderr" || filename == "STDERR") {
        return "stderr";
    }
    if (filename == "nul" || filename == "NUL" || filename == "/dev/null") {
        return "/dev/null";
    }
    if (isSocket(filename) || (isAbsolute(filename) && !force)) {
        return filename;
    }
    std::vector<std::string> filePathSplit = splitDirs(filename);
    std::vector<std::string> basePathSplit = splitDirs(basePath);
    if (isAbsolute(filename) || isAbsolute(basePath) || basePathSplit[0] == "..") {
        // if at least one is absolute we need to make the other absolute too
        // the same is true if the basePath refers to a parent dir
        if (curDir == "") {
            curDir = getCurrentDir();
        }
        if (!isAbsolute(filename)) {
            filePathSplit = splitDirs(curDir + "/" + filename);
        }
        if (!isAbsolute(basePath)) {
            basePathSplit = splitDirs(curDir + "/" + basePath);
        }
        if (filePathSplit[0] != basePathSplit[0]) {
            // don't try to make something relative on different windows disks
            return joinToString(filePathSplit, "/");
        }
    }
    while (!filePathSplit.empty() && !basePathSplit.empty() && filePathSplit[0] == basePathSplit[0]) {
        filePathSplit.erase(filePathSplit.begin());
        basePathSplit.erase(basePathSplit.begin());
    }
    for (int i = 0; i < (int)basePathSplit.size() - 1; i++) {
        filePathSplit.insert(filePathSplit.begin(), "..");
    }
    return joinToString(filePathSplit, "/");
}


std::string
FileHelpers::prependToLastPathComponent(const std::string& prefix, const std::string& path) {
    const std::string::size_type sep_index = path.find_last_of("\\/");
    if (sep_index == std::string::npos) {
        return prefix + path;
    } else {
        return path.substr(0, sep_index + 1) + prefix + path.substr(sep_index + 1);
    }
}

// ---------------------------------------------------------------------------
// binary reading/writing functions
// ---------------------------------------------------------------------------

std::ostream&
FileHelpers::writeInt(std::ostream& strm, int value) {
    strm.write((char*) &value, sizeof(int));
    return strm;
}


std::ostream&
FileHelpers::writeFloat(std::ostream& strm, double value) {
    strm.write((char*) &value, sizeof(double));
    return strm;
}


std::ostream&
FileHelpers::writeByte(std::ostream& strm, unsigned char value) {
    strm.write((char*) &value, sizeof(char));
    return strm;
}


std::ostream&
FileHelpers::writeString(std::ostream& strm, const std::string& value) {
    int size = (int)value.length();
    const char* cstr = value.c_str();
    writeInt(strm, size);
    strm.write((char*) cstr, (std::streamsize)(sizeof(char)*size));
    return strm;
}


std::ostream&
FileHelpers::writeTime(std::ostream& strm, SUMOTime value) {
    strm.write((char*) &value, sizeof(SUMOTime));
    return strm;
}


/****************************************************************************/
