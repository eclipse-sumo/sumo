/****************************************************************************/
/// @file    MELoop.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MELoop.h 96 2007-06-06 07:40:46Z behr_mi $
///
// The main mesocopic simulation loop
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
#ifndef MELoop_h
#define MELoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_MESOSIM

#include <queue>
#include <vector>
#include <map>
#include "MESegment.h"
#include "MEVehicle.h"

#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include "Priority.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MELoop
 * @brief The main mesocopic simulation loop
 */
class MELoop {
public:
    /// SUMO constructor
    MELoop() {
    }

    ~MELoop() {
        for(std::vector<MESegment*>::iterator i=segment.begin(); i!=segment.end(); ++i) {
            delete (*i);
        }
    }

    // {
    void simulationStep( SUMOTime start, SUMOTime step) {
        simulate((SUMOReal) STEP2TIME(step));
    }

    inline SUMOReal simulate(SUMOReal tMax) {
        if(cars.size()==0) {
            return -1;
        }
        //!!!0 MEVehicle *tz = cars.top();
        SUMOReal time = cars.topTime(); //!!!0 tz->get_tEvent();
        while(time<=tMax&&cars.size()!=0) {
            std::vector<MEVehicle*> vehs = cars.top();
            //!!!0 MEVehicle *veh = cars.top();
            //!!!0 veh->inserted = false;
            cars.pop();
            for(std::vector<MEVehicle*>::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
                (*i)->inserted = false;
                checkCar2((*i), tMax);
            }
            if(!cars.isEmpty()) {
                time = cars.topTime();
            } else {
                time = tMax + (SUMOReal) .1;
            }
        }
        return time;
    }

