#ifndef NBLoader_h
#define NBLoader_h
/***************************************************************************
                          NBLoader.h
                          An interface to the loading operations of the
                          netconverter
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:36:43  dkrajzew
// Visum7.5 and Cell import added
//
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "NLLoadFilter.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
class OptionsCont;
class SUMOSAXHandler;
class LineReader;
class LineHandler;
class SAX2XMLReader;

/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NBLoader
 * This class loads the specified data using a loading method that accords
 * to the data type specified by the user.
 * Subclasses may be used for the loading process, too, as it is done while
 * loading the XML-descriptions.
 */
class NBLoader {
public:
    /** loads the data from the files specified in the given option container */
    static void load(OptionsCont &oc);
    /// the information whether the loading shall be done in the verbose mode
    static bool _verbose;
private:
    /** loads data from sumo-files */
    static void loadSUMO(OptionsCont &oc, bool warn);
    /** loads net or logics */
    static void loadSUMOFiles(OptionsCont &oc, LoadFilter what,
        const std::string &files, const std::string &type);
    /** loads data from XML-files */
    static void loadXML(OptionsCont &oc, bool warn);
    /** loads data from the list of xml-files of certain type */
    static void loadXMLType(SUMOSAXHandler *handler,
        const std::string &files, const std::string &type);
    /** loads data from a single xml-file */
    static void loadXMLFile(SAX2XMLReader &parser, const std::string &file,
        const std::string &type);
    /** loads data from cell-input-files */
    static void loadCell(OptionsCont &oc, bool warn);
    /** reads using a file reader */
    static bool useLineReader(LineReader &lr, const std::string &file, LineHandler &lh);
    /** loads data from visum-input-file */
    static void loadVisum(OptionsCont &oc, bool warn);
    /// prints the given message when running in verbose mode
    static void reportBegin(const std::string &msg);
    /// prints the message "done." when running in verbose mode
    static void reportEnd();
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

