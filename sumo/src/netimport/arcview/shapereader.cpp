#include "shapereader.h"
#include <cassert>
#include <utils/geom/GeomHelper.h>

shapereader::shapereader()
    : myWasInitialised(false)
{
    iRecord =       0;
    nEntities =     0;
    pnWidth =       0;
    pnDecimals =    0;
    sopen = dopen = false;
    psShape = NULL;
}

shapereader::shapereader(char * sfile, char * dfile)
    : myWasInitialised(false)
{
    iRecord =       0;
    nEntities =     0;
    pnWidth =       0;
    pnDecimals =    0;
    psShape =       NULL;

    if( !openFiles(sfile, dfile) )
        sopen = dopen = true ;
    readSHP();
}

shapereader::~shapereader()
{
    if ( psShape != NULL )
        SHPDestroyObject( psShape );

    if ( sopen || dopen)
        closeFiles();
}


int
shapereader::openFiles(const char * sfile, const char * dfile)
{
    int nDBFCount;

    hSHP = SHPOpen( sfile, "rb" );

    if( hSHP == NULL )
    {
    return 1; // cant open shp- or shx-file
    }

    hDBF = DBFOpen( dfile, "rb" );
    if( hDBF == NULL )
    {
        return 2; // cant open dbf-file
    }

    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );
    nDBFCount = DBFGetRecordCount(hDBF);
    if( nDBFCount != 0 )
    {
        //cout << nDBFCount << "\t" <<  nEntities << endl;
        if ( nDBFCount == nEntities )
        {
            readSHP();
            return readDBFHeader();
        }
        else
            return 3;//the datasetcount of the shp- and dbf-file dosnt match
    }
    else
    {
        return 4;//there are no fields in the dbf-file
    }

    return 0;
}


void
shapereader::closeFiles()
{
    DBFClose( hDBF );
    dopen = false;
    SHPClose( hSHP );
    sopen = false;
}

int
shapereader::readDBFHeader()
{
    for(int i = 0; i < DBFGetFieldCount(hDBF); i++ )
        {
            char t[12];
            eType = DBFGetFieldInfo( hDBF, i, t, pnWidth, pnDecimals );
            FieldName.erase();
            FieldName.append(t);
            //strncpy (pszFieldName, t, 11);
            /*if( eType == FTString )
                pszTypeName = "String";
            else if( eType == FTInteger )
                pszTypeName = "Integer";
            else if( eType == FTDouble )
                pszTypeName = "Double";
            else if( eType == FTInvalid )
                pszTypeName = "Invalid";
                */
            vecHeader.push_back(FieldName);
        }

    return 0;
}

int //if its ok its zero
shapereader::getFieldNumber(char * FieldName, int &res)
{
    if ( vecHeader.size() > 0 )
    {
        for(size_t i = 0; i < vecHeader.size(); i++)
        {
            if ( vecHeader[i].compare( FieldName ) == 0 )
            {
                res = i ;
                return 0;
            }
        }
    }
    return 1;
}
std::string
shapereader::getAttribute(char *attrib)
{
    std::string ret;
    int i; //getFieldNubmer(attrib)
    int col;
    char Field[12];
    i = getFieldNumber(attrib, col );
    if ( i != 0 )
        return "FALSE";
    if( DBFIsAttributeNULL( hDBF, iRecord, col ) )
    {
        ret = "ZERO"; // the value is zero
    }
    else
    {
        eType = DBFGetFieldInfo( hDBF, col, Field, pnWidth, pnDecimals );
        switch( eType )
        {
            case FTString:
                ret = DBFReadStringAttribute( hDBF, iRecord, col ) ;
                break;

            case FTInteger:
                ret= DBFReadStringAttribute( hDBF, iRecord, col ) ;
                break;

            case FTDouble:
                ret= DBFReadStringAttribute( hDBF, iRecord, col ) ;
                break;

            default:
                ret = "TYPE_ERROR";
                break;
        }
    }
    return ret;
}

