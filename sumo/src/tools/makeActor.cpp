#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <sstream>
#include <set>
#include <vector>
#include <cmath>

#include <utils/geom/Line2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>

#pragma warning(disable :4996)
#pragma warning(disable :4786)
using namespace std;

struct tolseg{
	string bpoint;
	string epoint;
	vector<string> edges;
};

map<string, set<string> > mcedge;
map<string, set<string> > cmedge;

/*
float
distance(std::pair<float, float> p1, std::pair<float, float> p2)
{
    return sqrt(
        (p1.first-p2.first)*(p1.first-p2.first)
        +
        (p1.second-p2.second)*(p1.second-p2.second));

}
*/

Position2D
getPosition(string line, string c1, string c2)
{
    size_t pos = line.find(c1);
    size_t beg = line.find("\"", pos);
    size_t end = line.find("\"", beg+1);
    string valC1 = line.substr(beg+1, end-beg-1);
    pos = line.find(c2);
    beg = line.find("\"", pos);
    end = line.find("\"", beg+1);
    string valC2 = line.substr(beg+1, end-beg-1);
    return Position2D(atof(valC1.c_str()), atof(valC2.c_str()));
}

string
getXML(string line, string what)
{
    size_t beg = line.find(what);
    beg = line.find("\"", beg);
    size_t end = line.find("\"", beg+1);
    return line.substr(beg+1, end-beg-1);
}


bool
haveConnection(const vector<string>::iterator &begIt,
               const string &endI)
{
    vector<string>::iterator c = begIt;
    do {
        string firstE = *c;
        string afE = firstE + "-AddedOffRampEdge";
        if(mcedge.find(afE)!=mcedge.end()) {
            firstE = afE;
        }
        string nextE = *(c+1);
        string anE = nextE + "-AddedOnRampEdge";
        if(mcedge.find(anE)!=mcedge.end()) {
            nextE = anE;
        }
        if(mcedge[firstE].find(nextE)==mcedge[firstE].end()) {
            return false;
        }
        c++;
    } while(*(c)!=endI);
    return true;
}


bool
inverseHaveConnection(const vector<string>::iterator &begIt,
                      const string &endI)
{
    vector<string>::iterator c = begIt;
    do {
        string firstE = "-" + *c;
        string afE = firstE + "-AddedOffRampEdge";
        if(mcedge.find(afE)!=mcedge.end()) {
            firstE = afE;
        }
        string nextE = "-" + *(c+1);
        string anE = nextE + "-AddedOnRampEdge";
        if(mcedge.find(anE)!=mcedge.end()) {
            nextE = anE;
        }
        if(mcedge.find(firstE)==mcedge.end()) {
            return false;
        }
        if(mcedge[firstE].find(nextE)==mcedge[firstE].end()) {
            return false;
        }
        c++;
    } while(*(c)!=endI);
    return true;
}


template <class T>
inline std::string toString( const T& t ) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


string
findNearest(std::map<std::string, Position2DVector> &emap, const Position2D &p)
{
    double bestD = -1;
    string bestE = "";
    for(std::map<std::string, Position2DVector>::iterator i=emap.begin(); i!=emap.end(); ++i) {
        string edge = (*i).first;
        const Position2DVector &v = (*i).second;
        for(size_t j=0; j<v.size()-1; ++j) {
            Line2D l = v.lineAt(j);
            double d = l.distanceTo(p);
            if(d>=0&&(bestD<0||d<bestD)) {
                bestE = (*i).first;
                bestD = d;
            }
        }
    }
    return bestE;
}


bool
hasAny(const vector<string> &edges)
{
    vector<string>::const_iterator c;
    for(c=edges.begin(); c!=edges.end()-1; ++c) {
        if(mcedge.find(*c)!=mcedge.end()) {
            return true;
        }
        if(mcedge.find("-" + *c)!=mcedge.end()) {
            return true;
        }
    }
    return false;
}


