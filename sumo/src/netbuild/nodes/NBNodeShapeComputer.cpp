#include <utils/geom/Position2DVector.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/StdDefs.h>
#include "NBNode.h"
#include "NBNodeShapeComputer.h"


NBNodeShapeComputer::NBNodeShapeComputer(const NBNode &node)
    : myNode(node)
{
}

NBNodeShapeComputer::~NBNodeShapeComputer()
{
}


Position2DVector
NBNodeShapeComputer::compute()
{
    Position2DVector ret;
    // compute the shape of the junction
    //  only junction have a shape, otherwise it's somekind
    //  of another connection between streets
/*    if( (_incomingEdges->size()<1||_outgoingEdges->size()<1)
        ||
        (_incomingEdges->size()<2&&_outgoingEdges->size()<2) ) {
        return;
    }*/
    // check whether this is a real junction between different edges
    //  or only an edge's split/join
    bool isRealJunction = false;
    for(EdgeVector::const_iterator i=myNode._allEdges.begin(); i!=myNode._allEdges.end()&&!isRealJunction; i++) {
        Line2D incLine;
        if(myNode.hasIncoming(*i)) {
            const Position2DVector &geom = (*i)->getGeometry();
            incLine = Line2D(geom.at(geom.size()-2), geom.at(geom.size()-1));
        } else {
            const Position2DVector &geom = (*i)->getGeometry();
            incLine = Line2D(geom.at(1), geom.at(0));
        }
        NBEdge *e1 = (*i);
        double angle1 = incLine.atan2DegreeAngle();
        for(EdgeVector::const_iterator i2=i+1; i2!=myNode._allEdges.end()&&!isRealJunction; i2++) {
            Line2D outLine;
            NBEdge *e2 = (*i2);
            if(myNode.hasIncoming(*i2)) {
                const Position2DVector &geom = (*i2)->getGeometry();
                outLine = Line2D(geom.at(geom.size()-2), geom.at(geom.size()-1));
            } else {
                const Position2DVector &geom = (*i2)->getGeometry();
                outLine = Line2D(geom.at(1), geom.at(0));
            }
            double angle2 = outLine.atan2DegreeAngle();
            //
            if(abs(angle1-angle2)>35&&abs(angle1-angle2)<145) {
                isRealJunction = true;
            }
        }
    }
//    if(isRealJunction) {
        ret = computeRealNodeShape();
/*    } else {
        computeJoinSplitNodeShape();
    }*/
    // add the geometry of internal lanes
    if(OptionsSubSys::getOptions().getBool("add-internal-links")) {
        addInternalGeometry();
    }
    ret = ret.convexHull();
    ret.closePolygon();
    return ret;
}


void
NBNodeShapeComputer::addInternalGeometry()
{
    /*
    for(EdgeVector::const_iterator i=myNode._incomingEdges->begin(); i!=myNode._incomingEdges->end(); i++) {
        size_t noLanesEdge = (*i)->getNoLanes();
        for(size_t j=0; j<noLanesEdge; j++) {
            const EdgeLaneVector *elv = (*i)->getEdgeLanesFromLane(j);
            for(EdgeLaneVector::const_iterator k=elv->begin(); k!=elv->end(); k++) {
                if((*k).edge==0) {
                    continue;
                }
                Position2DVector shape =
                    computeInternalLaneShape(*i, j, (*k).edge, (*k).lane);
                if(shape.length()==0) {
                    continue;
                }
                Position2DVector l(shape);
                l.move2side(1.5);
                Position2DVector r(shape);
                l.move2side(-1.5);
                myPoly.push_back(shape);
            }
        }
    }
    */
}