int
shapereader::getAttribute(char *attrib,const char *&res)
{
    /*the meaning of the return value:
    1:  the value is NULL or not of the type which will parsed
    2:  the value is a string
    3:  the value is a integer
    4:  the value is a double
    */
    int i; //getFieldNubmer(attrib)
    int col;
    int ret;
    char Field[12];
    i = getFieldNumber(attrib, col );
    if ( i != 0 )
        return 1;
    if( DBFIsAttributeNULL( hDBF, iRecord, col ) )
    {
        ret = 1; // the value is zero
    }
    else
    {
        eType = DBFGetFieldInfo( hDBF, col, Field, pnWidth, pnDecimals );
        switch( eType )
        {
            case FTString:
                res = DBFReadStringAttribute( hDBF, iRecord, col ) ;
                ret = 2;
                break;

            case FTInteger:
                res = DBFReadStringAttribute( hDBF, iRecord, col ) ;
                ret = 3;
                break;

            case FTDouble:
                res = DBFReadStringAttribute( hDBF, iRecord, col ) ;
                ret = 4;
                break;

            default:
                ret = 1;
                break;
        }
    }
    return ret;
}


//put the hole shape in the SchapeObject and release it if its still filled
int// if its returns one if an illegal iRecord value is requested.
shapereader::readSHP()
{
    if ( psShape != NULL )
        SHPDestroyObject( psShape );

    psShape = SHPReadObject( hSHP, iRecord );
    if ( psShape == NULL )
        return 1;
    return getgeoStruct();
    //return 0;
}

int// if iRecord == nEntities the returnvalue is one
shapereader::forwardShape()
{
    assert( iRecord < nEntities );
    if ( iRecord < nEntities )
    {
        iRecord++;
        return readSHP();
    }
    return 1;
}

int
shapereader::backwardShape()
{
    if ( iRecord > 0 )
    {
        iRecord--;
        return readSHP();
    }
    return 1;
}

int//if id is not between zero and nEntities returnvalue is one
shapereader::letShape(int id)
{
    if ( 0 <= id  && id >= nEntities )
        iRecord = id ;
    else
        return 1;

    return readSHP();
}

int
shapereader::getgeoStruct()
{
    myPoints.clear();

    for(int j = 0; j < psShape->nVertices; j++ )
    {
        double x = psShape->padfX[j];
        double y = psShape->padfY[j];
        if(!myWasInitialised) {
            myWasInitialised = true;
            myInitX = x;
            myInitY = y;
        }
        double ys = y;
        x = (x-myInitX);
        y = (y-myInitY);
        double x1 = x * 111.320*1000;
        double y1 = y * 111.136*1000;
        x1 *= cos(ys*PI/180.0);
        myPoints.push_back( Position2D( x1, y1 ) );
    }
    return 0;
}

const Position2D &
shapereader::getFromNodePosition() const
{
    return myPoints.at(0);
}


const Position2D &
shapereader::getToNodePosition() const
{
    return myPoints.at(myPoints.size()-1);
}

double shapereader::getLength()
{
    return myPoints.length();
}

const Position2DVector &
shapereader::getShape() const
{
    return myPoints;
}


Position2DVector
shapereader::getReverseShape() const
{
    return myPoints.reverse() ;
}

/*shapereader::debug()
{
    Position2D p;
    for(int j = 0, iPart = 1; j < psShape->nVertices; j++ )
    {
        const char  *pszPartType = "";

        if( j == 0 && psShape->nParts > 0 )
            cot << SHPPartTypeName( psShape->panPartType[0] ) << endl;
            //pszPartType = SHPPartTypeName( psShape->panPartType[0] );

        if( iPart < psShape->nParts && psShape->panPartStart[iPart] == j )
        {
            cot << SHPPartTypeName( psShape->panPartType[iPart] ) << endl;
                //pszPartType = SHPPartTypeName( psShape->panPartType[iPart] );
        iPart++;

        }
        p = myPoints.at(j);
        cut << p.x() << "\t"   << p.y() << endl;
    }
    //myPoints.debug();
}*/
