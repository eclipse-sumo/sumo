/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/importio/LineReader.h>
#include <cassert>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


using namespace std;

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)



// mq-name -> idindex
std::map<std::string, std::string> idmap;

// det->det pos
std::map<std::string, SUMOReal> det2pos;
// det->det lane
std::map<std::string, int> det2lane;

// edges approaching the key edge
std::map<std::string, std::vector<std::string> > approaching;
// edges approached by key edge
std::map<std::string, std::vector<std::string> > approached;

// edge->mapped edge
std::map<std::string, std::string> edgemap;
// edge->speed
std::map<std::string, SUMOReal> speedmap;
// edge->lanes
std::map<std::string, int> lanemap;
// edge->length
std::map<std::string, SUMOReal> lengthmap;


// det -> edge
std::map<std::string, std::string> detmap;
// edge -> det
std::map<std::string, std::string> edge2det;


// sources (det -> edge)
std::map<std::string, std::string> sources;
// destinations (det -> edge)
std::map<std::string, std::string> destinations;
// hsources (det -> edge)
std::map<std::string, std::string> hsources;


// done edges
std::map<std::string, bool> done;


// overall emitted vehicles for detectors
std::map<std::string, int> overallMeso;
std::map<std::string, int> overallMicro;

int beginTime, endTime;


struct FlowDef {
    SUMOReal qKFZ;
    SUMOReal qLKW;
    SUMOReal vLKW;
    SUMOReal vPKW;
    int time;
    std::string det;
    SUMOReal isLKW;
};

struct RouteDesc {
    std::vector<std::string> edges2Pass;
    string routename;
//    std::string det;
    std::vector<SUMOReal> q; // [prob] (0-1440)
    std::vector<SUMOReal> fLKW; // [qLKW/qKFZ] (0-1440)
    std::vector<SUMOReal> isLKW; // [isLKW] (0-1440)
    SUMOReal duration;
};

// detector -> [flow definition] (0-1440)
std::map<std::string, std::vector<FlowDef> > myFlows;

// routes
std::vector<RouteDesc> myRoutes;


bool
isSource(const std::string &edgeid,
         std::vector<std::string> seen,
         const std::string &detid)
{
    if(approaching.find(edgeid)==approaching.end()||approaching[edgeid].size()==0) {
        return true;
    }
    if(speedmap[edgeid]<19.4 &&
        (edge2det.find(edgeid)==edge2det.end() || edge2det[edgeid]!=detid) ) {
        return true;
    }
    if(edge2det.find(edgeid)!=edge2det.end() && detmap[detid]!=edgeid) {
        return false;
    }
    const std::vector<std::string> &appr
        = approaching[edgeid];
    bool isall = true;
    seen.push_back(edgeid);
    for(int i=0; i<appr.size()&&isall; i++) {
        //printf("checking %s->\n", appr[i].c_str());
        bool had =
            find(seen.begin(), seen.end(), appr[i])!=seen.end();
        if(!had) {
            if(!isSource(appr[i], seen, detid)) {
                isall = false;
            }
            //printf("<-yes ");
        } else {
            //printf("<-no ");
        }
        //isall ? printf(" YES\n") : printf(" NO\n");
    }
    return isall;
}

bool
isHighwaySource(const std::string &edgeid,
                std::vector<std::string> seen,
                const std::string &detid)
{
    if(approaching.find(edgeid)==approaching.end()||approaching[edgeid].size()==0) {
        return true;
    }
    if(speedmap[edgeid]<19.4) {
        return false;
    }
    const std::vector<std::string> &appr
        = approaching[edgeid];
    bool isall = true;
    seen.push_back(edgeid);
    for(int i=0; i<appr.size()&&isall; i++) {
        //printf("checking %s->\n", appr[i].c_str());
        bool had =
            find(seen.begin(), seen.end(), appr[i])!=seen.end();
        if(!had) {
            if(!isHighwaySource(appr[i], seen, detid)) {
                isall = false;
            }
            //printf("<-yes ");
        } else {
            //printf("<-no ");
        }
        //isall ? printf(" YES\n") : printf(" NO\n");
    }
    return isall;
}

bool
isDestination(const std::string &edgeid,
              std::vector<std::string> seen,
              const std::string &detid)
{
    if(approached.find(edgeid)==approached.end()||approached[edgeid].size()==0) {
        return true;
    }
    if(speedmap[edgeid]<19.4 &&
        (edge2det.find(edgeid)==edge2det.end() || edge2det[edgeid]!=detid) ) {
        return true;
    }
    if(edge2det.find(edgeid)!=edge2det.end() && detmap[detid]!=edgeid) {
        return false;
    }
    const std::vector<std::string> &appr
        = approached[edgeid];
    bool isall = true;
    seen.push_back(edgeid);
    for(int i=0; i<appr.size()&&isall; i++) {
        //printf("checking %s->\n", appr[i].c_str());
        bool had =
            find(seen.begin(), seen.end(), appr[i])!=seen.end();
        if(!had) {
            if(!isDestination(appr[i], seen, detid)) {
                isall = false;
            }
            //printf("<-yes ");
        } else {
            //printf("<-no ");
        }
        //isall ? printf(" YES\n") : printf(" NO\n");
    }
    return isall;
}