Position2DVector
NBNodeShapeComputer::computeRealNodeShape()
{
    Position2DVector ret;
    std::vector<double> edgeOffsets;
    EdgeVector::const_iterator i;
    for(i=myNode._allEdges.begin(); i!=myNode._allEdges.end(); i++) {
        NBEdge *current = *i;
        // the clockwise and the counter clockwise border;
        Position2DVector cb, ccb;
        // the left and the right crossing line
        // the crossing edges
        EdgeVector::const_iterator ri = i;
        EdgeVector::const_iterator li = i;
        double ccw = current->width();
        double cw = current->width();
        // clockwise border
        cb = current->getCWBounderyLine(myNode, 2.5);
        // counterclockwise border
        ccb = current->getCCWBounderyLine(myNode, 2.5);
        Position2DVector cl, ccl;
        if(myNode.hasIncoming(current)) {
            // is an incoming edge
            //  the counter clockwise edge is surely not the opposite direction
            NBContHelper::nextCCW(&(myNode._allEdges), ri);
            //  check the clockwise edge
            NBContHelper::nextCW(&(myNode._allEdges), li);
            if(current->isTurningDirection(*li)/*&&_incomingEdges->size()>2*/) {
                EdgeVector::const_iterator tmpi = li;
                NBContHelper::nextCW(&(myNode._allEdges), tmpi);
                double angle =
                    myNode.getCWAngleDiff(
                        cb.lineAt(0).atan2DegreeAngle(),
                        (*tmpi)->getCCWBounderyLine(myNode, 2.5).lineAt(0).atan2DegreeAngle());
                if(angle<90) {
                    cb = (*li)->getCWBounderyLine(myNode, 2.5);
                    cw = (*li)->width();
                }
                NBContHelper::nextCW(&(myNode._allEdges), li);
            }
            ccl = (*ri)->getCWBounderyLine(myNode, 2.5);
            cl = (*li)->getCCWBounderyLine(myNode, 2.5);
        } else {
            NBContHelper::nextCCW(&(myNode._allEdges), ri);
            if((*ri)->isTurningDirection(current)) {
                EdgeVector::const_iterator tmpi = ri;
                NBContHelper::nextCCW(&(myNode._allEdges), tmpi);
                double angle =
                    myNode.getCCWAngleDiff(
                        ccb.lineAt(0).atan2DegreeAngle(),
                        (*tmpi)->getCWBounderyLine(myNode, 2.5).lineAt(0).atan2DegreeAngle());
                if(angle<90) {
                    ccb = (*ri)->getCCWBounderyLine(myNode, 2.5);
                    ccw = (*ri)->width();
                }
                NBContHelper::nextCCW(&(myNode._allEdges), ri);
            }
            NBContHelper::nextCW(&(myNode._allEdges), li);
            ccl = (*ri)->getCWBounderyLine(myNode, 2.5);
            cl = (*li)->getCCWBounderyLine(myNode, 2.5);
        }
        double offr, offl, cca, ca;
        if(ri==i) {
            offr = 4;
            cca = 360;
        } else {
            offr = myNode.getOffset(ccb, ccl);
            double a1 = ccb.lineAt(0).atan2DegreeAngle();
            double a2 = ccl.lineAt(0).atan2DegreeAngle();
            cca = myNode.getCCWAngleDiff(a1, a2);
            if(cca>180||cca<10) {
                offr = -1;
            }
/*            double lw1 = fabs(sin(cca*3.1415926535897932384626433832795/180.0)*((*ri)->width()*1.5));
            double lw2 = fabs(sin(cca*3.1415926535897932384626433832795/180.0)*(ccw*1.5));
            if(cca>90&&lw1<fabs(100-offr)&&lw2<fabs(100-offr)) {
                offr = -1;
            }*/

        }
        if(li==i) {
            offl = 4;
            ca = 360;
        } else {
            offl = myNode.getOffset(cb, cl);
            double a1 = cb.lineAt(0).atan2DegreeAngle();
            double a2 = cl.lineAt(0).atan2DegreeAngle();
            ca = myNode.getCWAngleDiff(a1, a2);
            if(ca>180||ca<10) {
                offl = -1;
            }
/*            double lw1 = fabs(sin(ca*3.1415926535897932384626433832795/180.0)*((*li)->width()*1.5));
            double lw2 = fabs(sin(ca*3.1415926535897932384626433832795/180.0)*(cw*1.5));
            if(ca>90&&lw1<fabs(100-offl)&&lw2<fabs(100-offl)) {
                offl = -1;
            }*/

        }
        if(offr==-1&&offl==-1) {
            edgeOffsets.push_back(4);
        } else if(offr==-1||offl==-1) {
            edgeOffsets.push_back(MAX(offr, offl));
        } /*else if(cca!=360-ca) {
            if(cca>ca) {
                edgeOffsets.push_back(offl);
            } else {
                edgeOffsets.push_back(offr);
            }
        } */else {
            edgeOffsets.push_back(MAX(offr, offl));
        }
    }
    std::vector<double>::iterator j;
    for(i=myNode._allEdges.begin(), j=edgeOffsets.begin(); i!=myNode._allEdges.end(); i++, j++) {
        double offset = *j;
        // do not process edges with no crossing
        if(offset<0) {
            continue;
        }
        // do not process outgoing which have opposite incoming for themselves
        EdgeVector::const_iterator li = i;
        NBContHelper::nextCW(&(myNode._allEdges), li);
        NBEdge *current = *i;
        if(myNode.hasIncoming(current)&&current->isTurningDirection(*li)) {
            std::vector<double>::iterator j2 = j+1;
            if(j2==edgeOffsets.end()) {
                j2 = edgeOffsets.begin();
            }
            offset = MAX(offset,(*j2));
            // ok, process both directions
            addCCWPoint(ret, current, offset);
            addCWPoint(ret, (*li), offset);
            // and skip the next one
            if(i+1!=myNode._allEdges.end()) {
                i++;
                j++;
            }
        } else if(myNode.hasOutgoing(current)) {
            EdgeVector::const_iterator ri = i;
            NBContHelper::nextCCW(&(myNode._allEdges), ri);
            // skip outgoing that have an incoming
            if((*ri)->isTurningDirection(current)) {
                continue;
            }
            addCCWPoint(ret, current, offset);
            addCWPoint(ret, current, offset);
        } else {
            // process this edge only
            addCCWPoint(ret, current, offset);
            addCWPoint(ret, current, offset);
        }
    }
    return ret;
}