vector<string>
buildDirectedEdges(const vector<string> &edges)
{
    vector<string> ret;
    vector<string>::const_iterator c;
    string lastE;
    for(c=edges.begin(); c!=edges.end()-1; ++c) {
        bool found = false;


        // check: no '-' on from, no '-' on to
        if(!found) {
            string firstE = *c;
            string afE = firstE + "-AddedOffRampEdge";
            if(mcedge.find(afE)!=mcedge.end()) {
                firstE = afE;
            }
            string nextE = *(c+1);
            string anE = nextE + "-AddedOnRampEdge";
            if(mcedge.find(anE)!=mcedge.end()) {
                nextE = anE;
            }
            if(mcedge[firstE].find(nextE)!=mcedge[firstE].end()) {
                found = true;
                ret.push_back(firstE);
                lastE = nextE;
            }
        }
        // check: no '-' on from, a '-' on to
        if(!found) {
            string firstE = *c;
            string afE = firstE + "-AddedOffRampEdge";
            if(mcedge.find(afE)!=mcedge.end()) {
                firstE = afE;
            }
            string nextE = "-" + *(c+1);
            string anE = nextE + "-AddedOnRampEdge";
            if(mcedge.find(anE)!=mcedge.end()) {
                nextE = anE;
            }
            if(mcedge[firstE].find(nextE)!=mcedge[firstE].end()) {
                found = true;
                ret.push_back(firstE);
                lastE = nextE;
            }
        }
        // check: a '-' on from, a '-' on to
        if(!found) {
            string firstE = "-" + *c;
            string afE = firstE + "-AddedOffRampEdge";
            if(mcedge.find(afE)!=mcedge.end()) {
                firstE = afE;
            }
            string nextE = "-" + *(c+1);
            string anE = nextE + "-AddedOnRampEdge";
            if(mcedge.find(anE)!=mcedge.end()) {
                nextE = anE;
            }
            if(mcedge[firstE].find(nextE)!=mcedge[firstE].end()) {
                found = true;
                ret.push_back(firstE);
                lastE = nextE;
            }
        }
        // check: a '-' on from, no '-' on to
        if(!found) {
            string firstE = "-" + *c;
            string afE = firstE + "-AddedOffRampEdge";
            if(mcedge.find(afE)!=mcedge.end()) {
                firstE = afE;
            }
            string nextE = *(c+1);
            string anE = nextE + "-AddedOnRampEdge";
            if(mcedge.find(anE)!=mcedge.end()) {
                nextE = anE;
            }
            if(mcedge[firstE].find(nextE)!=mcedge[firstE].end()) {
                found = true;
                ret.push_back(firstE);
                lastE = nextE;
            }
        }

        if(!found) {
            cout << "No connection found between '" << *c << "' and '" << *(c+1) << "'." << endl;
            return ret;
        }
    }
    ret.push_back(lastE);
    return ret;
}

string
tryFindBegEdge(const map<string, Position2DVector> &edgeShapes, 
               const string &beginAt,
               const Position2D &begPointPos,
               bool first);


string
tryFindEndEdge(const map<string, Position2DVector> &edgeShapes, 
               const string &beginAt,
               const Position2D &endPointPos,
               bool first)
{
    string current = beginAt;
    while(mcedge.find(current)!=mcedge.end()) {
        const set<string> &nextEdges = mcedge.find(current)->second;
        if(nextEdges.size()==0) {
            return "";
        }
        if(nextEdges.size()!=1) {
            if(first) {
                return tryFindBegEdge(edgeShapes, beginAt, endPointPos, false);
            }
            return "";
        }
        current = *(nextEdges.begin());
        const Position2DVector &v = edgeShapes.find(current)->second;
        for(size_t j=0; j<v.size()-1; ++j) {
            Line2D l = v.lineAt(j);
            double d = l.distanceTo(endPointPos);
            if(d>=0) {
                return current;
            }
        }
    }
    return "";
}



string
tryFindBegEdge(const map<string, Position2DVector> &edgeShapes, 
               const string &beginAt,
               const Position2D &begPointPos,
               bool first)
{
    string current = beginAt;
    while(cmedge.find(current)!=cmedge.end()) {
        const set<string> &nextEdges = cmedge.find(current)->second;
        if(nextEdges.size()==0) {
            return "";
        }
        if(nextEdges.size()!=1) {
            if(first) {
                return tryFindEndEdge(edgeShapes, beginAt, begPointPos, false);
            }
            return "";
        }
        current = *(nextEdges.begin());
        const Position2DVector &v = edgeShapes.find(current)->second;
        for(size_t j=0; j<v.size()-1; ++j) {
            Line2D l = v.lineAt(j);
            double d = l.distanceTo(begPointPos);
            if(d>=0) {
                return current;
            }
        }
    }
    return "";
}