void
readMap(char *file)
{
    LineReader lr(file);
    if(!lr.good()) {
        cout << "Could not open '" << file << "'." << endl;
        throw 1;
    }
    string edgeid;
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.length()==0) {
            continue;
        }
        StringTokenizer st(line, StringTokenizer::WHITECHARS);
        string edge = st.next();
        SUMOReal length = 0;
        while(st.hasNext()) {
            string tmp = st.next();
            StringTokenizer st2(tmp, ":");
            edgemap[st2.next()] = edge;
        }
    }
}


void
readNet(char *file)
{
    LineReader lr(file);
    if(!lr.good()) {
        cout << "Could not open '" << file << "'." << endl;
        throw 1;
    }
    string edgeid;
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.length()==0) {
            continue;
        }
        if(line.find("cedge id=")!=string::npos) {
            string apprid = line.substr(line.find("id=")+4);
            apprid = apprid.substr(0, apprid.find("\""));
            if(approaching.find(apprid)==approaching.end()) {
                approaching[apprid] = std::vector<std::string>();
            }
            approaching[apprid].push_back(edgeid);

            if(approached.find(apprid)==approached.end()) {
                approached[apprid] = std::vector<std::string>();
            }
            approached[edgeid].push_back(apprid);
        } else {
            if(line.find("edge id=")!=string::npos) {
                edgeid = line.substr(line.find("id=")+4);
                edgeid = edgeid.substr(0, edgeid.find("\""));
                string tmp = line.substr(line.find("Speed=")+7);
                tmp = tmp.substr(0, tmp.find("\""));
                speedmap[edgeid] = TplConvert<char>::_2SUMOReal(tmp.c_str());
                tmp = line.substr(line.find("NoLanes=")+9);
                tmp = tmp.substr(0, tmp.find("\""));
                lanemap[edgeid] = TplConvert<char>::_2int(tmp.c_str());
                tmp = line.substr(line.find("Length=")+8);
                tmp = tmp.substr(0, tmp.find("\""));
                lengthmap[edgeid] = TplConvert<char>::_2SUMOReal(tmp.c_str());
            }
        }
        if(line.find("row-logic")!=string::npos) {
            break;
        }
    }
}

void
readDetectors(char *file)
{
    LineReader lr(file);
    if(!lr.good()) {
        cout << "Could not open '" << file << "'." << endl;
        throw 1;
    }
    string edgeid;
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.length()==0) {
            continue;
        }
        if(line[0]!='#') {
            StringTokenizer st(line, StringTokenizer::WHITECHARS);
            string id = st.next();
            string tab = st.next();
            string desc = st.next();
            string longit = st.next();
            string lat = st.next();
            string redge = st.next();
            if(id!="-1") {
                StringTokenizer st2(desc, ";");
                string name = st2.next();
                string dir = st2.next();
                string bla = st2.next();
                string lane = st2.next();
                string dist = st2.next();
                string edge = edgemap[redge];
                det2pos[id] = TplConvert<char>::_2SUMOReal(dist.c_str());
                det2lane[id] = TplConvert<char>::_2int(lane.c_str()) - 1;
                if(edge=="") {
                    cout << "Warning: could not find edge '" << redge
                        << "' for detector '" << id << "'." << endl;
                }
                detmap[id] = edge;
                edge2det[edge] = id;
            }
        }
    }
}

string
trim(const string &s)
{
    string ret;
    int i=0;
    for(; i<s.length()&&s[i]==' '; ++i);
    for(; i<s.length()&&s[i]!=' '; ++i) {
        ret += s[i];
    }
    return ret;
}

string
rmSpaces(const string &s)
{
    string ret;
    int i=0;
    for(; i<s.length(); ++i) {
        if(s[i]!=' ') {
            ret += s[i];
        }
    }
    return ret;
}


void
readIDMap(char *file)
{
    LineReader lr(file);
    if(!lr.good()) {
        cout << "Could not open '" << file << "'." << endl;
        throw 1;
    }
    string edgeid;
    int time = 0;
    int lineNo = 6;
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.length()==0) {
            continue;
        }
        StringTokenizer st(line, ";");
        st.next();
        st.next();
        st.next();
        st.next();
        st.next();
        string id1 = st.next();
        string id2 = st.next();
        st.next();
        string id3 = st.next();
        string id = id1 + "-" + id2 + "-" + id3;
        idmap[id] = toString(lineNo);
        lineNo++;
    }
}

