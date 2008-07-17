/****************************************************************************/
/// @file    MFXUtils.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// Some helper functions for FOX
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
#ifndef MFXUtils_h
#define MFXUtils_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MFXUtils
 * @brief Some helper functions for FOX
 */
class MFXUtils
{
public:
    /** @brief Deletes all children of the given window
     *
     * @param[in] w The window to delete all of his children
     */
    static void deleteChildren(FXWindow *w) throw();


    /** @brief Returns true if either the file given by its name does not exist or the user allows overwriting it
     *
     * If the named file does not exist, true is returned. Otherwise, a message
     *  box is prompted that asks whether the file may be replaced. If the user
     *  answers "yes" in this case, true is returned. In any other cases
     *  ("no"/"cancel"), false.
     *
     * @param[in] parent A parent window needed to prompt the dialog box
     * @param[in] file The file to check whether it may be generated
     * @return Whether the named file may be written
     */
    static FXbool userPermitsOverwritingWhenFileExists(
        FXWindow * const parent, const FXString &file) throw();


    /** @brief Returns the title text in dependance to an optional file name
     *
     * The title is computed as default on windows: The application name only if no 
     *  file name is given. If a file name is given, it is used without the extension, 
     *  extended by the application name.
     *
     * @param[in] appname The name of the application to return the title of
     * @param[in] appname The name of the file loaded by the application
     * @return The built title
     */
    static FXString getTitleText(const FXString &appname, 
        FXString filename="") throw();


    /** @brief Returns the document name
     * 
     * Removes the path first. Then, returns the part before the first '.'
     *  occurence of the so obtained string.
     *
     * @param[in] filename The file name (including the path) to obtain the name of
     * @return The name (without the path and the extension)
     */
    static FXString getDocumentName(const FXString &filename) throw();

};


#endif

/****************************************************************************/