    inline void checkCar2(MEVehicle *veh, SUMOReal tMax)
    {
        assert(MSNet::getInstance()->getVehicleControl().getVehicle(veh->getSUMOVehicle()->getID())!=0);
        MESegment *on_segment = veh->at_segment();
        if(on_segment==0) {
            // !!!
            return ;
        }
        MESegment *to_segment = next_segment(on_segment, veh);
        assert(MSNet::getInstance()->getVehicleControl().getVehicle(veh->getSUMOVehicle()->getID())!=0);
        if(to_segment==0) {
            MEVehicle *next = on_segment->removeCar2(veh);
            assert(next!=veh);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(static_cast<MSVehicle*>(veh));
            if(next!=0) {
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(next->getSUMOVehicle()->getID())!=0);
                cars.add(next, next->get_tEvent());
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(next->getSUMOVehicle()->getID())!=0);
            }
            return ;
        }
        if(to_segment->get_state()!=Full) {
            MEVehicle *next = on_segment->removeCar2(veh);
            assert(next!=veh);
            if(next!=0) {
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(next->getSUMOVehicle()->getID())!=0);
                cars.add(next, next->get_tEvent());
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(next->getSUMOVehicle()->getID())!=0);
            }
            SUMOReal time = veh->get_tEvent();
            MEVehicle *toRemove = 0;
            MEVehicle *add = to_segment->receive2(veh, time, &toRemove);
            if(add!=0) {
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(add->getSUMOVehicle()->getID())!=0);
                cars.add(add, add->get_tEvent());
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(add->getSUMOVehicle()->getID())!=0);
            }
            if(toRemove!=0) {
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(toRemove->getSUMOVehicle()->getID())!=0);
                if(cars.erase(toRemove, toRemove->get_tEvent())) {
                    toRemove->inserted = false;
                }
            }
        } else {
            if(veh->getTLastEntry()<tMax-2400) {
                // standing in a jam for 300s
                MEVehicle *next = on_segment->removeCar2(veh);
                assert(next!=veh);
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(veh->getSUMOVehicle()->getID())!=0);
                MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(static_cast<MSVehicle*>(veh));
                if(next!=0) {
                    assert(MSNet::getInstance()->getVehicleControl().getVehicle(next->getSUMOVehicle()->getID())!=0);
                    cars.add(next, next->get_tEvent());
                    assert(MSNet::getInstance()->getVehicleControl().getVehicle(next->getSUMOVehicle()->getID())!=0);
                }
            } else {
                //veh->update_tEvent(MAX2(to_segment->getFirstTEvent2(veh) + on_segment->getTauJJ(), veh->get_tEvent()));
                SUMOTime ttt = MSNet::getInstance()->getCurrentTimeStep();
                veh->update_tEvent(MAX2((SUMOReal) ttt+2, veh->get_tEvent()));
                cars.add(veh, veh->get_tEvent());
                assert(MSNet::getInstance()->getVehicleControl().getVehicle(veh->getSUMOVehicle()->getID())!=0);
            }
        }
    }


    inline MEVehicle *getUnboundNext(MEVehicle *predec, SUMOReal time, MESegment *from) {
        if(from->cars.size()<2||from->myLastCars.size()==0) {
            return 0;
        }
        MESegment *predecNext = next_segment(predec->at_segment(), predec);
        std::map<MSEdge*, std::vector<MEVehicle*> >::iterator i;
        for(i=from->myLastCars.begin(); i!=from->myLastCars.end(); ++i) {
            if( (*i).first!=predecNext->getEdge() ) {
                if( (*i).second.size()!=0 ) {
                    MEVehicle *possNext = (*i).second.back();
                    if(!possNext->inserted && possNext->get_tEvent()<time) {
                        return possNext;
                    }
                }
            }
        }
        return 0;
    }


    inline void addCar(MEVehicle *v) {
        assert(!v->inserted);
        cars.add(v, v->get_tEvent());
        v->inserted = true;
    }

    /// Retrieve next segment; SUMO-method
    inline MESegment *next_segment(MESegment *s, MEVehicle *v) {
        MESegment *next = s->getNextSegment();
        if(next!=0) {
            // ok, the street continues
            return next;
        }
        // we have to check the next edge in the vehicle's route
        MSVehicle * const veh = v->getSUMOVehicle();
        MSEdge* nextEdge = (MSEdge*) veh->succEdge(1);
            assert(myEdges2FirstSegments.find(nextEdge)!=myEdges2FirstSegments.end());
            return myEdges2FirstSegments[nextEdge];
    }

    /// Build the segments for a given edge
    inline MESegment *buildSegmentsFor(const MSEdge * const e, MSNet &net,
        const OptionsCont &oc) {
        unsigned int no = (unsigned int) ((*e->getLanes())[0]->length() / oc.getFloat("mesosim-edgelength"));
        if(no==0) { // assure there is at least one segment
            no = 1;
        }
        // build list of next
        std::vector<MSEdge*> next = e->getFollowingEdges();
        //
        MESegment *new_segment = 0;
        MESegment *nextSegment = 0;
        for(unsigned int s=0; s<no; s++) {
            std::string id = e->getID() + ":" + toString(s);
            new_segment =
                new MESegment(net, id,
                    e, nextSegment, oc.getFloat("mesosim-edgelength"),
                    (SUMOReal) (*(e->getLanes()))[0]->maxSpeed(),
                    segment_tauff, segment_taufj,
                    segment_taujf, segment_taujj, s, oc.getFloat("meso-lanescale"),
                    next);
            next.clear();
            segment.push_back(new_segment);
            nextSegment = new_segment;
        }
        myEdges2FirstSegments[e] = new_segment;
        return new_segment;
    }

    inline MESegment *first_segment() {
        return segment[0];
    }

    void insertMeanData(size_t no) {
        for(std::vector<MESegment*>::iterator i=segment.begin(); i!=segment.end(); ++i) {
            (*i)->addMeanData(no);
        }
    }

    MESegment *getSegmentForEdge(const MSEdge * const e) {
        return myEdges2FirstSegments[e];
    }

    void eraseCar(MEVehicle*v) {
        cars.erase(v, v->get_tEvent()); // may fail!
        assert(MSNet::getInstance()->getVehicleControl().getVehicle(v->getSUMOVehicle()->getID())!=0);
        MESegment *s = v->at_segment();
        assert(s!=0);
        MEVehicle *next = s->removeCar2(v, false);
        assert(MSNet::getInstance()->getVehicleControl().getVehicle(v->getSUMOVehicle()->getID())!=0);
        assert(next!=v);
        if(next!=0) {
            cars.add(next, next->get_tEvent());
        }
        assert(MSNet::getInstance()->getVehicleControl().getVehicle(v->getSUMOVehicle()->getID())!=0);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(static_cast<MSVehicle*>(v));
    }


    void setSpeed(MESegment *s, SUMOReal speed_) {
        if(s->speed==speed_) {
            return;
        }
        s->speed = speed_;
        if(s->cars.size()!=0) {
            int ccar = s->cars.size()-1;
            for(std::vector<MEVehicle*>::iterator i=s->cars.begin(); i!=s->cars.end()-1; ++i, --ccar) {
                SUMOReal newEvent = 0;
                SUMOReal vspeed = s->length / (*i)->getNeededTime();
                if(vspeed>speed_) {
                    newEvent = (SUMOReal)
                        MAX2(((*i)->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), (*i)->get_tEvent());
                } else {
                    newEvent = (SUMOReal)
                        MIN2(((*i)->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), (*i)->get_tEvent());
                }
                if(newEvent==(*i)->get_tEvent()) {
                    continue;
                }
                (*i)->update_tEvent(newEvent);
            }
            MEVehicle *v = s->cars.back();
            SUMOReal newEvent = 0;
            SUMOReal vspeed = s->length / v->getNeededTime();
            if(vspeed>speed_) {
                newEvent = (SUMOReal)
                    MAX2((v->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), v->get_tEvent());
            } else {
                newEvent = (SUMOReal)
                    MIN2((v->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), v->get_tEvent());
            }
            if(newEvent!=v->get_tEvent()) {
                if(cars.remap(v, v->get_tEvent(), newEvent)) {
                    v->update_tEvent(newEvent);
                }
            }
        }
        if(s->myLastCars.size()!=0) {
            std::map<MSEdge*, std::vector<MEVehicle*> >::const_iterator i;
            for(i=s->myLastCars.begin(); i!=s->myLastCars.end(); ++i) {
                const std::vector<MEVehicle*> &vehs = (*i).second;
                if(vehs.size()==0) {
                    continue;
                }
                int ccar = vehs.size()-1;
                for(std::vector<MEVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end()-1; ++j, --ccar) {
                    SUMOReal newEvent = 0;
                    SUMOReal vspeed = s->length / (*j)->getNeededTime();
                    if(vspeed>speed_) {
                        newEvent = (SUMOReal)
                            MAX2(((*j)->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), (*j)->get_tEvent());
                    } else {
                        newEvent = (SUMOReal)
                            MIN2(((*j)->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), (*j)->get_tEvent());
                    }
                    if(newEvent==(*j)->get_tEvent()) {
                        continue;
                    }
                    (*j)->update_tEvent(newEvent);
                }
                MEVehicle *v = vehs.back();
                SUMOReal newEvent = 0;
                SUMOReal vspeed = s->length / v->getNeededTime();
                if(vspeed>speed_) {
                    newEvent = (SUMOReal)
                        MAX2((v->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), v->get_tEvent());
                } else {
                    newEvent = (SUMOReal)
                        MIN2((v->getTLastEntry() + s->length / speed_) + (s->getTauFF() * (float) ccar), v->get_tEvent());
                }   
                if(newEvent!=v->get_tEvent()) {
                    if(cars.remap(v, v->get_tEvent(), newEvent)) {
                        v->update_tEvent(newEvent);
                    }
                }
            }
        }
    }


    void saveState(std::ostream &os, long what) {
        for(std::vector<MESegment*>::iterator j=segment.begin(); j!=segment.end(); ++j) {
            (*j)->saveState(os, what);
        }
    }

    void loadState(BinaryInputDevice &bis, long what, MSVehicleControl &vc) {
        for(std::vector<MESegment*>::iterator j=segment.begin(); j!=segment.end(); ++j) {
            std::vector<MEVehicle*> firstCars = (*j)->loadState(bis, what, vc);
            for(std::vector<MEVehicle*>::iterator i=firstCars.begin(); i!=firstCars.end(); ++i) {
                cars.add((*i), (*i)->get_tEvent());
            }
        }
    }

private:
    std::vector<MESegment*> segment;
    std::vector<MEVehicle*> myToErase;

    class CarComp {
    public:
        bool operator()(MEVehicle *c1, MEVehicle *c2) {
            assert(c1!=c2);
            return c1->get_tEvent()>c2->get_tEvent();
        }

    };

    Priority<MEVehicle> cars;

    std::map<const MSEdge*, MESegment*> myEdges2FirstSegments;

};


#endif // HAVE_MESOSIM

#endif

/****************************************************************************/