string
twoPosHourString(int i)
{
    if(i==24) {
        i = 0;
    }
    string ret = toString(i);
    if(ret.length()<2) {
        ret = "0" + ret;
    }
    return ret;
}


void
readFlows(char *f)
{
    int mbeg = beginTime / 4 * 4;
    int mend = endTime / 4 * 4;
    if(mend!=endTime) {
        mend += 4;
    }
    cout << " Reading from " << mbeg << " to " << mend << endl;
    for(int i=mbeg; i<mend; i+=4) {
        string file(f);
        file = file + string("_") + twoPosHourString(i) + string("-") + twoPosHourString(i+4);
        LineReader lr(file);
        if(!lr.good()) {
            cout << "Could not open '" << file << "'." << endl;
            throw 1;
        } else {
            cout << "Parsing '" << file << "'." << endl;
        }
        string edgeid;
        int time = 0;
        while(lr.hasMore()) {
            string line = lr.readLine();
            if(line.length()==0) {
                continue;
            }
            if(line.find("gmtime")!=string::npos) {
                size_t beg = line.find(":");
                beg = line.rfind(" ", beg);
                size_t end = line.find(" ", beg+1);
                string times = line.substr(beg+1, end-beg-1);
                StringTokenizer st(times, ":");
                int hour = TplConvert<char>::_2int(st.next().c_str());
                int minute = TplConvert<char>::_2int(st.next().c_str());
                int second = TplConvert<char>::_2int(st.next().c_str());
                time = hour * 60 + minute;// * 60 + second;
            } else if(line.find("|")!=string::npos&&line.find("-")!=string::npos) {
                StringTokenizer st(line, "|");
                string id = rmSpaces(st.next());
                string qKFZ = st.next();
                string qLKW = st.next();
                string vPKW = st.next();
                string vLKW = st.next();
                string tNetto = st.next();
                string Beleg = st.next();
                string s = st.next();
                string vKFZMittel = st.next();
                string det = idmap[id];

                FlowDef fd;
                fd.isLKW = 0;
                fd.det = det;
                fd.qKFZ = TplConvert<char>::_2int(trim(qKFZ).c_str());
                fd.qLKW = TplConvert<char>::_2int(trim(qLKW).c_str());
                if(fd.qKFZ<0) {
                    fd.qKFZ = 0;
                    fd.qLKW = 0;
                }
                fd.vPKW = TplConvert<char>::_2int(trim(vPKW).c_str());
                fd.vLKW = TplConvert<char>::_2int(trim(vLKW).c_str());
                fd.time = time;
                if(myFlows.find(det)==myFlows.end()) {
                    myFlows[det] = std::vector<FlowDef>();
                }
                if(myFlows[det].size()==0) {
                    myFlows[det].reserve(1440);
                    for(int i=0; i<1440; i++) {
                        FlowDef tmp;
                        tmp.det = det;
                        tmp.isLKW = 0;
                        tmp.qKFZ = 0;
                        tmp.qLKW = 0;
                        tmp.time = i;
                        tmp.vLKW = 0;
                        tmp.vPKW = 0;
                        myFlows[det].push_back(tmp);
                    }
                }
                assert(fd.qKFZ>=0);
                assert(time>=0&&time<1440);
                myFlows[det][time] = fd;
            }
        }
    }
}



