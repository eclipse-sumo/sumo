/*
$Id$
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <div/structureparser.h>
#include <qfile.h>
#include <qxml.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

int main( int argc, char **argv )
{
    for ( int i=1; i < argc; i++ ) {
        StructureParser handler (argv[i]);
        QFile xmlFile( argv[i] );
        QXmlInputSource source( xmlFile );
        QXmlSimpleReader reader;
        reader.setContentHandler( &handler );
        reader.parse( source );
    }
    return 0;
}
