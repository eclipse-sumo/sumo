/****************************************************************************/
/// @file    tagreader.cpp
/// @author  unknown_author
/// @date    unknown_date
/// @version $Id: $
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <div/structureparser.h>
#include <qfile.h>
#include <qxml.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

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