void
computeRoutesFor(std::string edgeid, RouteDesc base, int no, std::vector<string> &visited)
{
//    base.edges2Pass.push_back(edgeid);
    std::vector<std::string> appr = approached[edgeid];
    visited.push_back(edgeid);
    if(appr.size()==0) {
        RouteDesc tmp = base;
        tmp.duration += lengthmap[edgeid]/(100./3.6);
        myRoutes.push_back(tmp);
        return;
    }
    std::vector<std::string>::iterator i;
    //string next = appr[0];
    bool cont = true;
    while(cont) {
        cont = false;
        // get fastest next
        string best = "";
        {
            for(i=appr.begin(); i!=appr.end(); i++) {
                string c = (*i);
                if(best=="") {
                    best = c;
                } else {
                    if(speedmap[c]>speedmap[best]) {
                        best = c;
                    } else if(speedmap[c]==speedmap[best] && lanemap[c]>lanemap[best]) {
                        best = c;
                    }
                }
            }
        }

        std::vector<bool> use(appr.size(), true);
        int idx = 0;
        for(i=appr.begin(); i!=appr.end(); i++, idx++) {
            string c = (*i);
            if(find(base.edges2Pass.begin(),  base.edges2Pass.end(), c)!=base.edges2Pass.end()) {
                // already visited ?!
                use[idx] = false;
                continue;
            } else {
                //base.edges2Pass.push_back(c);
                if(edge2det.find(c)!=edge2det.end()) {
                    // we have a detector, let's check what it does
                    string det = edge2det[c];
                    if(sources.find(det)!=sources.end()) {
                        // ups - a source, may be a loop or something, we will do it anyhow
                  //      myRoutes.push_back(base);
                        use[idx] = false;
                        continue;
                    }
                    if(destinations.find(det)!=destinations.end()) {
                        // oha, great, a destination - quit here
                    //    myRoutes.push_back(base);
                        use[idx] = false;
                        continue;
                    }
                    /*
                    if(speedmap[c]<speedmap[best]) {
                        // should be an off-ramp
                        myRoutes.push_back(base);
                        use[idx] = false;
                        continue;
                    } /*else {
                        // a detector in between - let's continue as from begin
                        computeRoutesFor(c, base, 0);
                    }*/
                }/* else if(no<1) {
                    // ok, continue so far - maybe there are some edges in between two detectors
                    computeRoutesFor(c, base, no++);
                }*/
            }
        }
        // get fastest nexts
        string next = "";
        std::vector<std::string> nexts;
        for(i=appr.begin(), idx=0; i!=appr.end(); i++, idx++) {
            string c = (*i);
            if(use[idx]) {
                if(next=="") {
                    next = c;
                } else {
                    if(speedmap[c]>speedmap[next]) {
                        next = c;
                    } else if(speedmap[c]==speedmap[next] && lanemap[c]>lanemap[next]) {
                        next = c;
                    }
                }
            }
        }
        /*
        if(next=="") {
            cont = false;
            continue;
        }
        */
        if(speedmap[next]<19||lanemap[next]<2) {
            next = "";
        }
        for(i=appr.begin(), idx=0; i!=appr.end(); i++, idx++) {
            string c = (*i);
            if(use[idx]&&next!=""&&speedmap[c]==speedmap[next]&&lanemap[c]==lanemap[next]) {
                /*
                cout << "next: " << c << "(" << speedmap[c] << ", "
                    << lanemap[c] << ")" << endl;
                    */
                RouteDesc t = base;
                t.duration += lengthmap[c]/(100./3.6);
                t.edges2Pass.push_back(c);
                computeRoutesFor(c, t, 0, visited);
            } else {
                if(!use[idx]||speedmap[c]!=speedmap[next]||lanemap[c]!=lanemap[next]) {
                    RouteDesc t = base;
                    t.edges2Pass.push_back(c);
                    t.duration += lengthmap[c]/(100./3.6);
                    myRoutes.push_back(t);
                }
            }
        }
    }
}


void
buildRoutesForDetector(const std::string &det)
{
    string src = detmap[det];
    // do not process edges multiple times
    if(done.find(src)==done.end()) {
        done[src] = true;
        string current = src;
        RouteDesc rd;
        rd.edges2Pass.push_back(current);
        rd.duration = 0;
        std::vector<string> visited;
        visited.push_back(current);
        computeRoutesFor(current, rd, 0, visited);
    }
}


class matching_time_finder {
protected:
    int mTime;

public:
    /** constructor */
    matching_time_finder(int time) : mTime(time) { }
    bool operator() (const FlowDef &fd) {
            return fd.time == mTime;
    }

};

FlowDef
getOverallFlow(int time, string det)
{
    if(myFlows.find(det)==myFlows.end()) {
        throw 1;
    }
    std::vector<FlowDef>::iterator i
        = find_if(myFlows[det].begin(), myFlows[det].end(), matching_time_finder(time));
    if(i==myFlows[det].end()) {
        throw 1;
    }
    return (*i);
}

FlowDef
getOverallFlow(int time, vector<string> dets)
{
    FlowDef ret;
    ret.qKFZ = 0;
    ret.qLKW = 0;
    ret.vLKW = 0;
    ret.vPKW = 0;
    ret.isLKW = 0;
    ret.time = time;
    for(vector<string>::iterator i=dets.begin(); i!=dets.end(); ++i) {
        try {
            FlowDef tmp = getOverallFlow(time, (*i));
            ret.vLKW = ((ret.vLKW * ret.qLKW) + (tmp.vLKW * tmp.qLKW)) / (ret.qLKW + tmp.qLKW);
            ret.vPKW = ((ret.vPKW * (ret.qKFZ-ret.qLKW)) + (tmp.vPKW * (tmp.qKFZ-tmp.qLKW))) / ((ret.qKFZ-ret.qLKW) + (tmp.qKFZ-tmp.qLKW));
            ret.qLKW += tmp.qLKW;
            ret.qKFZ += tmp.qKFZ;
        } catch (...) {
        }
    }
    return ret;
}


class desc_p_by_length_sorter {
public:
    /// constructor
    explicit desc_p_by_length_sorter() { }

    int operator() (const RouteDesc *p1, const RouteDesc *p2) {
        return p1->edges2Pass.size()<p2->edges2Pass.size();
    }
};


string
tryFindDestinationDetector(string edge)
{
    if(edge2det.find(edge)!=edge2det.end()) {
        return edge2det[edge];
    }
    return "";
}


