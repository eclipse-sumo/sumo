#ifndef NIArcView_ShapeReader_h
#define NIArcView_ShapeReader_h

#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>

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

#endif
