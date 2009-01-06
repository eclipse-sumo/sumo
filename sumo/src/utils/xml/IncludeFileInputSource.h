/****************************************************************************/
/// @file    IncludeFileInputSource.h
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
#ifndef IncludeFileInputSource_h
#define IncludeFileInputSource_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax/InputSource.hpp>


// ===========================================================================
// class declarations
// ===========================================================================
class BinInputStream;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *  This class is a derivative of the standard InputSource class. It provides
 *  for the parser access to data which is referenced via a local file path
 *  and may contain include references to other local files.
 *
 *  The path provided can either be a fully qualified path or a relative path.
 *  If relative, it will be completed either relative to a passed base path
 *  or relative to the current working directory of the process.
 */
class IncludeFileInputSource : public InputSource
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------

    /** @name Constructors */
    //@{

    /**
      * A local file input source requires a path to the file to load. This
      * can be provided either as a fully qualified path, a path relative to
      * the current working directly, or a path relative to a provided base
      * path.
      *
      * This constructor takes an explicit base path and a possibly relative
      * path. If the relative path is seen to be fully qualified, it is used
      * as is. Otherwise, it is made relative to the passed base path.
      *
      * @param  basePath    The base path from which the passed relative path
      *                     will be based, if the relative part is indeed
      *                     relative.
      *
      * @param  relativePath    The relative part of the path. It can actually
      *                         be fully qualified, in which case it is taken
      *                         as is.
      *
      * @exception XMLException If the path is relative and doesn't properly
      *            resolve to a file.
      */
    IncludeFileInputSource
    (
        const   XMLCh* const   basePath
        , const XMLCh* const   relativePath
    );

    /**
      * This constructor takes a single parameter which is the fully qualified
      * or relative path. If it is fully qualified, it is taken as is. If it is
      * relative, then it is completed relative to the current working directory
      * (or the equivalent on the local host machine.)
      *
      * @param  filePath    The relative or fully qualified path.
      *
      * @exception XMLException If the path is relative and doesn't properly
      *            resolve to a file.
      */
    IncludeFileInputSource
    (
        const   XMLCh* const   filePath
    );
    //@}

    /** @name Destructor */
    //@{
    ~IncludeFileInputSource();
    //@}


    // -----------------------------------------------------------------------
    //  Virtual input source interface
    // -----------------------------------------------------------------------

    /** @name Virtual methods */
    //@{

    /**
    * This method will return a binary input stream derivative that will
    * parse from the local file indicatedby the system id.
    *
    * @return A dynamically allocated binary input stream derivative that
    *         can parse from the file indicated by the system id.
    */
    virtual BinInputStream* makeStream() const;

    //@}
private:
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    IncludeFileInputSource(const IncludeFileInputSource&);
    IncludeFileInputSource& operator=(const IncludeFileInputSource&);

};

#endif