void
buildFlowsForDetector(const std::string &det)
{
    string src = detmap[det];
    // collect detectors for this edge
    std::vector<std::string> detectors;
    {
        for(std::map<std::string, std::string>::iterator i=detmap.begin(); i!=detmap.end(); ++i) {
            string ndet = (*i).first;
            if(sources.find(ndet)!=sources.end()) {
                string nedge = (*i).second;
                if(nedge==src) {
                    detectors.push_back(ndet);
                }
            }
        }
    }
    // collect routes that contain this detector
    std::vector<RouteDesc*> descs;
    {
        for(std::vector<RouteDesc>::iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
            if((*i).edges2Pass[0]==src) {
                descs.push_back(&(*i));
            }
        }
        sort(descs.begin(), descs.end(), desc_p_by_length_sorter());
    }
    // ok, have all detectors and all edges; what next?
    int beg = beginTime * 60;
    int end = endTime * 60;
    for(int time=beg; time<end; time++) {
        // get the flow to insert
        FlowDef fd;
        fd.qKFZ = 0;
        fd.qLKW = 0;
        fd.isLKW = 0;
        try {
            fd = getOverallFlow(time, detectors);
        } catch(...) {
        }
        if(fd.qKFZ==0) {
            continue;
        }
        SUMOReal duration = 0;
        SUMOReal speed1 = fd.vPKW;
        string firstEdge = "";
        for(std::vector<RouteDesc*>::iterator ri = descs.begin(); ri!=descs.end(); ++ri) {
            std::vector<std::string> edges = (*ri)->edges2Pass;
            std::vector<string>::iterator ei = edges.begin();
            if(firstEdge!="") {
                ei = find(edges.begin(), edges.end(), firstEdge);
                if(ei==edges.end()) {
                    ei = edges.begin();
                    duration = 0;
                }
            }
            for(; ei!=edges.end()-1; ++ei) {
                string edge = (*ei);
                SUMOReal speed2 = speedmap[edge];
                SUMOReal speed = speed1<speed2 ? speed1 : speed2;
                SUMOReal length = lengthmap[edge];
                duration += (length/speed);
            }
            firstEdge = *(ei-1);
            string nextDet = tryFindDestinationDetector(*ei);
            FlowDef out;
            out.qKFZ = 0;
            out.qLKW = 0;
            out.vLKW = 0;
            out.vPKW = 0;
            /*
            out.qKFZ = fd.qKFZ / 10.;
            out.qLKW = fd.qLKW / 10.;
            out.vLKW = fd.vLKW;
            out.vPKW = fd.vPKW;
            out.isLKW = 0;
            */
            if(nextDet!="") {
                try {
                    out = getOverallFlow(time, nextDet);
                } catch(...) {
                }
            }
            /*
            fd.qKFZ -= out.qKFZ;
            fd.qLKW -= out.qLKW;
            */
            if((*ri)->q.size()==0) {
                (*ri)->q.reserve(1440);
                (*ri)->fLKW.reserve(1440);
                (*ri)->isLKW.reserve(1440);
                for(int i=0; i<1440; i++) {
                    (*ri)->q.push_back(0);
                    (*ri)->fLKW.push_back(0);
                    (*ri)->isLKW.push_back(0);
                }
            }
            if(out.qKFZ<0) {
                out = getOverallFlow(time, nextDet);
            }
            assert(time>=0&&time<1440);
            assert(out.qKFZ>=0);
            (*ri)->q[time] = out.qKFZ;
            (*ri)->fLKW[time] = out.qLKW / out.qKFZ;
        }
    }
}

std::vector<RouteDesc*>
collectRoutesForEdge(string edge)
{
    std::vector<RouteDesc*> descs;
    {
        for(std::vector<RouteDesc>::iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
            if((*i).edges2Pass[0]==edge) {
                descs.push_back(&(*i));
            }
        }
    }
    return descs;
}

/*
std::vector<RouteDesc*>
collectRoutesForDetector(string det)
{
    std::vector<RouteDesc*> descs;
    {
        for(std::vector<RouteDesc>::iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
            if((*i).det==det) {
                descs.push_back(&(*i));
            }
        }
    }
    return descs;
}
*/

template<class T>
class dist
{
public:
    dist() : myProb(0) { }
    ~dist() { }

    void add(SUMOReal prob, T val) {
        assert(prob>=0);
        myVals.push_back(val);
        myProbs.push_back(prob);
        myProb += prob;
    }

    T get() const {
        SUMOReal prob = (SUMOReal) (((SUMOReal) rand()/(SUMOReal) RAND_MAX) * myProb);
        for(int i=0; i<myVals.size(); i++) {
            if(prob<myProbs[i]) {
                return myVals[i];
            }
            prob -= myProbs[i];
        }
        return myVals[myVals.size()-1];
    }

