#include <map>
#include <fstream>
#include <string>
#include "SharedOutputDevices.h"
#include "OutputDevice.h"
#include "OutputDevice_File.h"

using namespace std;


SharedOutputDevices *SharedOutputDevices::myInstance = 0;

SharedOutputDevices *
SharedOutputDevices::getInstance()
{
    if(myInstance==0) {
        myInstance = new SharedOutputDevices();
    }
    return myInstance;
}


SharedOutputDevices::~SharedOutputDevices()
{
    for(FileMap::iterator i=myOutputFiles.begin(); i!=myOutputFiles.end(); ++i) {
        delete (*i).second;
    }
    myOutputFiles.clear();
    myInstance = 0;
}


OutputDevice *
SharedOutputDevices::getOutputFile(const std::string &name)
{
    FileMap::iterator i = myOutputFiles.find(name);
    if(i!=myOutputFiles.end()) {
        return (*i).second;
    }
    std::ofstream *strm = new std::ofstream(name.c_str());
    OutputDevice *dev = new OutputDevice_File(strm);
    myOutputFiles[name] = dev;
    return dev;
}


SharedOutputDevices::SharedOutputDevices()
{
}


