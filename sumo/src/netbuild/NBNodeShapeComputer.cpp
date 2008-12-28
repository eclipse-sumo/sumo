/****************************************************************************/
/// @file    NBNodeShapeComputer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// This class computes shapes of junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/geom/Position2DVector.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/StdDefs.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBNode.h"
#include "NBNodeShapeComputer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBNodeShapeComputer::NBNodeShapeComputer(const NBNode &node)
        : myNode(node)
{}


NBNodeShapeComputer::~NBNodeShapeComputer()
{}


Position2DVector
NBNodeShapeComputer::compute()
{
    Position2DVector ret;
    bool isDeadEnd = false;
    if (myNode.myAllEdges.size()==1) {
        isDeadEnd = true;
    }
    if (myNode.myAllEdges.size()==2&&myNode.getIncomingEdges().size()==1) {
        if (myNode.getIncomingEdges()[0]->isTurningDirectionAt(&myNode, myNode.getOutgoingEdges()[0])) {
            isDeadEnd = true;
        }
    }
    if (isDeadEnd) {
        ret = computeNodeShapeByCrosses();
        return ret;
    }


    bool simpleContinuation = myNode.isSimpleContinuation();
    ret = computeContinuationNodeShape(simpleContinuation);
    // add the geometry of internal lanes
    if (!OptionsCont::getOptions().getBool("no-internal-links")) {
        addInternalGeometry();
    }
    if (ret.size()<3) {
        ret = computeNodeShapeByCrosses();
    }
    if (OptionsCont::getOptions().isSet("node-geometry-dump")) {
        for (int i=0; i<(int) ret.size(); ++i) {
            OutputDevice::getDeviceByOption("node-geometry-dump")
            << "   <poi id=\"end_" << myNode.getID() << "_"
            << i << "\" type=\"nodeshape.end\" color=\"1,0,1\""
            << " x=\"" << ret[i].x() << "\" y=\"" << ret[i].y() << "\"/>\n";
        }
    }
    return ret;
}


void
NBNodeShapeComputer::addInternalGeometry()
{
    // !!!
}


void
computeSameEnd(Position2DVector& l1, Position2DVector &l2)
{
    Line2D sub(l1.lineAt(0).getPositionAtDistance(100), l1[1]);
    Line2D tmp(sub);
    tmp.rotateDegAtP1(90);
    tmp.extrapolateBy(100);
    if (l1.intersects(tmp.p1(), tmp.p2())) {
        SUMOReal offset1 = l1.intersectsAtLengths(tmp)[0];
        Line2D tl1 = Line2D(
                         l1.lineAt(0).getPositionAtDistance(offset1),
                         l1[1]);
        tl1.extrapolateBy(100);
        l1.replaceAt(0, tl1.p1());
    }
    if (l2.intersects(tmp.p1(), tmp.p2())) {
        SUMOReal offset2 = l2.intersectsAtLengths(tmp)[0];
        Line2D tl2 = Line2D(
                         l2.lineAt(0).getPositionAtDistance(offset2),
                         l2[1]);
        tl2.extrapolateBy(100);
        l2.replaceAt(0, tl2.p1());
    }
}


void
NBNodeShapeComputer::replaceLastChecking(Position2DVector &g, bool decenter,
        Position2DVector counter,
        size_t counterLanes, SUMOReal counterDist,
        int laneDiff)
{
    counter.extrapolate(100);
    Position2D counterPos = counter.positionAtLengthPosition(counterDist);
    Position2DVector t = g;
    t.extrapolate(100);
    SUMOReal p = t.nearest_position_on_line_to_point(counterPos);
    if (p>=0) {
        counterPos = t.positionAtLengthPosition(p);
    }
    if (GeomHelper::distance(g[-1], counterPos)<SUMO_const_laneWidth*(SUMOReal) counterLanes) {
        g.replaceAt(g.size()-1, counterPos);
    } else {
        g.push_back_noDoublePos(counterPos);
    }
    if (decenter) {
        Line2D l(g[-2], g[-1]);
        SUMOReal factor = laneDiff%2!=0 ? SUMO_const_halfLaneAndOffset : SUMO_const_laneWidthAndOffset;
        l.move2side(-factor);//SUMO_const_laneWidthAndOffset);
        g.replaceAt(g.size()-1, l.p2());
    }
}


