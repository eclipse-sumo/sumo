/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <fstream>
#include <cassert>
#include "DFRORouteDesc.h"
#include "DFRORouteCont.h"
#include <router/ROEdge.h>


using namespace std;


DFRORouteCont::DFRORouteCont()
{
}


DFRORouteCont::~DFRORouteCont()
{
}


void
DFRORouteCont::addRouteDesc(const DFRORouteDesc &desc)
{
    ROEdge *start = desc.edges2Pass[0];
    if(myRoutes.find(start)==myRoutes.end()) {
        myRoutes[start] = std::vector<DFRORouteDesc>();
    }
    myRoutes[start].push_back(desc);
}


bool
DFRORouteCont::readFrom(const std::string &file)
{
    throw 1;
}


bool
DFRORouteCont::save(const std::string &file)
{
    ofstream strm(file.c_str());
    if(!strm.good()) {
        return false;
    }
    strm << "<routes>" << endl;
    for(std::map<ROEdge*, std::vector<DFRORouteDesc> >::iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
        const std::vector<DFRORouteDesc> &routes = (*i).second;
        for(std::vector<DFRORouteDesc>::const_iterator j=routes.begin(); j!=routes.end(); ++j) {
            const DFRORouteDesc &desc = (*j);
            assert(desc.edges2Pass.size()>=1);
            ROEdge *first = *(desc.edges2Pass.begin());
            ROEdge *last = *(desc.edges2Pass.end()-1);
            strm << "   <route id=\"" << first->getID() << "_to_" << last->getID() << "\" multi_ref=\"x\">";
            for(std::vector<ROEdge*>::const_iterator k=desc.edges2Pass.begin(); k!=desc.edges2Pass.end(); k++) {
                if(k!=desc.edges2Pass.begin()) {
                    strm << ' ';
                }
                strm << (*k)->getID();
            }
            strm << "</route>" << endl;
        }
        strm << endl;
    }
    strm << "</routes>" << endl;
    return true;
}


bool
DFRORouteCont::computed() const
{
    return myRoutes.size()!=0;
}


