/*
$Id$
*/

#include "structureparser.h"
#include <qstring.h>
#include <iostream>

using namespace std;
 

StructureParser::StructureParser(std::string datname) 
{
  name_out = datname;
  name_out.replace((name_out.end()-3),name_out.end(),"txt");
  t = ";" ;
}


bool StructureParser::startDocument()
{
    outfile.open(name_out.c_str(),ios_base::app);
    indent = 0;
    maxindent = 0;
    return TRUE;
}



ContType::iterator StructureParser::getColumn (ContType::iterator start, std::string tofound)
{
    for (  ContType::iterator i = start; i !=tmpcont.end(); i++)    {
        if (i->first == tofound)    {
            break;
        };
    }
    return i;
}



bool StructureParser::startElement( const QString&, const QString&, 
                                    const QString& qName, 
                                    const QXmlAttributes& myattr)
{
    QXmlAttributes a = myattr;
    
    std::string tmp = qName;  
    ContType::iterator it = tmpcont.end();
    ContType::iterator itS = tmpcont.begin();
    
    if (it==getColumn(itS, tmp))    {
        PairType pt (tmp, "");
        tmpcont.push_back(pt);
        maxindent ++;
        cout << tmp << t;
        outfile << tmp << t ;
    }
    
    int l = a.length();
    for (int i = 0; i<l; i++)   {          
        it = tmpcont.end();
        itS = tmpcont.begin();
        ContType::iterator st = getColumn (itS, tmp );
        
        std::string s = a.localName(i);    
        ContType::iterator itF = getColumn(st, s);
                
        if (it == itF ) {
            std::string v = a.value(i);         
            PairType pt1 (s,v);
            tmpcont.push_back(pt1);
            cout << s << t;
            outfile << s << t ;
        }
        else    {   
            std::string v = a.value(i);         
            itF->second = v;         
        }
    }
         
    indent ++;
    return TRUE;
}



bool StructureParser::endElement( const QString&, const QString&, const QString& )
{
    if (indent == maxindent)     {   
        outfile << endl;
        cout << endl;
        for (ContType::iterator i = tmpcont.begin(); i !=tmpcont.end(); i++)     {
            outfile << i->second << t ;
            cout << i->second << t ;
        }
    }
        
    indent --; 
    return TRUE;
}