void
NBNodeShapeComputer::replaceFirstChecking(Position2DVector &g, bool decenter,
        Position2DVector counter,
        size_t counterLanes, SUMOReal counterDist,
        int laneDiff)
{
    counter.extrapolate(100);
    Position2D counterPos = counter.positionAtLengthPosition(counterDist);
    Position2DVector t = g;
    t.extrapolate(100);
    SUMOReal p = t.nearest_position_on_line_to_point(counterPos);
    if (p>=0) {
        counterPos = t.positionAtLengthPosition(p);
    }
    if (GeomHelper::distance(g[0], counterPos)<SUMO_const_laneWidth*(SUMOReal) counterLanes) {
        g.replaceAt(0, counterPos);
    } else {
        g.push_front_noDoublePos(counterPos);
    }
    if (decenter) {
        Line2D l(g[0], g[1]);
        SUMOReal factor = laneDiff%2!=0 ? SUMO_const_halfLaneAndOffset : SUMO_const_laneWidthAndOffset;
        /*
            SUMO_const_laneWidthAndOffset * (SUMOReal) (counterLanes-1)
            + SUMO_const_halfLaneAndOffset * (SUMOReal) (counterLanes%2);
            */
        l.move2side(-factor);//SUMO_const_laneWidthAndOffset);
        g.replaceAt(0, l.p1());
    }
}



