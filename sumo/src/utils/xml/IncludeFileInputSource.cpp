/****************************************************************************/
/// @file    IncludeFileInputSource.cpp
/// @author  Michael Behrisch
/// @date    Nov 2007
/// @version $Id$
///
// A handler which converts occuring elements and attributes into enums
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "IncludeFileInputSource.hpp"
#include "IncludeFileInputStream.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

/***
 *
 * Originated by Chris larsson
 *
 * Issue:
 *
 * There is an inconsistency in URI resolution in the case where the file itself is a
 * symbolic link to another path (or the path has path segment which is a symbolic
 * link to another path). So, is the base path the directory where the symbolic link resides
 * or the directory where the real file resides? I'm sure one could argue either way,
 * but I think that having the base path be the directory where the symbolic link resides
 * is more intuitive.
 *
 * Defining it this way would then make the behavior consistent with using an absolute
 * path as well as with the java behavior.
 *
 * Proposal:
 *
 * The URI is resolved within the parser code, and is somewhat independant of the OS.
 *
 * A relative path is resolved by querying the current directory and appending the
 * relative part onto the returned current directory string to obtain the base URI.
 * An absolute path is simply used as the base URI.
 * Then remove all "./" and "../" path segments using an algorithm like weavepath to obtain
 * the resolved base URI.
 *
 * When you need to access another file such as a dtd, use the resolved base URI and add on
 * the relative URI of the dtd file. Then resolve it using the same weavepath algorithm.
 *
 * Note:
 *
 *   Java parser behaves differently for a path containning symbolic path segment. When
 *   it is given an absolute path, it can locate the primary instance document, while given
 *   relative path, it might not.
 *
 *   It is because Java parser uses URI solution where "/segment/../" is required to be removed
 *   from the resultant path if a relative URI is merged to a baseURI. While this is NOT required
 *   for an absolute URI.
 *
 *   So if a path segment, which is symbolic link, happen to be followed by the '/../', it is
 *   NOT removed from the path if it is given in absolute form, and the underlying file system
 *   will locate the file, if in relative form, that symbolic link path segment together with
 *   '../' is removed from the resultant path, and the file system may NOT be able to locate
 *   the file, if there is a one, it is definitely not the one expected, in fact by accident.
 *
 *   Therefore, to keep consistent with Java parser, for now, we do not apply removeDotDotSlash()
 *   for absolute path.
 *
 ***/

// ---------------------------------------------------------------------------
//  IncludeFileInputSource: Constructors and Destructor
// ---------------------------------------------------------------------------
IncludeFileInputSource::IncludeFileInputSource(const XMLCh* const basePath
        , const XMLCh* const relativePath)
        : InputSource()
{
    //
    //  If the relative part is really relative, then weave it together
    //  with the base path. If not, just take the relative path as the
    //  entire path.
    //
    if (XMLPlatformUtils::isRelative(relativePath)) {
        XMLCh* tmpBuf = XMLPlatformUtils::weavePaths(basePath, relativePath);
        setSystemId(tmpBuf);
        manager->deallocate(tmpBuf); //delete [] tmpBuf;
    } else {
        XMLCh* tmpBuf = XMLString::replicate(relativePath);
        XMLPlatformUtils::removeDotSlash(tmpBuf);
        setSystemId(tmpBuf);
        manager->deallocate(tmpBuf);//delete [] tmpBuf;
    }

}

IncludeFileInputSource::IncludeFileInputSource(const XMLCh* const filePath)
        : InputSource()
{

    //
    //  If the path is relative, then complete it acording to the current
    //  working directory rules of the current platform. Else, just take
    //  it as is.
    //
    if (XMLPlatformUtils::isRelative(filePathIncludeFile)) {
        XMLCh* curDir = XMLPlatformUtils::getCurrentDirectory(manager);

        int    curDirLen = XMLString::stringLen(curDir);
        int    filePathLen = XMLString::stringLen(filePath);
        XMLCh* fullDir = (XMLCh*) manager->allocate
                         (
                             (curDirLen + filePathLen + 2) * sizeof(XMLCh)
                         );//new XMLCh [ curDirLen + filePathLen + 2];

        XMLString::copyString(fullDir, curDir);
        fullDir[curDirLen] = chForwardSlash;
        XMLString::copyString(&fullDir[curDirLen+1], filePath);

        XMLPlatformUtils::removeDotSlash(fullDirIncludeFile);
        XMLPlatformUtils::removeDotDotSlash(fullDirIncludeFile);

        setSystemId(fullDir);

        manager->deallocate(curDir);//delete [] curDir;
        manager->deallocate(fullDir);//delete [] fullDir;
    } else {
        XMLCh* tmpBuf = XMLString::replicate(filePathIncludeFile);
        XMLPlatformUtils::removeDotSlash(tmpBufIncludeFile);
        setSystemId(tmpBuf);
        manager->deallocate(tmpBuf);//delete [] tmpBuf;
    }

}

IncludeFileInputSource::~IncludeFileInputSource()
{
}


// ---------------------------------------------------------------------------
//  IncludeFileInputSource: InputSource interface implementation
// ---------------------------------------------------------------------------
BinInputStream* IncludeFileInputSource::makeStream() const
{
    BinFileInputStream* retStrm = new IncludeFileInputStream(getSystemId());
    if (!retStrm->getIsOpen()) {
        delete retStrm;
        return 0;
    }
    return retStrm;
}

