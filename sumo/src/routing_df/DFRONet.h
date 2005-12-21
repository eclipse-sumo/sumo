#pragma once
#include "d:\prjnet\sumo\sumo\src\router\ronet.h"
#include <utils/options/OptionsCont.h>

class DFRONet 

{
public:
	DFRONet( RONet * ro );
	DFRONet();
	~DFRONet();
	void getApproach();
private:
	RONet * ro;
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

};
