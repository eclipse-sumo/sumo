#ifndef Bin_ShapeReader_h
#define Bin_ShapeReader_h


#include <string>
//#include <iostream.h>
#include <vector>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include "shapefil.h"


class shapereader
{
public:
    shapereader();
    shapereader(char * sfile, char * dfile);
    ~shapereader();
    int openFiles(const char * sfile, const char * dfile);
    int closeFiles();
    std::string getAttribute(char *attrib);
    int getAttribute(char * attrib, const char *&res);
    int forwardShape();
    int backwardShape();
    int letShape( int id );
    //debug();
    int getShapeCount() { return nEntities;};
    int getShapeID() {return iRecord;};

       /// Returns the position of the from-node
    const Position2D &getFromNodePosition() const;

    /// Returns the position of the to-node
    const Position2D &getToNodePosition() const;
    double getLength();
    /// Returns the points the edge goes trough
    const Position2DVector &getShape() const;
    Position2DVector getReverseShape() const;

private:
    int getFieldNumber(char * FieldName, int &result);
    int readDBFHeader();
    int readSHP();
    //clear();????????????????????????????
    int getgeoStruct();
private:
    DBFHandle	hDBF;
    SHPHandle	hSHP;
    bool        dopen, sopen;

    SHPObject	*psShape;
    DBFFieldType eType;
    int         iRecord;        //points to the current entity
    int	        nShapeType;     // type of shape
    int         nEntities;      // total count of entities
    double 	adfMinBound[4], adfMaxBound[4];
    //char	* pszFieldName;
    std::string FieldName;
    int * pnWidth;
    int * pnDecimals;
    std::vector<std::string> vecHeader; //cache the dbf-header
    Position2DVector myPoints;

};
#endif
