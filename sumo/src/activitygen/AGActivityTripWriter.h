#ifndef AGACTIVITYTRIPWRITER_H
#define AGACTIVITYTRIPWRITER_H

#include "activities/AGTrip.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>

class AGActivityTripWriter
{
public:
	AGActivityTripWriter(std::string file) :
		fileName(file),
		routes(file.c_str())
	{
		initialize();
	}

	void initialize();
	void addTrip(AGTrip trip);
	void writeOutputFile();

private:
	std::string fileName;
	std::ofstream routes;
	std::map<std::string, std::string> colors;

	void vtypes();
};


#endif
