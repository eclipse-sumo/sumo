/****************************************************************************/
/// @file    tagreader.cpp
/// @author  Julia Ringel
/// @date    unknown_date
/// @version $Id$
///
// */
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <div/structureparser.h>
#include <qfile.h>
#include <qxml.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

int main(int argc, char **argv)
{
    for (int i=1; i < argc; i++) {
        StructureParser handler(argv[i]);
        QFile xmlFile(argv[i]);
        QXmlInputSource source(xmlFile);
        QXmlSimpleReader reader;
        reader.setContentHandler(&handler);
        reader.parse(source);
    }
    return 0;
}



/****************************************************************************/