void
NBNodeShapeComputer::addCCWPoint(Position2DVector &poly,
                                 NBEdge *e, double offset)
{
    Position2DVector l = e->getCCWBounderyLine(myNode, 1.5);
    double len = l.length();
    if(len>=offset) {
        poly.push_back(l.positionAtLengthPosition(offset));
    } else {
        poly.push_back(l.positionAtLengthPosition(len));
    }
}

void
NBNodeShapeComputer::addCWPoint(Position2DVector &poly,
                                NBEdge *e, double offset)
{
    Position2DVector l = e->getCWBounderyLine(myNode, 1.5);
    double len = l.length();
    if(len>=offset) {
        poly.push_back(l.positionAtLengthPosition(offset));
    } else {
        poly.push_back(l.positionAtLengthPosition(len));
    }
}

Position2DVector
NBNodeShapeComputer::computeJoinSplitNodeShape()
{
    Position2DVector ret;
    std::vector<double> edgeOffsets;
    EdgeVector::const_iterator i;
    for(i=myNode._allEdges.begin(); i!=myNode._allEdges.end(); i++) {
        Position2DVector o1 = (*i)->getCCWBounderyLine(myNode, 1.5);
        Position2DVector o2 = (*i)->getCWBounderyLine(myNode, 1.5);
        Position2DVector geom = (*i)->getGeometry();
        geom.extrapolate(1000);
        // build normal
        Line2D incLine;
        if(myNode.hasIncoming(*i)) {
            incLine = Line2D(geom.at(geom.size()-2), geom.at(geom.size()-1));
        } else {
            incLine = Line2D(geom.at(1), geom.at(0));
        }
        double xcenter = (incLine.p1().x() + incLine.p2().x()) / 2.0;
        double ycenter = (incLine.p1().y() + incLine.p2().y()) / 2.0;
        incLine.sub(xcenter, ycenter);
        Line2D extrapolated(
            GeomHelper::extrapolate_first(incLine.p1(), incLine.p2(), 1000),
            GeomHelper::extrapolate_second(incLine.p1(), incLine.p2(), 1000));
        Line2D normal(
            Position2D(extrapolated.p1().y(), extrapolated.p1().x()),
            Position2D(extrapolated.p2().y(), extrapolated.p2().x()));
        normal.add(myNode.getPosition());
        // get cross position
        double pos;
        if(myNode.hasIncoming(*i)) {
            pos = 0;//!!!(*i)->getGeometry().length();
        } else {
            pos = 0;
        }
        if(!geom.intersects(normal.p1(), normal.p2())) {
//            continue;//!!! muss immer gelten, tut's nicht
            ret.push_back(geom.positionAtLengthPosition(pos));
        }

        if(o1.intersects(normal.p1(), normal.p2())) {
            ret.push_back(o1.intersectsAtPoint(normal.p1(), normal.p2()));
        } else {
            int bla = 0;
        }
        if(o2.intersects(normal.p1(), normal.p2())) {
            ret.push_back(o2.intersectsAtPoint(normal.p1(), normal.p2()));
        } else {
            int bla = 0;
        }
//        if(geom.intersects(normal.p1(), normal.p2())) {
        if(!o1.intersects(normal.p1(), normal.p2())
            &&
            !o2.intersects(normal.p1(), normal.p2())
            &&
            geom.intersects(normal.p1(), normal.p2())
            ) {
            assert(geom.intersects(normal.p1(), normal.p2()));
            DoubleVector posses = geom.intersectsAtLengths(normal);
            if(posses.size()==0) {
                int bla = 0;
                posses = geom.intersectsAtLengths(normal);
            }
            if(myNode.hasIncoming(*i)) {
                pos = DoubleVectorHelper::minValue(posses);
            } else {
                pos = DoubleVectorHelper::maxValue(posses);
            }
            // add geometry point
            ret.push_back(
                (*i)->getCCWBounderyLine(myNode, 1.5).positionAtLengthPosition(pos-1000));
            ret.push_back(
                (*i)->getCWBounderyLine(myNode, 1.5).positionAtLengthPosition(pos-1000));
        }
//        myPoly.push_back(geom.positionAtLengthPosition(pos));
    }
    return ret;
}


//double getOffset(Position2DVector on, Position2DVector cross) const;
