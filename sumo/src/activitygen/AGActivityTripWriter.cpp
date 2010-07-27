#include "AGActivityTripWriter.h"
//#include "city/AGTime.h"

using namespace std;

void
AGActivityTripWriter::initialize()
{
	routes << "<?xml version=\"1.0\"?>" << endl << endl;
	//TODO: variables
	routes << "<routes>" << endl;
	vtypes();
}

void
AGActivityTripWriter::vtypes()
{
	routes << "\t<vtype id=\"default\" accel=\"0.8\" decel=\"4.5\" sigma=\"0.5\" length=\"5\" maxspeed=\"70\"/>" << endl;
	routes << "\t<vtype id=\"bus\" accel=\"0.4\" decel=\"2.5\" sigma=\"0.5\" length=\"10\" maxspeed=\"70\"/>" << endl << endl;

	colors["default"] = "1,0,0";
	colors["bus"] = "0,1,0";
}

void
AGActivityTripWriter::addTrip(AGTrip trip)
{
	list<AGPosition>::iterator it;
	int time = (trip.getDay()-1)*86400 + trip.getTime();

	//the vehicle:
	routes << "\t<vehicle";
	routes << " id=\"" << trip.getVehicleName();
	routes << "\" type=\"" << trip.getType();
	routes << "\" depart=\"" << time;
	routes << "\" color=\"" << colors[trip.getType()];
	routes << "\">" << endl;

	//the route
	routes << "\t\t<route edges=\""; //TODO the exact position
	routes << trip.getDep().street->getName();
	for(it=trip.getPassed()->begin() ; it!=trip.getPassed()->end() ; ++it)
	{
		routes << " " << it->street->getName();
	}
	routes << " " << trip.getArr().street->getName();
	routes << "\" />" << endl;

	routes << "\t</vehicle>" << endl;
}

void
AGActivityTripWriter::writeOutputFile()
{
	routes << "</routes>" << endl;
	routes.close();
}

