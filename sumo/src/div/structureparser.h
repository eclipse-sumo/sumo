/*
$Id$
*/

#include <qxml.h>
#include <string>
#include <fstream>
#include <vector>
#include <utility>

class QString;

typedef std::pair <std::string,std::string> PairType;
typedef std::vector< PairType > ContType;

class StructureParser : public QXmlDefaultHandler
{

public:

    StructureParser(std::string datname);
    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );

    bool endElement( const QString&, const QString&, const QString& );

    // returns the iterator of the column, named like the searched tagname
    ContType::iterator getColumn (ContType::iterator start, std::string tofound);

private:
    // keeps the name of the outputfile
    // the name of outputfile and inputfile are the same,
    // except "xml" extension of the inputfile is replaced by a "cvs"extension for the outputfile
    std::string name_out;
    std::ofstream outfile;
    int indent;
    int maxindent;
    // symbol separating columnes
    std::string t;
    // keeps the names of the columns and the values of the actual row
    ContType tmpcont;

};
