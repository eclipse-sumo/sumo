#include <iostream>
#include <fstream>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/UtilExceptions.h>
#include <microsim/MSJunction.h>
#include "SUMOFrame.h"

using namespace std;

OptionsCont *
SUMOFrame::getOptions()
{
    OptionsCont *oc = new OptionsCont();
    // register the file i/o options
    oc->doRegister("net-files", 'n', new Option_FileName());
    oc->doRegister("route-files", 'r', new Option_FileName());
    oc->doRegister("junction-files", 'j', new Option_FileName());
    oc->doRegister("detector-files", 'd', new Option_FileName());
    oc->doRegister("output-file", 'o', new Option_FileName(""));
    oc->doRegister("configuration-file", 'c', new Option_FileName("sumo.cfg"));
    oc->doRegister("source-files", 's', new Option_FileName());
    oc->addSynonyme("net-files", "net");
    oc->addSynonyme("route-files", "routes");
    oc->addSynonyme("junction-files", "junctions");
    oc->addSynonyme("detector-files", "detectors");
    oc->addSynonyme("output-file", "output");
    oc->addSynonyme("configuration-file", "configuration");
    oc->addSynonyme("source-files", "sources");
    // register the simulation settings
    oc->doRegister("begin", 'b', new Option_Long());
    oc->doRegister("end", 'e', new Option_Long());
    // register the report options
    oc->doRegister("verbose", 'v', new Option_Bool(false));
    oc->doRegister("warn", 'w', new Option_Bool(true));
    oc->doRegister("print-options", 'p', new Option_Bool(false));
    oc->doRegister("help", new Option_Bool(false));
//    oc->doRegister("validate-nodes", new Option_Bool(false));
    // register some research options
    oc->doRegister("initial-density", new Option_Float());
    oc->doRegister("initial-speed", new Option_Float());
    // register the data processing options
    oc->doRegister("no-config", 'C', new Option_Bool(false));
    oc->addSynonyme("no-config", "no-configuration");
    oc->doRegister("no-raw", 'R', new Option_Bool(false));
    oc->addSynonyme("no-raw", "no-raw-output");
    oc->doRegister("dump-intervals", new Option_UIntVector(""));
    oc->doRegister("dump-basename", new Option_FileName());;
    // parse the command line arguments and configuration the file
    return oc;
}

/**
 * Builds the ostream used for raw data output
 */
ostream *
SUMOFrame::buildRawOutputStream(OptionsCont *oc) {
    if(oc->getBool("R"))
	    return 0;

    filebuf *fb = new filebuf; // !!! possible memory leak
    ostream *craw = new ostream( (oc->getString("o")=="") ?
	    cout.rdbuf() :
	    fb->open(oc->getString("o").c_str(), ios::out|ios::trunc));
    if(craw->rdbuf()!=fb)
	    delete fb;
    if(!craw->good()) {
        cout << "The output file '" << oc->getString("o")
            << "' could not be built." << endl;
        cout << "Simulation failed." << endl;
        throw ProcessError();
    }
    return craw;
}

void 
SUMOFrame::postbuild(MSNet &net)
{
    MSJunction::postloadInitContainer();
}

