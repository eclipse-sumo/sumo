#ifndef NIArcView_Loader_h
#define NIArcView_Loader_h
//---------------------------------------------------------------------------//
//                        NIArcView_Loader.h -
//  The loader of arcview-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2003/06/05 11:44:14  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include "NIArcView_ShapeReader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIArcView_Loader :
    public FileErrorReporter,
    public LineHandler
{
public:
    /// Contructor
    NIArcView_Loader(const std::string &dbf_name,
        const std::string &shp_name);

    /// Destructor
    ~NIArcView_Loader();

    /// loads the navtech-data
    void load(OptionsCont &options);

    /// Retrieves data from the dbf-file
    bool report(const std::string &line);

private:
    /// parses a non-first line (a data line)
    bool parseLine(const std::string &line);

    /// parses the maximum speed allowed on the edge currently processed
    double getSpeed(const std::string &edgeid);

    /// parses the number of lanes of the edge currently processed
    size_t getLaneNo(const std::string &edgeid);

    /// parses the length of the edge currently processed
    double getLength(const std::string &edgeid,
        const Position2D &from_pos, const Position2D &to_pos);

    /// parses the priority of the edge currently processed
    int getPriority(const std::string &edgeid);

private:
    /// The shape reader
    NIArcView_ShapeReader myShapeReader;

    /// Information whether the current line is the first line
    bool myIsFirstLine;

    /// The line reader which parses the dbf-file
    LineReader myLineReader;

    /// The number of the current link
    size_t myCurrentLink;

    /// parser of the dbf-file columns
    NamedColumnsParser myColumnsParser;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIArcView_Loader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

