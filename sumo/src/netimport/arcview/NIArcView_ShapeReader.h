#ifndef NIArcView_ShapeReader_h
#define NIArcView_ShapeReader_h
//---------------------------------------------------------------------------//
//                        NIArcView_ShapeReader.h -
//  The reader of shape-files
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
// Revision 1.3  2003/12/04 16:53:54  dkrajzew
// native ArcView-importer by ericnicolay added
//
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
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIArcView_ShapeReader :
        public FileErrorReporter,
        public LineHandler
{
public:
    /// Constructor
    NIArcView_ShapeReader(const std::string &file);

    /// Destrcutor
    ~NIArcView_ShapeReader();

    /// Reads the shape information
    void readShape(size_t no);

    /// Retrieves information from the shp-file
    bool report(const std::string &line);

    /// Returns the points the edge goes trough
    const Position2DVector &getShape() const;
    const Position2DVector &getReverseShape() const;
    /// Returns the position of the from-node
    const Position2D &getFromNodePosition() const;

    /// Returns the position of the to-node
    const Position2D &getToNodePosition() const;

private:
    // Parses a position description from the given line
    Position2D parsePoint(const std::string &line);

    // Retrieves the numbr of the shape from the given line
    int parseShapeNumber(const std::string &line);

    // Retrieves the value of a named parameter (nVertices/nParts)
    int getNamedParam(const std::string &from, const std::string &name);

private:
    /// information whether the wanted shape was found and is currently being read
    bool myFoundAndProcessing;

    /// the number of the wanted shape
    size_t mySearchedShape;

    /// list of geometrical points
    Position2DVector myPoints;

    /// information whether the shape was read
    bool myShapeRead;

    /// the line reader used
    LineReader myLineReader;

    /** @brief the current processing step
        The following steps are enumerated: reading upper left corener,
        reading lower right corner, reading points */
    size_t myParseStep;

    /// The corners of the edge
    Position2D myULCorner, myDRCorner;

    /// the number of points the edge currently parsed consists of
    size_t myVertexNo;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIArcView_ShapeReader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

