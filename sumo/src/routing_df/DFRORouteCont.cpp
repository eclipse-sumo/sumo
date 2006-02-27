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


DFRORouteCont::DFRORouteCont(const DFRORouteCont &s)
{
	for(std::vector<DFRORouteDesc*>::const_iterator j=s.myRoutes.begin(); j!=s.myRoutes.end(); ++j) {
		DFRORouteDesc *desc = (*j);
		myRoutes.push_back(desc);
	}
}

DFRORouteCont::~DFRORouteCont()
{
}


void
DFRORouteCont::addRouteDesc(DFRORouteDesc *desc)
{
	/*
    ROEdge *start = desc.edges2Pass[0];
    if(myRoutes.find(start)==myRoutes.end()) {
        myRoutes[start] = std::vector<DFRORouteDesc>();
    }
	*/
    myRoutes.push_back(desc);
}


bool
DFRORouteCont::readFrom(const std::string &file)
{
    throw 1;
}


bool
DFRORouteCont::save(std::vector<std::string> &saved,
					const std::string &prependix, std::ostream &os/*const std::string &file*/)
{
	/*
    ofstream strm(file.c_str());
    if(!strm.good()) {
        return false;
    }
    //strm << "<routes>" << endl;
//    for(std::map<ROEdge*, std::vector<DFRORouteDesc> >::iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
//        const std::vector<DFRORouteDesc> &routes = (*i).second;
*/
	bool haveSavedOnAtLeast = false;
        for(std::vector<DFRORouteDesc*>::const_iterator j=myRoutes.begin(); j!=myRoutes.end(); ++j) {
            const DFRORouteDesc *desc = (*j);
			if(find(saved.begin(), saved.end(), (*j)->routename)!=saved.end()) {
				continue;
			}
			saved.push_back((*j)->routename);
            assert(desc->edges2Pass.size()>=1);
            os << "   <route id=\"" << prependix << (*j)->routename << "\" multi_ref=\"x\">";
            for(std::vector<ROEdge*>::const_iterator k=desc->edges2Pass.begin(); k!=desc->edges2Pass.end(); k++) {
                if(k!=desc->edges2Pass.begin()) {
                    os << ' ';
                }
                os << (*k)->getID();
            }
            os << "</route>" << endl;
			haveSavedOnAtLeast = true;
        }
/*        strm << endl;
  //  }
    //strm << "</routes>" << endl;
    return true;*/
	return haveSavedOnAtLeast;
}


bool
DFRORouteCont::computed() const
{
    return myRoutes.size()!=0;
}


const std::vector<DFRORouteDesc*> &
DFRORouteCont::get() const
{
	return myRoutes;
}


void
DFRORouteCont::sortByDistance()
{
    sort(myRoutes.begin(), myRoutes.end(), by_distance_sorter());
}


void
DFRORouteCont::setDets2Follow(const std::map<ROEdge*, std::vector<ROEdge*> > &d2f)
{
    myDets2Follow = d2f;
}


const std::map<ROEdge*, std::vector<ROEdge*> > &
DFRORouteCont::getDets2Follow() const
{
    return myDets2Follow;
}

