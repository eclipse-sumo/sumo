#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/common/SErrorHandler.h>
#include <utils/geom/Position2DVector.h>
#include "NIArtemisTempSegments.h"

using namespace std;

NIArtemisTempSegments::Link2PosInfos
        NIArtemisTempSegments::myLinkPosInfos;

NIArtemisTempSegments::SingleGeomPoint::SingleGeomPoint(int segment,
                                                       double x,
                                                       double y)
    : mySegment(segment), myX(x), myY(y)
{
}


NIArtemisTempSegments::SingleGeomPoint::~SingleGeomPoint()
{
}


Position2D
NIArtemisTempSegments::SingleGeomPoint::getPosition() const
{
    return Position2D(myX, myY);
}


int
NIArtemisTempSegments::SingleGeomPoint::getSegmentNo() const
{
    return mySegment;
}





void
NIArtemisTempSegments::add(const std::string &linkname, int segment,
                          double x, double y)
{
    myLinkPosInfos[linkname].push_back(
        new SingleGeomPoint(segment, x, y));
}


void
NIArtemisTempSegments::close()
{
    // go through the read values
    for(Link2PosInfos::iterator i=myLinkPosInfos.begin(); i!=myLinkPosInfos.end(); i++) {
        // get the current list of segment definitions
        PosInfos infos = (*i).second;
        // sort them
        sort(infos.begin(), infos.end(), info_sorter());
        // build the geomentry list
        Position2DVector geom;
        for(PosInfos::iterator j=infos.begin(); j!=infos.end(); j++) {
            geom.push_back((*j)->getPosition());
        }
        // add to the edge
        string name = (*i).first;
        NBEdge *edge = NBEdgeCont::retrieve(name);
        if(edge==0) {
            SErrorHandler::add(
                string("Trying to parse the geometry for the unknown edge '")
                + name + string("'."));
        } else {
            edge->setGeometry(geom);
        }
    }
}