int main(int ac, char * av[]){
	ifstream fin, fnet, fla, fseg, ftol;
	ofstream fout;
	if(ac<7){
		cout << "no input-file-name given\n";
		cout << "pointcollection netfile la2cell segmente tolpoints"<<endl;
		return -1;
	}

    ofstream logF("log.txt");

	fout.open("sumo.add.xml", ios_base::out);

    /*erstmal aus der netzdatei alle edges und die anzahl der lanes raus schreiben.
	desweiteren werden in mcedge die folgekanten gespeichert*/
    cout << "Parsing network '" << av[2] << "'." << endl;
	fnet.open(av[2], ios_base::in);
	if(!fnet.is_open()){
		cout << "cant open file " << av[2] << endl;
        logF << "cant open file " << av[2] << endl;
		return -2;
	}
	map<string, pair< string, double> > medge;
    std::map<string, Position2D> edgeBegins;
    std::map<string, Position2D> edgeEnds;
    std::map<string, Position2DVector> edgeShapes;
	string bline, tmp, id, nolanes, slength;
	double length=0, length_offset=0;
	int count=0;
	char foo[512];
	while(!fnet.eof()){
		getline(fnet, bline);
		if(bline.size()==0)
			continue;
		/*for (unsigned int i=0; i<bline.size(); i++){
		if (bline.at(i)==' ')
		bline.erase(i,1);
		}*/
		while ( bline.at(0)==' ' ){
			bline.erase(0,1);

		}
		if( strcmp(bline.substr(0,6).c_str(), "<edge ")==0 ){
			strcpy(foo, bline.c_str());
			strtok(foo, "\"");
			id=strtok(NULL, "\"");
			strtok(NULL, "\"");
			length=atof(strtok(NULL, "\""));
			strtok(NULL, "\""); strtok(NULL, "\"");
			strtok(NULL, "\""); strtok(NULL, "\"");
			strtok(NULL, "\"");
			nolanes=strtok(NULL, "\"");
    		medge[id]=make_pair(nolanes, length);
			++count;

            //
            edgeBegins[id] = getPosition(bline, "XFrom", "YFrom");
            edgeEnds[id] = getPosition(bline, "XTo", "YTo");
		}
		else if( strcmp(bline.substr(0,7).c_str(), "<cedge ")==0 ){
			strcpy(foo, bline.c_str());
			strtok(foo, "\"");
            string next = strtok(NULL, "\"");
			mcedge[id].insert(next);
            cmedge[next].insert(id);
		}
		else if( strcmp(bline.substr(0,6).c_str(), "<lane ")==0 ){
            size_t beg = bline.find('>');
            size_t end = bline.find('<', beg);
            string shapeS = bline.substr(beg+1, end-beg-1);
            Position2DVector shape = GeomConvHelper::parseShape(shapeS);
            edgeShapes[id] = shape;
		}
	}

	cout << "count medge="<< medge.size()<< "\t\t"<<count << endl;
    logF << "count medge="<< medge.size()<< "\t\t"<<count << endl;


    // Parse the la2cells
    cout << "Parsing la2cell '" << av[3] << "'." << endl;
	fla.open(av[3], ios_base::in);
	if(!fla.is_open()){
		cout << "cant open file " << av[3] << endl;
        logF << "cant open file " << av[3] << endl;
		return -2;
	}
	map<string, string> mla2cell;
	map<string, string>::iterator itla;
	string laid;

	while(!fla.eof()){
		getline(fla, bline);
		if(bline.size()==0)
			continue;
		strcpy(foo, bline.c_str());
		id=strtok(foo, " ");
		laid=strtok(NULL, "\n");
		mla2cell[id]=laid;
	}

	/*schreibe die add-datei*/
	fout << "<additional>"<<endl;
	/*zu erst die la/cell aktoren*/
	string lane, pos, to, objid, la;
    string offid, onid;

    vector<string> cells;

    // At first, build actors for street/cell crossings
    //
	/*lese die pointcoll von elmar ein und erzeuge fuer jeden schnittpunkt einen aktor*/
    cout << "Parsing cell/street crossings '" << av[1] << "'." << endl;
	fin.open(av[1], ios_base::in);
	if(!fin.is_open()){
		cout << "cant open file " << av[1] << endl;
        logF << "cant open file " << av[1] << endl;
		return -2;
	}
	while(!fin.eof()){
		getline(fin, bline);
		if(bline.size()==0 || strcmp(bline.substr(0,1).c_str() , "#")==0)
			continue;
		strcpy(foo, bline.c_str());

		string l = strtok(foo, "\t");
		string type = strtok(NULL, "\t");
        if(type!="10") {
            continue;
        }
		la=strtok(NULL, ";");
		to=strtok(NULL, ";");
        if(find(cells.begin(), cells.end(), to)==cells.end())
            cells.push_back(to);
		pos=strtok(NULL, ";");
		pos=strtok(NULL, ";");
		pos=strtok(NULL, ";");
		pos=pos.substr(8, pos.length()-8);
		strtok(NULL,"\t");	strtok(NULL,"\t");
		id=strtok(NULL, "\n");

        if(medge.find(id)==medge.end())
            continue;

        if(to=="-565028875") {
            int bla = 0;
        }
        // get the position on the edge
		length=medge[id].second;
        onid = id+"-AddedOnRampEdge";
        bool posFound = false;
        // check whether it is lying on the on-ramp
        if(medge.find(onid)!=medge.end()){
            length_offset = medge[onid].second;
            if( atof(pos.c_str())>length_offset){
                // actor lies beyond the added on ramp
                pos = toString(atof(pos.c_str())-length_offset);
            } else { // the actor is on the addedonrampedge
                id = onid;
                posFound = true;
            }
        }
        // check whether it is laying on the edge
        if(!posFound) {
            if(atof(pos.c_str())<=length) {
                posFound = true;
            } else {
                pos = toString(atof(pos.c_str())-length);
            }
        }
        // check whether it is lying on the off-ramp
        if(!posFound) {
            offid= id+"-AddedOffRampEdge";
            if(medge.find(offid)!=medge.end()){
                length_offset = medge[offid].second;
                if(atof(pos.c_str())<=(length_offset)){
                    id = offid;
                    posFound = true;
                }
            }
        }
        if(!posFound) {
            pos="-1";
            cout << "edge " << id << " is too short" << endl;
            logF  << "edge " << id << " is too short" << endl;
        }

        count=atoi(medge[id].first.c_str());
		if(count==0)
			continue;//cout << id << "_" << count << endl;
		itla=mla2cell.find(la);
		if ( itla!=mla2cell.end() )
			laid = itla->second;
		else
			laid = "0";
		for(int i=0; i!=count;i++){
			//cout << id << "_" << i << endl;
			objid=id+"_"+itoa(i,foo,10);
			tmp=objid+"-"+to+"-"+laid;
			fout <<"\t<trigger objecttype=\"vehicle_actor\" id=\""<<tmp;
			fout <<"\" objectid=\""<< objid << "\" pos=\"" << pos << "\" to=\"";
			fout << to << "\" xto=\"" << laid << "\" type=\"1\"/>" << endl;
		}
		//if(count==0)
		//	cout << id << "_" << count << endl;
	}


    fout << endl;

    /* ----------------------------------------
     * build actors for tol-sas
     * ---------------------------------------- */
	/*tol-abschnitt-actoren*/
	map<string, tolseg> mtol;
	map<string, tolseg>::iterator ittol;
	/*einlesen der tolpoints*/
    cout << "Parsing tol points '" << av[5] << "'." << endl;
	ftol.open(av[5], ios_base::in);
	if(!ftol.is_open()){
		cout << "cant open file " << av[5] << endl;
        logF << "cant open file " << av[5] << endl;
		return -2;
	}
	while(!ftol.eof()){
		getline(ftol, bline);
		if(bline.size()==0)
			continue;
		strcpy(foo, bline.c_str());
		tmp=strtok(foo, " ");
		to=strtok(NULL, " ");
		pos=strtok(NULL, "\n");
		ittol=mtol.find(tmp);
		if(ittol==mtol.end()){
			tolseg ts;
			mtol[tmp] = ts;
			ittol=mtol.find(tmp);
		}
		if( strcmp(pos.c_str(), "0")==0)
			ittol->second.bpoint = to;
		else
			ittol->second.epoint = to;
	}
	/*einlesen der tolsegmente*/
    cout << "Parsing tol segments '" << av[4] << "'." << endl;
	fseg.open(av[4], ios_base::in);
	if(!ftol.is_open()){
		cout << "cant open file " << av[4] << endl;
        logF << "cant open file " << av[4] << endl;
		return -2;
	}
    std::vector<std::string> missingKnown;
    std::vector<std::string> missingUnknown;
	while(!fseg.eof()){
		getline(fseg, bline);
		if(bline.size()==0)
			continue;
		strcpy(foo, bline.c_str());
		tmp=strtok(foo, ";");
		to=strtok(NULL, ";");
		ittol=mtol.find(tmp);
		if(ittol==mtol.end()){
            if(medge.find(to)==medge.end()) {
                if(find(missingUnknown.begin(), missingUnknown.end(), tmp)==missingUnknown.end()) {
                    missingUnknown.push_back(tmp);
                }
            } else {
                if(find(missingKnown.begin(), missingKnown.end(), tmp)==missingKnown.end()) {
                    missingKnown.push_back(tmp);
                }
            }
			continue;
		}
		ittol->second.edges.push_back(to);
	}
    cout << "Number of missing sa-ids lying on known edges: " << missingKnown.size() << endl;
    cout << "Number of missing sa-ids lying on unknown edges: " << missingUnknown.size() << endl;
    logF << "Number of missing sa-ids lying on known edges: " << missingKnown.size() << endl;
    logF << "Number of missing sa-ids lying on unknown edges: " << missingUnknown.size() << endl;


	/*einlesen der positionen*/
    cout << "Parsing tol positionen '" << av[6] << "'." << endl;
	ifstream fpos(av[6], ios_base::in);
	if(!fpos.is_open()){
		cout << "cant open file " << av[6] << endl;
        logF << "cant open file " << av[6] << endl;
		return -2;
	}
    std::map<string, Position2D> poses;
    getline(fpos, bline);
	while(!fpos.eof()){
		getline(fpos, bline);
		if(bline.size()==0)
			continue;
        if(bline.find("<poi ")==string::npos)
            continue;
        string point_id = getXML(bline, "id=");
        string xS = getXML(bline, "x=");
        string yS = getXML(bline, "y=");
        float x = atof(xS.c_str());
        float y = atof(yS.c_str());
        if(point_id=="17579") {
            int bla = 0;
        }
        poses[point_id] = Position2D(x, y);
    }

	/*jetzt suchen wir fuer jeden tolpoint eine kante aus der netdatei*/
	string edge, be1, be2, ee1, ee2; // beginedge==be endedge==ee
    size_t bla = mtol.size();
	for(ittol=mtol.begin(); ittol!=mtol.end(); ittol++){

        if(!hasAny(ittol->second.edges)) {
            if(poses.find(ittol->second.bpoint)!=poses.end() || poses.find(ittol->second.epoint)!=poses.end()) {
                cout << "At least one point lies within area, but no edge exists (" << ittol->second.bpoint << "/" << ittol->second.epoint << ")" << endl;
                logF << "At least one point lies within area, but no edge exists (" << ittol->second.bpoint << "/" << ittol->second.epoint << ")" << endl;
            }
            continue;
        }
        vector<string> nEdges = buildDirectedEdges(ittol->second.edges);
        /*da wir in der net die richtung durch ein - angezeigt ist muessen wir erst eraus-
		finden welche richting wir nehmen*/
        string bla = ittol->first;
        string beginEdge1 = "";
        vector<string>::iterator begIt = nEdges.begin();
        while(begIt!=nEdges.end()&&medge.find(*begIt)==medge.end()) {
            ++begIt;
        }
        if(begIt!=nEdges.end()) {
            beginEdge1 = *begIt;
        }

        string endEdge1 = "";
        vector<string>::reverse_iterator endIt = nEdges.rbegin();
        while(endIt!=nEdges.rend()&&medge.find(*endIt)==medge.end()) {
            ++endIt;
        }
        if(endIt!=nEdges.rend()) {
            endEdge1 = *endIt;
        }

        if(beginEdge1==""||endEdge1=="") {
            if(beginEdge1!=""||endEdge1!="") {
                cout << "ups - found only one edge (" << beginEdge1 << "/" << endEdge1 << "); what to do?" << endl;
                logF << "ups - found only one edge (" << beginEdge1 << "/" << endEdge1 << "); what to do?" << endl;
            }
            continue;
        }

        string beginPoint = ittol->second.bpoint;
        string endPoint = ittol->second.epoint;


        if(beginPoint=="17579") {
            int bla = 0;
        }

        vector<string>::iterator it;
        std::map<string, Position2DVector> shapes;
        for(it=nEdges.begin(); it!=nEdges.end(); ++it) {
            string edge = *it;
            /*
            if(!keepDir) {
                edge = "-" + edge;
            }
            */
            if(edgeShapes.find(edge)!=edgeShapes.end()) {
                shapes[edge] = edgeShapes[edge];
            } else {
                cout << "No shape for edge " << edge << endl;
                logF << "No shape for edge " << edge << endl;
            }
        }

        Position2D begPointPos = poses[beginPoint];
        Position2D endPointPos = poses[endPoint];

        if(beginPoint=="13854") {
            int bla = 0;
        }
        if(beginPoint=="17579") {
            int bla = 0;
        }


        string beginEdge2 = findNearest(shapes, begPointPos);
        string endEdge2 = findNearest(shapes, endPointPos);

        if(beginEdge2=="") {
            cout << " Begin edge (points: " << beginPoint << "/" << endPoint << ") is beyond the sa" << endl;
            logF << " Begin edge (points: " << beginPoint << "/" << endPoint << ") is beyond the sa" << endl;
            beginEdge2 = *nEdges.begin();
            beginEdge2 = tryFindBegEdge(edgeShapes, beginEdge2, begPointPos, true);
            if(beginEdge2=="") {
                cout << "  Could not find begin edge (points: " << beginPoint << "/" << endPoint << ")" << endl;
                logF << "  Could not find begin edge (points: " << beginPoint << "/" << endPoint << ")" << endl;
            } else {
                cout << "  Found " << beginEdge2 << endl;
                logF << "  Found " << beginEdge2 << endl;
            }
        }
        if(endEdge2=="") {
            cout << " End edge (points: " << beginPoint << "/" << endPoint << ") is beyond the sa" << endl;
            logF << " End edge (points: " << beginPoint << "/" << endPoint << ") is beyond the sa" << endl;
            endEdge2 = *(nEdges.end()-1);
            endEdge2 = tryFindEndEdge(edgeShapes, endEdge2, endPointPos, true);
            if(endEdge2=="") {
                cout << "  Could not find end edge (points: " << beginPoint << "/" << endPoint << ")" << endl;
                logF << "  Could not find end edge (points: " << beginPoint << "/" << endPoint << ")" << endl;
            } else {
                cout << "  Found " << endEdge2 << endl;
                logF << "  Found " << endEdge2 << endl;
            }
        }
        // !!! beinhaltet noch keine off/on-ramps

        // write begin position
        if(beginEdge2!="") {
            Position2DVector shape = edgeShapes[beginEdge2];
            SUMOReal pos = shape.nearest_position_on_line_to_point(begPointPos);
            int count=atoi(medge[beginEdge2].first.c_str());
		    for(int i=0; i!=count;i++){
                fout <<"\t<trigger objecttype=\"vehicle_actor\" id=\"" << beginPoint << '_' << i
	        	    <<"\" objectid=\"" << beginEdge2  << '_' << i << "\" pos=\"" << pos << "\" to=\""
		        	<< beginPoint << "\" xto=\"" << -1 << "\" type=\"2\"/>" << endl;
            }
        }
        // write end position
        if(endEdge2!="") {
            Position2DVector shape = edgeShapes[endEdge2];
            SUMOReal pos = shape.nearest_position_on_line_to_point(endPointPos);
            int count=atoi(medge[endEdge2].first.c_str());
		    for(int i=0; i!=count;i++){
                fout <<"\t<trigger objecttype=\"vehicle_actor\" id=\"" << endPoint << '_' << i
	        	    <<"\" objectid=\"" << endEdge2  << '_' << i << "\" pos=\"" << pos << "\" to=\""
		        	<< endPoint << "\" xto=\"" << -1 << "\" type=\"2\"/>" << endl;
            }
        }


        //

        //

        /*
        cout << "Definition: " << endl
            << " ID: " << ittol->first << " (" << beginPoint << "/" << endPoint << ")" << endl
            << " beginEdge: " << beginEdge2 << endl
            << " endEdge: " << endEdge2 << endl;
        if(beginEdge1!=beginEdge2) {
            cout << "Recheck beginEdge " << beginEdge1 << "<->" << beginEdge2 << endl;
        }
        if(endEdge1!=endEdge2) {
            cout << "Recheck endEdge " << endEdge1 << "<->" << endEdge2 << endl;
        }

        string beginEdge = beginEdge2;
        string endEdge = endEdge2;

        count=atoi(medge[beginEdge].first.c_str());
        if(count!=0){
            for( int i=0; i!=count; i++){
                objid=beginEdge+"_"+itoa(i, foo, 10);
                tmp=objid+"-"+ittol->first;
                fout <<"\t<trigger objecttype=\"vehicle_actor\" id=\""<<tmp;
			    fout <<"\" objectid=\""<< objid << "\" pos=\"0\" to=\"0\"";
			    fout <<" xto=\"0\" type=\"2\"/>"<< endl;
            }
        }
        count=atoi(medge[endEdge].first.c_str());
        if(count!=0){
            for( int i=0; i!=count; i++){
                objid=endEdge+"_"+itoa(i, foo, 10);
                tmp=objid+"-"+ittol->first;
                fout <<"\t<trigger objecttype=\"vehicle_actor\" id=\""<<tmp;
			    fout <<"\" objectid=\""<< objid << "\" pos=\"-1\" to=\"0\"";
			    fout <<" xto=\"0\" type=\"2\"/>"<< endl;
            }
        }
        */
	}


    /* ----------------------------------------
     * build static call amounts
     * ---------------------------------------- */
    cout << "Parsing static call statistics '" << av[7] << "'." << endl;
    fout << endl;
	ifstream fcs(av[7], ios_base::in);
	if(!fcs.is_open()){
		cout << "cant open file " << av[7] << endl;
        logF << "cant open file " << av[7] << endl;
		return -2;
	}
    getline(fcs, bline);
	while(!fcs.eof()){
        getline(fcs, bline);
        if(bline.length()<2)
            continue;
        // event_interval;cell_id;count_active;count_entries;count_exits;count_starts;count_ends;dur_mean;dur_stdev
        StringTokenizer st(bline, ";");
        int time = TplConvert<char>::_2int(st.next().c_str());
        int cell = TplConvert<char>::_2int(st.next().c_str());
        if(find(cells.begin(), cells.end(), toString(cell))==cells.end())
            continue;
        int no   = TplConvert<char>::_2int(st.next().c_str());
        fout << "\t<trigger objecttype=\"vehicle_actor\" id=\"" << cell
            <<"\" objectid=\"" << time  << "\" pos=\"" << no << "\" to=\""
            << -1 << "\" xto=\"" << -1 << "\" type=\"3\"/>" << endl;
    }
    fcs.close();

    /* ----------------------------------------
     * build moving call amounts/durations
     * ---------------------------------------- */
    cout << "Parsing dynamic call statistics '" << av[8] << "'." << endl;
    fout << endl;
	//fcs.open(av[8], ios_base::in);
    ifstream fcd(av[8], ios_base::in);
	if(!fcd.is_open()){
		cout << "cant open file " << av[8] << endl;
        logF << "cant open file " << av[8] << endl;
		return -2;
	}
    getline(fcd, bline);
	while(!fcd.eof()){
        getline(fcd, bline);
        if(bline.length()<2)
            continue;
        // event_interval;cell_id;count_active;count_entries;count_exits;count_starts;count_ends;dur_mean;dur_stdev
        StringTokenizer st(bline, ";");
        int time = TplConvert<char>::_2int(st.next().c_str());
        int cell = TplConvert<char>::_2int(st.next().c_str());
        if(find(cells.begin(), cells.end(), toString(cell))==cells.end())
            continue;
        int no   = TplConvert<char>::_2int(st.next().c_str());
        st.next(); // count_entries
        st.next(); // count_exits
        st.next(); // count_starts
        st.next(); // count_ends
        SUMOReal mean = TplConvert<char>::_2SUMOReal(st.next().c_str());
        SUMOReal dev  = TplConvert<char>::_2SUMOReal(st.next().c_str());
        fout << "\t<trigger objecttype=\"vehicle_actor\" id=\"" << cell
            <<"\" objectid=\"" << time  << "\" pos=\"" << no << "\" to=\""
            << mean << "\" xto=\"" << dev << "\" type=\"4\"/>" << endl;
    }

    fout << "</additional>"<<endl;
	fout.close();
	fin.close();
	fnet.close();
	return 0;
}