    SUMOReal getOverallProb() const {
        return myProb;
    }

private:
    SUMOReal myProb;
    std::vector<T> myVals;
    std::vector<SUMOReal> myProbs;

};

std::vector<string>
collectSourceDetectorsOn(const string &edge)
{
    std::vector<string> ret;
    for(std::map<std::string, std::string>::iterator i=sources.begin(); i!=sources.end(); ++i) {
        if(detmap[(*i).first]==edge) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}


bool testOnly = false;

int
main(int argc, char **argv)
{
    if(argc<8) {
        printf("SyntaxError!!!\n");
	    printf("Syntax: wjt_routesFromDets(2).exe <DETEKTORLISTE> <KANTENZUORDNUNGEN> <SUMONETZ> <DETEKTORWERTE> <ZIELORDNER> <BEGINTIME> <ENDTIME>\n");
        return 1;
    }
    //
    beginTime = TplConvert<char>::_2int(argv[7]);
    endTime = TplConvert<char>::_2int(argv[8]);
    //
    cout << "Loading map" << endl;
    readMap(argv[1]);
    cout << "Loading net" << endl;
    readNet(argv[2]);
    cout << "Loading detectors" << endl;
    readDetectors(argv[3]);
    cout << "Loading id map" << endl;
    readIDMap(argv[4]);
    cout << "Loading flows" << endl;
    readFlows(argv[5]);
    // check detectors
    cout << "Computing detector types" << endl;
    {
        std::map<std::string, std::string>::iterator i;
        for(i=detmap.begin(); i!=detmap.end(); ++i) {
            string edge = detmap[(*i).first];
            string det = (*i).first;
            //cout << det << ": ";
            if(isSource(edge, std::vector<std::string>(), (*i).first)) {
                sources[det] = edge;
        	    //cout << "source, ";
            }
            if(isDestination(edge, std::vector<std::string>(), (*i).first)) {
                destinations[det] = edge;
        	    //cout << "destination, ";
            }
            //cout << endl;
        }
    }
    // check highway detectors
    {
        std::map<std::string, std::string>::iterator i;
        for(i=detmap.begin(); i!=detmap.end(); ++i) {
            string edge = detmap[(*i).first];
            string det = (*i).first;
            if(isHighwaySource(edge, std::vector<std::string>(), (*i).first)) {
                hsources[det] = edge;
            }
        }
    }
    cout << "Building detector routes" << endl;
    // generate routes starting at sources
    {
        if(testOnly) {
            buildRoutesForDetector("12");
        } else {
            std::map<std::string, std::string>::iterator i;
            int index = 0;
            for(i=sources.begin(); i!=sources.end(); ++i, ++index) {
                buildRoutesForDetector((*i).first);
                //cout << (*i).first << "( " << index << " ) " << myRoutes.size() << endl;
            }
        }
    }

    cout << "Building detector flows" << endl;
    // build flows
    {
        if(testOnly) {
            buildFlowsForDetector("12");
        } else {
            std::map<std::string, std::string>::iterator i;
            int index = 0;
            for(i=sources.begin(); i!=sources.end(); ++i, ++index) {
                cout << (*i).first << endl;
                buildFlowsForDetector((*i).first);
                //cout << (*i).first << "( " << index << " ) " << myRoutes.size() << endl;
            }
        }
    }

    cout << "Writing emitter and routes" << endl;
    // print routes
        // write selected
    {
        ofstream strm1("selected.txt");
        std::vector<RouteDesc>::iterator i;
        for(i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
            std::vector<std::string> route = (*i).edges2Pass;
            std::vector<std::string>::iterator j;
            for(j=route.begin(); j!=route.end(); ++j) {
                strm1 << "lane:" << (*j) << "_0" << endl;
            }
        }
    }

    dist<int> vehSpeedDist;
    std::vector<string> lkwTypes;
    std::vector<string> pkwTypes;
    {
        // set vehicle distributions
        vehSpeedDist.add(0.025, 0);
        vehSpeedDist.add(0.05, 1);
        vehSpeedDist.add(0.1, 2);
        vehSpeedDist.add(0.2, 3);
        vehSpeedDist.add(0.25, 4);
        vehSpeedDist.add(0.2, 5);
        vehSpeedDist.add(0.1, 6);
        vehSpeedDist.add(0.05, 7);
        vehSpeedDist.add(0.025, 8);

        lkwTypes.push_back("uaLKW_70");
        lkwTypes.push_back("uaLKW_75");
        lkwTypes.push_back("uaLKW_80");
        lkwTypes.push_back("uaLKW_85");
        lkwTypes.push_back("uaLKW_90");
        lkwTypes.push_back("uaLKW_95");
        lkwTypes.push_back("uaLKW_100");
        lkwTypes.push_back("uaLKW_105");
        lkwTypes.push_back("uaLKW_110");

        pkwTypes.push_back("uaPKW_90");
        pkwTypes.push_back("uaPKW_100");
        pkwTypes.push_back("uaPKW_110");
        pkwTypes.push_back("uaPKW_120");
        pkwTypes.push_back("uaPKW_130");
        pkwTypes.push_back("uaPKW_140");
        pkwTypes.push_back("uaPKW_150");
        pkwTypes.push_back("uaPKW_160");
        pkwTypes.push_back("uaPKW_170");
    }


        // write routes
    string destPath = argv[6];
    {
        cout << "Destination is '" << (destPath + "/ua_routes.xml").c_str() << "'." << endl;
        std::map<std::string, std::vector<std::string> > routes;
        ofstream strm1((destPath + "/ua_routes.xml").c_str());
        strm1 << "<routes>" << endl;
        ofstream strm3a((destPath + "/ua_sources_meso.xml").c_str());
        strm3a << "<add>" << endl;
        ofstream strm3b((destPath + "/ua_sources_micro.xml").c_str());
        strm3b << "<add>" << endl;
        // loop over detectors
        std::map<std::string, std::string>::iterator i;
        std::map<std::string, bool> done;
        for(i=sources.begin(); i!=sources.end(); ++i) {
            string cdet = (*i).first;
            string edge = detmap[(*i).first];
            if(done.find(edge)!=done.end()) {
                continue;
            }
            done[edge] = true;
            std::vector<RouteDesc*> droutes = collectRoutesForEdge(edge);
            std::vector<string> dets = collectSourceDetectorsOn(edge);
            sort(droutes.begin(), droutes.end(), desc_p_by_length_sorter());
            ofstream strm2((destPath + "/sources_meso/uameSource_" + (*i).first + ".xml").c_str());
            std::vector<RouteDesc*>::iterator j;
            std::vector<string>::iterator j2;
            strm2 << "   <triggeredsource>" << endl;
            strm2 << "      <routedist>" << endl;
            overallMeso[(*i).first] = 0;
            ofstream **strm2mi = new ofstream*[dets.size()];
            int srcDist = 0;
            for(j2=dets.begin(); j2!=dets.end(); ++j2, ++srcDist) {
                strm2mi[srcDist] = new ofstream((destPath + "/sources_micro/uamiSource_" + *j2 + ".xml").c_str());
                (*strm2mi[srcDist]) << "   <triggeredsource>" << endl;
                (*strm2mi[srcDist]) << "      <routedist>" << endl;
                overallMicro[*j2] = 0;
            }
            for(j=droutes.begin(); j!=droutes.end(); ++j) {
                std::vector<std::string> route = (*j)->edges2Pass;
                if((*j)->routename=="") {
                    string routename =
                        "ua_from-" + *(route.begin()) + "-to-" + *(route.end()-1);
                    while(routes.find(routename)!=routes.end()&&routes.find(routename)->second!=route) {
                        routename += "_2";
                    }
                    routes[routename] = route;
                    (*j)->routename = routename;
                }
                strm2 << "         <routedistelem routeid=\""
                    << (*j)->routename << "\" frequency=\"1\"/>" << endl;
                for(srcDist=0; srcDist<dets.size(); ++srcDist) {
                    (*strm2mi[srcDist]) << "         <routedistelem routeid=\""
                        << (*j)->routename << "\" frequency=\"1\"/>" << endl;
                }
                // write route
                strm1 << "   <route multi_ref=\"x\" id=\""
                    << (*j)->routename << "\">";
                std::vector<std::string>::iterator k;
                for(k=route.begin(); k!=route.end(); ++k) {
                    if(k!=route.begin()) {
                        strm1 << ' ';
                    }
                    strm1 << (*k);
                }
                strm1 << "</route>" << endl;
            }
            strm2 << "      </routedist>" << endl;
            for(srcDist=0; srcDist<dets.size(); ++srcDist) {
                (*strm2mi[srcDist]) << "      </routedist>" << endl;
            }
            // write vehicles
                // loop over time
            int running = 0;
            int beg = beginTime * 60;
            int end = endTime * 60;
            int ltime = beg;
            for(int time=beg; time<end; time++) {
                FlowDef srcFD;
                srcFD.qKFZ = 0;
                try {
                    srcFD = getOverallFlow(time, dets);
                } catch(...) {
                }

                int no = srcFD.qKFZ;//destDist.getOverallProb();
                if(no==0) {
                    cout << "No vehicle for detector '" << (*i).first
                        << "' time " << time << endl;
                    continue;
                }
                // prepare the destinations
                dist<int> destDist;
                int destIndex=0;
                for(j=droutes.begin(); j!=droutes.end(); ++j, ++destIndex) {
                    destDist.add((*j)->q[time+(*j)->duration], destIndex);
                }
                // prepare the sources
                dist<int> srcDist;
                int srcIndex = 0;
                int got = 0;
                for(j2=dets.begin(); j2!=dets.end(); ++j2, ++srcIndex) {
                    try {
                        srcDist.add(getOverallFlow(time, (*j2)).qKFZ, srcIndex);
                        got++;
                    } catch(...) {
                        /*
                        cout << "Warning: no flow for detector '" << *j2
                            << "' time " << time << endl;
                            */
                    }
                }
                if(got!=dets.size()&&srcDist.getOverallProb()==0) {
                    /*
                    cout << "Warning: no flows defined for detector '" << (*i).first
                        << "'" << endl;
                        */
                    srcDist.add(1, 0);
                }
                //
                for(int car=0; car<no; ++car) {
                    destIndex = destDist.get();
                    srcIndex = srcDist.get();
                    std::vector<std::string> route = droutes[destIndex]->edges2Pass;
                    string type;
                    SUMOReal v;
        			if(droutes[destIndex]->isLKW[time]>1) {
				        droutes[destIndex]->isLKW[time] = droutes[destIndex]->isLKW[time] - 1.;
				        type = lkwTypes[vehSpeedDist.get()];
				        v = srcFD.vLKW;//droutes[destIndex]->fd[time].vLKW;
			        } else {
				        type = pkwTypes[vehSpeedDist.get()];
                        v = srcFD.vPKW;//v = droutes[destIndex]->fd[time].vPKW;
			        }
                    if(v<=0) {
                        v = 100;
                    } else if(v>=180) {
                        v = 100;
                    }
                    v = v / 3.6;

                    int ctime = time * 60 + (60. * (SUMOReal) car / (SUMOReal) no);
                    /*
                    if(ctime<=ltime) {
                        ctime = ltime + 1;
                    }
                    ltime = ctime;*/
                    // !!! micro-dectectore einzeln
                    strm2 << "   <emit id=\"ua_" <<
                        (*i).first << "_" << running++  << "\""
                        << " time=\"" << ctime << "\""
                        << " speed=\"" << v << "\""
                        << " route=\"" << droutes[destIndex]->routename << "\""
                        << " vehtype=\"" << type << "\"/>" << endl;
                    (*strm2mi[srcIndex]) << "   <emit id=\"ua_" <<
                        (*i).first << "_" << running++  << "\""
                        << " time=\"" << ctime << "\""
                        << " speed=\"" << v << "\""
                        << " route=\"" << droutes[destIndex]->routename << "\""
                        << " vehtype=\"" << type << "\"/>" << endl;
                    overallMeso[(*i).first] = overallMeso[(*i).first] + 1;
                    overallMicro[dets[srcIndex]] = overallMicro[dets[srcIndex]] + 1;

        			droutes[destIndex]->isLKW[time] +=
                        droutes[destIndex]->fLKW[time];
                }
            }
            strm2 << "</triggeredsource>" << endl;
            for(srcDist=0; srcDist<dets.size(); ++srcDist) {
                (*strm2mi[srcDist]) << "</triggeredsource>" << endl;
                delete strm2mi[srcDist];
            }
            delete[] strm2mi;
            if(overallMeso[(*i).first]!=0) {
                SUMOReal rpos = det2pos[(*i).first];
                if(rpos>lengthmap[detmap[(*i).first]]) {
                    cout << "Warning; Patching detector's '" << (*i).first << "' position from "
                        << rpos << " to " << lengthmap[detmap[(*i).first]] << endl;
                    rpos = lengthmap[detmap[(*i).first]];
                }
                // write that sources shall be used
                strm3a << "   <trigger id=\"uameSource_"
                    <<  (*i).first
                    << "\" objecttype=\"emitter\" "
                    << "pos=\"" << rpos << "\" "
                    << "objectid=\"" << detmap[(*i).first] << "_" << det2lane[(*i).first] << "\" "
                    << "file=\"sources_meso/uameSource_"
                    << (*i).first << ".xml\"/>" << endl;
            }
            for(j2=dets.begin(); j2!=dets.end(); ++j2, ++srcDist) {
                SUMOReal rpos = det2pos[*j2];
                if(rpos>lengthmap[detmap[*j2]]) {
                    cout << "Warning; Patching detector's '" << *j2 << "' position from "
                        << rpos << " to " << lengthmap[detmap[*j2]] << endl;
                    rpos = lengthmap[detmap[*j2]];
                }
                if(overallMicro[*j2]!=0) {
                    strm3b << "   <source id=\"uamiSource_" <<  *j2
                        << "\" objecttype=\"emitter\" "
                        << "pos=\"" << rpos << "\" "
                        << "objectid=\"" << detmap[*j2] << "_" << det2lane[*j2] << "\" "
                        << "file=\"sources_micro/uamiSource_"
                        << *j2 << ".xml\"/>" << endl;
                }
            }
        }
        strm1 << "</routes>" << endl;
        strm3a << "</add>" << endl;
        strm3b << "</add>" << endl;
    }
    return 0;
}