Position2DVector
NBNodeShapeComputer::computeContinuationNodeShape(bool simpleContinuation)
{
    // if we have less than two edges, we can not compute the node's shape this way
    if (myNode.myAllEdges.size()<2) {
        return Position2DVector();
    }

    // initialise
    EdgeVector::const_iterator i;
    // edges located in the value-vector have the same direction as the key edge
    std::map<NBEdge*, std::vector<NBEdge*> > same;
    // the counter-clockwise boundary of the edge regarding possible same-direction edges
    std::map<NBEdge*, Position2DVector> geomsCCW;
    // the clockwise boundary of the edge regarding possible same-direction edges
    std::map<NBEdge*, Position2DVector> geomsCW;
    // store relationships
    std::map<NBEdge*, NBEdge*> ccwBoundary;
    std::map<NBEdge*, NBEdge*> cwBoundary;
    for (i=myNode.myAllEdges.begin(); i!=myNode.myAllEdges.end(); i++) {
        cwBoundary[*i] = *i;
        ccwBoundary[*i] = *i;
    }

    // check which edges are parallel
    joinSameDirectionEdges(same, geomsCCW, geomsCW);

    // compute unique direction list
    std::vector<NBEdge*> newAll = computeUniqueDirectionList(same, geomsCCW, geomsCW, ccwBoundary, cwBoundary);

    // if we have only two "directions", let's not compute the geometry using this method
    if (newAll.size()<2) {
        return Position2DVector();
    }

    // combine all geoms
    std::map<NBEdge*, bool> myExtended;
    std::map<NBEdge*, SUMOReal> distances;
    for (i=newAll.begin(); i!=newAll.end(); ++i) {
        EdgeVector::const_iterator cwi = i;
        cwi++;
        if (cwi==newAll.end()) {
            cwi = newAll.begin();
        }
        EdgeVector::const_iterator ccwi = i;
        if (ccwi==newAll.begin()) {
            ccwi = newAll.end() - 1;
        } else {
            ccwi--;
        }

        assert(geomsCCW.find(*i)!=geomsCCW.end());
        assert(geomsCW.find(*ccwi)!=geomsCW.end());
        assert(geomsCW.find(*cwi)!=geomsCW.end());
        SUMOReal twoPI = (SUMOReal)(2.*3.1415926535897);
        SUMOReal pi = (SUMOReal) 3.1415926535897;
        SUMOReal angleI = geomsCCW[*i].lineAt(0).atan2PositiveAngle();
        SUMOReal angleCCW = geomsCW[*ccwi].lineAt(0).atan2PositiveAngle();
        SUMOReal angleCW = geomsCW[*cwi].lineAt(0).atan2PositiveAngle();
        SUMOReal ccad;
        SUMOReal cad;
        if (angleI>angleCCW) {
            ccad = angleI - angleCCW;
        } else {
            ccad = twoPI - angleCCW + angleI;
        }

        if (angleI>angleCW) {
            cad = twoPI - angleI + angleCW;
        } else {
            cad = angleCW - angleI;
        }

        if (ccad<0) {
            ccad = (SUMOReal) 3.1415926535897 * (SUMOReal) 2.0 + ccad;
        }
        if (ccad>(SUMOReal)(2.*3.1415926535897)) {
            ccad -= (SUMOReal)(2.*3.1415926535897);
        }
        if (cad<0) {
            cad = (SUMOReal) 3.1415926535897 * (SUMOReal) 2.0 + cad;
        }
        if (cad>(SUMOReal)(2.*3.1415926535897)) {
            cad -= (SUMOReal)(2.*3.1415926535897);
        }

        if (simpleContinuation&&ccad<(SUMOReal)(45./180.*PI)) {
            ccad += twoPI;
        }
        if (simpleContinuation&&cad<(SUMOReal)(45./180.*PI)) {
            cad += twoPI;
        }

        if (fabs(ccad-cad)<(SUMOReal) 0.1&&*cwi==*ccwi) {
            // compute the mean position between both edges ends ...
            Position2D p;
            if (myExtended.find(*ccwi)!=myExtended.end()) {
                p = geomsCCW[*ccwi][0];
                p.add(geomsCW[*ccwi][0]);
                p.mul(0.5);
            } else {
                p = geomsCCW[*ccwi][0];
                p.add(geomsCW[*ccwi][0]);
                p.add(geomsCCW[*i][0]);
                p.add(geomsCW[*i][0]);
                p.mul(0.25);
            }
            // ... compute the distance to this point ...
            SUMOReal dist = geomsCCW[*i].nearest_position_on_line_to_point(p);
            if (dist<0) {
                // ok, we have the problem that even the extrapolated geometry
                //  does not reach the point
                // in this case, the geometry has to be extenden... too bad ...
                // ... let's append the mean position to the geometry
                Position2DVector g = (*i)->getGeometry();
                if (myNode.hasIncoming(*i)) {
                    g.push_back_noDoublePos(p);
                } else {
                    g.push_front_noDoublePos(p);
                }
                (*i)->setGeometry(g);
                // and rebuild previous information
                geomsCCW[*i] = (*i)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
                geomsCCW[*i].extrapolate(100);
                geomsCW[*i] = (*i)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
                geomsCW[*i].extrapolate(100);
                // the distance is now = zero (the point we have appended)
                distances[*i] = 100;
                myExtended[*i] = true;
            } else {
                if (!simpleContinuation) {
                    // let us put some geometry stuff into it
                    dist = (SUMOReal) 1.5 + dist;
                }
                distances[*i] = dist;
            }

        } else {
            if (ccad>(90.+70.)/180.*pi&&cad>(90.+70.)/180.*PI&&*ccwi!=*cwi/*&&cwBoundary[*i]==ccwBoundary[*i]*/) {
                // ok, in this case we have a street which is opposite to at least
                //  two edges which have a very similar angle
                // let's skip the computation for now, because we want the current
                //  edge to be almost at these edges' crossing point and maybe
                //  one of the edges' crossings is not yet computed...
                continue;
            } else {
                if (ccad<cad) {
                    if (!simpleContinuation) {
                        if (geomsCCW[*i].intersects(geomsCW[*ccwi])) {
                            distances[*i] = (SUMOReal) 1.5 + geomsCCW[*i].intersectsAtLengths(geomsCW[*ccwi])[0];
                            if (*cwi!=*ccwi&&geomsCW[*i].intersects(geomsCCW[*cwi])) {
                                SUMOReal a1 = distances[*i];
                                SUMOReal a2 = (SUMOReal) 1.5 + geomsCW[*i].intersectsAtLengths(geomsCCW[*cwi])[0];
                                if (ccad>(SUMOReal)((90.+45.)/180.*pi)&&cad>(SUMOReal)((90.+45.)/180.*PI)) {
                                    SUMOReal mmin = MIN2(distances[*cwi], distances[*ccwi]);
                                    if (mmin>100) {
                                        distances[*i] = (SUMOReal) 5. + (SUMOReal) 100. - (SUMOReal)(mmin-100);  //100 + 1.5;
                                    }
                                } else  if (a2>a1+POSITION_EPS&&a2-a1<(SUMOReal) 10) {
                                    distances[*i] = a2;
                                }
                            }
                        } else {
                            if (*cwi!=*ccwi&&geomsCW[*i].intersects(geomsCCW[*cwi])) {
                                distances[*i] = (SUMOReal) 1.5 + geomsCW[*i].intersectsAtLengths(geomsCCW[*cwi])[0];
                            } else {
                                distances[*i] = (SUMOReal)(100. + 1.5);
                            }
                        }
                    } else {
                        if (geomsCCW[*i].intersects(geomsCW[*ccwi])) {
                            distances[*i] = geomsCCW[*i].intersectsAtLengths(geomsCW[*ccwi])[0];
                        } else {
                            distances[*i] = (SUMOReal) 100.;
                        }
                    }
                } else {
                    if (!simpleContinuation) {
                        if (geomsCW[*i].intersects(geomsCCW[*cwi])) {
                            distances[*i] = (SUMOReal)(1.5 + geomsCW[*i].intersectsAtLengths(geomsCCW[*cwi])[0]);
                            if (*cwi!=*ccwi&&geomsCCW[*i].intersects(geomsCW[*ccwi])) {
                                SUMOReal a1 = distances[*i];
                                SUMOReal a2 = (SUMOReal)(1.5 + geomsCCW[*i].intersectsAtLengths(geomsCW[*ccwi])[0]);
                                if (ccad>(SUMOReal)((90.+45.)/180.*pi)&&cad>(SUMOReal)((90.+45.)/180.*PI)) {
                                    SUMOReal mmin = MIN2(distances[*cwi], distances[*ccwi]);
                                    if (mmin>100) {
                                        distances[*i] = (SUMOReal) 5. + (SUMOReal) 100. - (SUMOReal)(mmin-100);  //100 + 1.5;
                                    }
                                } else if (a2>a1+POSITION_EPS&&a2-a1<(SUMOReal) 10) {
                                    distances[*i] = a2;
                                }
                            }
                        } else {
                            if (*cwi!=*ccwi&&geomsCCW[*i].intersects(geomsCW[*ccwi])) {
                                distances[*i] = (SUMOReal) 1.5 + geomsCCW[*i].intersectsAtLengths(geomsCW[*ccwi])[0];
                            } else {
                                distances[*i] = (SUMOReal)(100. + 1.5);
                            }
                        }
                    } else {
                        if (geomsCW[*i].intersects(geomsCCW[*cwi])) {
                            distances[*i] = geomsCW[*i].intersectsAtLengths(geomsCCW[*cwi])[0];
                        } else {
                            distances[*i] = (SUMOReal) 100;
                        }
                    }
                }
            }
        }
    }

    for (i=newAll.begin(); i!=newAll.end(); ++i) {
        if (distances.find(*i)!=distances.end()) {
            continue;
        }
        EdgeVector::const_iterator cwi = i;
        cwi++;
        if (cwi==newAll.end()) {
            cwi = newAll.begin();
        }
        EdgeVector::const_iterator ccwi = i;
        if (ccwi==newAll.begin()) {
            ccwi = newAll.end() - 1;
        } else {
            ccwi--;
        }

        assert(geomsCW.find(*ccwi)!=geomsCW.end());
        assert(geomsCW.find(*cwi)!=geomsCW.end());
        Position2D p1 = distances.find(*cwi)!=distances.end()&&distances[*cwi]!=-1
                        ? geomsCCW[*cwi].positionAtLengthPosition(distances[*cwi])
                        : geomsCCW[*cwi].positionAtLengthPosition((SUMOReal) -.1);
        Position2D p2 = distances.find(*ccwi)!=distances.end()&&distances[*ccwi]!=-1
                        ? geomsCW[*ccwi].positionAtLengthPosition(distances[*ccwi])
                        : geomsCW[*ccwi].positionAtLengthPosition((SUMOReal) -.1);
        Line2D l(p1, p2);
        l.extrapolateBy(1000);
        SUMOReal angleI = geomsCCW[*i].lineAt(0).atan2PositiveAngle();
        SUMOReal angleCCW = geomsCW[*ccwi].lineAt(0).atan2PositiveAngle();
        SUMOReal angleCW = geomsCW[*cwi].lineAt(0).atan2PositiveAngle();
        SUMOReal ccad;
        SUMOReal cad;
        SUMOReal twoPI = (SUMOReal)(2.*3.1415926535897);
        if (angleI>angleCCW) {
            ccad = angleI - angleCCW;
        } else {
            ccad = twoPI - angleCCW + angleI;
        }

        if (angleI>angleCW) {
            cad = twoPI - angleI + angleCW;
        } else {
            cad = angleCW - angleI;
        }

        if (ccad<0) {
            ccad = (SUMOReal) 3.1415926535897 * (SUMOReal) 2.0 + ccad;
        }
        if (ccad>2.*3.1415926535897) {
            ccad -= (SUMOReal)(2.*3.1415926535897);
        }
        if (cad<0) {
            cad = (SUMOReal) 3.1415926535897 * (SUMOReal) 2.0 + cad;
        }
        if (cad>2.*3.1415926535897) {
            cad -= (SUMOReal)(2.*3.1415926535897);
        }
        SUMOReal offset = 0;
        int laneDiff = (*i)->getNoLanes() - (*ccwi)->getNoLanes();
        if (*ccwi!=*cwi) {
            laneDiff -= (*cwi)->getNoLanes();
        }
        laneDiff = 0;
        if (myNode.hasIncoming(*i)&&(*ccwi)->getNoLanes()%2==1) {
            laneDiff = 1;
        }
        if (myNode.hasOutgoing(*i)&&(*cwi)->getNoLanes()%2==1) {
            laneDiff = 1;
        }

        Position2DVector g = (*i)->getGeometry();
        Position2DVector counter;
        if (myNode.hasIncoming(*i)) {
            if (myNode.hasOutgoing(*ccwi)&&myNode.hasOutgoing(*cwi)) {
                if (distances.find(*cwi)==distances.end()) {
                    return Position2DVector();
                }
                replaceLastChecking(g, (*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                    (*cwi)->getGeometry(), (*cwi)->getNoLanes(), distances[*cwi],
                                    laneDiff);
            } else {
                if (distances.find(*ccwi)==distances.end()) {
                    return Position2DVector();
                }
                counter = (*ccwi)->getGeometry();
                if (myNode.hasIncoming(*ccwi)) {
                    counter = counter.reverse();
                }
                replaceLastChecking(g, (*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                    counter, (*ccwi)->getNoLanes(), distances[*ccwi],
                                    laneDiff);
            }
        } else {
            if (myNode.hasIncoming(*ccwi)&&myNode.hasIncoming(*cwi)) {
                if (distances.find(*ccwi)==distances.end()) {
                    return Position2DVector();
                }
                replaceFirstChecking(g,(*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                     (*ccwi)->getGeometry().reverse(), (*ccwi)->getNoLanes(), distances[*ccwi],
                                     laneDiff);
            } else {
                if (distances.find(*cwi)==distances.end()) {
                    return Position2DVector();
                }
                counter = (*cwi)->getGeometry();
                if (myNode.hasIncoming(*cwi)) {
                    counter = counter.reverse();
                }
                replaceFirstChecking(g,(*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                     counter, (*cwi)->getNoLanes(), distances[*cwi],
                                     laneDiff);
            }
        }
        (*i)->setGeometry(g);

        if (cwBoundary[*i]!=*i) {
            Position2DVector g = cwBoundary[*i]->getGeometry();
            Position2DVector counter = (*cwi)->getGeometry();
            if (myNode.hasIncoming(*cwi)) {
                counter = counter.reverse();
            }
            if (myNode.hasIncoming(cwBoundary[*i])) {
                if (distances.find(*cwi)==distances.end()) {
                    return Position2DVector();
                }
                replaceLastChecking(g, (*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                    counter, (*cwi)->getNoLanes(), distances[*cwi],
                                    laneDiff);
            } else {
                if (distances.find(*cwi)==distances.end()) {
                    return Position2DVector();
                }
                replaceFirstChecking(g,(*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                     counter, (*cwi)->getNoLanes(), distances[*cwi],
                                     laneDiff);
            }
            cwBoundary[*i]->setGeometry(g);
            myExtended[cwBoundary[*i]] = true;
            geomsCW[*i] = cwBoundary[*i]->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
        } else {
            geomsCW[*i] = (*i)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);

        }

        geomsCW[*i].extrapolate(100);

        if (ccwBoundary[*i]!=*i) {
            Position2DVector g = ccwBoundary[*i]->getGeometry();
            Position2DVector counter = (*ccwi)->getGeometry();
            if (myNode.hasIncoming(*ccwi)) {
                counter = counter.reverse();
            }
            if (myNode.hasIncoming(ccwBoundary[*i])) {
                if (distances.find(*ccwi)==distances.end()) {
                    return Position2DVector();
                }
                replaceLastChecking(g, (*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                    counter, (*ccwi)->getNoLanes(), distances[*ccwi],
                                    laneDiff);
            } else {
                if (distances.find(*cwi)==distances.end()) {
                    return Position2DVector();
                }
                replaceFirstChecking(g,(*i)->getLaneSpreadFunction()==NBEdge::LANESPREAD_CENTER,
                                     counter, (*cwi)->getNoLanes(), distances[*cwi],
                                     laneDiff);
            }
            ccwBoundary[*i]->setGeometry(g);
            myExtended[ccwBoundary[*i]] = true;
            geomsCCW[*i] = ccwBoundary[*i]->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
        } else {
            geomsCCW[*i] = (*i)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);

        }
        geomsCCW[*i].extrapolate(100);

        computeSameEnd(geomsCW[*i], geomsCCW[*i]);

        // and rebuild previous information
        if (((*cwi)->getNoLanes()+(*ccwi)->getNoLanes())>(*i)->getNoLanes()) {
            offset = 5;
        }
        if (ccwBoundary[*i]!=cwBoundary[*i]) {
            offset = 5;
        }

        myExtended[*i] = true;
        distances[*i] = 100 + offset;
    }

    // build
    Position2DVector ret;
    for (i=newAll.begin(); i!=newAll.end(); ++i) {
        Position2DVector l = geomsCCW[*i];
        SUMOReal len = l.length();
        SUMOReal offset = distances[*i];
        if (offset==-1) {
            offset = (SUMOReal) -.1;
        }
        Position2D p;
        if (len>=offset) {
            p = l.positionAtLengthPosition(offset);
        } else {
            p = l.positionAtLengthPosition(len);
        }
        ret.push_back_noDoublePos(p);
        //
        l = geomsCW[*i];
        len = l.length();
        if (len>=offset) {
            p = l.positionAtLengthPosition(offset);
        } else {
            p = l.positionAtLengthPosition(len);
        }
        ret.push_back_noDoublePos(p);
    }
    return ret;
}



void
NBNodeShapeComputer::joinSameDirectionEdges(std::map<NBEdge*, std::vector<NBEdge*> > &same,
        std::map<NBEdge*, Position2DVector> &geomsCCW,
        std::map<NBEdge*, Position2DVector> &geomsCW)
{
    EdgeVector::const_iterator i, j;
    for (i=myNode.myAllEdges.begin(); i!=myNode.myAllEdges.end()-1; i++) {
        // store current edge's boundary as current ccw/cw boundary
        geomsCCW[*i] = (*i)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
        geomsCW[*i] = (*i)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
        // extend the boundary by extroplating it by 100m
        Position2DVector g1 =
            myNode.hasIncoming(*i)
            ? (*i)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth)
            : (*i)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
        Line2D l1 = g1.lineAt(0);
        Line2D tmp = geomsCCW[*i].lineAt(0);
        tmp.extrapolateBy(100);
        geomsCCW[*i].replaceAt(0, tmp.p1());
        tmp = geomsCW[*i].lineAt(0);
        tmp.extrapolateBy(100);
        geomsCW[*i].replaceAt(0, tmp.p1());
        //
        for (j=i+1; j!=myNode.myAllEdges.end(); j++) {
            geomsCCW[*j] = (*j)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
            geomsCW[*j] = (*j)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
            Position2DVector g2 =
                myNode.hasIncoming(*j)
                ? (*j)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth)
                : (*j)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth);
            Line2D l2 = g2.lineAt(0);
            tmp = geomsCCW[*j].lineAt(0);
            tmp.extrapolateBy(100);
            geomsCCW[*j].replaceAt(0, tmp.p1());
            tmp = geomsCW[*j].lineAt(0);
            tmp.extrapolateBy(100);
            geomsCW[*j].replaceAt(0, tmp.p1());
            if (fabs(l1.atan2DegreeAngle()-l2.atan2DegreeAngle())<1) {
                if (same.find(*i)==same.end()) {
                    same[*i] = std::vector<NBEdge*>();
                }
                if (same.find(*j)==same.end()) {
                    same[*j] = std::vector<NBEdge*>();
                }
                if (find(same[*i].begin(), same[*i].end(), *j)==same[*i].end()) {
                    same[*i].push_back(*j);
                }
                if (find(same[*j].begin(), same[*j].end(), *i)==same[*j].end()) {
                    same[*j].push_back(*i);
                }
            }
        }
    }
}


std::vector<NBEdge*>
NBNodeShapeComputer::computeUniqueDirectionList(
    const std::map<NBEdge*, std::vector<NBEdge*> > &same,
    std::map<NBEdge*, Position2DVector> &geomsCCW,
    std::map<NBEdge*, Position2DVector> &geomsCW,
    std::map<NBEdge*, NBEdge*> &ccwBoundary,
    std::map<NBEdge*, NBEdge*> &cwBoundary)
{
    std::vector<NBEdge*> newAll = myNode.myAllEdges;
    EdgeVector::const_iterator j;
    EdgeVector::iterator i2;
    std::map<NBEdge*, std::vector<NBEdge*> >::iterator k;
    bool changed = true;
    while (changed) {
        changed = false;
        for (i2=newAll.begin(); !changed&&i2!=newAll.end();) {
            std::vector<NBEdge*> other;
            if (same.find(*i2)!=same.end()) {
                other = same.find(*i2)->second;
            }
            for (j=other.begin(); j!=other.end(); ++j) {
                std::vector<NBEdge*>::iterator k =
                    find(newAll.begin(), newAll.end(), *j);
                if (k!=newAll.end()) {
                    if (myNode.hasIncoming(*i2)) {
                        if (myNode.hasIncoming(*j)) {} else {
                            geomsCW[*i2] = geomsCW[*j];
                            cwBoundary[*i2] = *j;
                            computeSameEnd(geomsCW[*i2], geomsCCW[*i2]);
                        }
                    } else {
                        if (myNode.hasIncoming(*j)) {
                            ccwBoundary[*i2] = *j;
                            geomsCCW[*i2] = geomsCCW[*j];
                            computeSameEnd(geomsCW[*i2], geomsCCW[*i2]);
                        } else {}
                    }
                    newAll.erase(k);
                    changed = true;
                }
            }
            if (!changed) {
                ++i2;
            }
        }
    }
    return newAll;
}


Position2DVector
NBNodeShapeComputer::computeNodeShapeByCrosses()
{
    Position2DVector ret;
    EdgeVector::const_iterator i;
    for (i=myNode.myAllEdges.begin(); i!=myNode.myAllEdges.end(); i++) {
        // compute crossing with normal
        {
            Line2D edgebound1 = (*i)->getCCWBoundaryLine(myNode, SUMO_const_halfLaneWidth).lineAt(0);
            Line2D edgebound2 = (*i)->getCWBoundaryLine(myNode, SUMO_const_halfLaneWidth).lineAt(0);
            Line2D cross(edgebound1);
            cross.sub(cross.p1().x(), cross.p1().y());
            cross.rotateAround(Position2D(0, 0), (SUMOReal)(90/180.*3.1415926535897932384626433832795));
            cross.add(myNode.getPosition());
            cross.extrapolateBy(500);
            edgebound1.extrapolateBy(500);
            edgebound2.extrapolateBy(500);
            if (cross.intersects(edgebound1)) {
                ret.push_back_noDoublePos(cross.intersectsAt(edgebound1));
            }
            if (cross.intersects(edgebound2)) {
                ret.push_back_noDoublePos(cross.intersectsAt(edgebound2));
            }
        }
    }
    if (OptionsCont::getOptions().isSet("node-geometry-dump")) {
        for (int i=0; i<(int) ret.size(); ++i) {
            OutputDevice::getDeviceByOption("node-geometry-dump")
            << "   <poi id=\"cross1_" << myNode.getID() << "_" << i
            << "\" type=\"nodeshape.cross1\" color=\"0,0,1\""
            << " x=\"" << ret[i].x() << "\" y=\"" << ret[i].y() << "\"/>\n";
        }
    }
    return ret;
}



/****************************************************************************/

