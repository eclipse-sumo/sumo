#ifndef SharedOutputDevices_h
#define SharedOutputDevices_h

#include <map>
#include <string>

class OutputDevice;

class SharedOutputDevices {
public:
    static SharedOutputDevices *getInstance();
    ~SharedOutputDevices();
    OutputDevice *getOutputFile(const std::string &name);

private:
    SharedOutputDevices();


private:
    static SharedOutputDevices *myInstance;

    typedef std::map<std::string, OutputDevice*> FileMap;
    FileMap myOutputFiles;

};

#endif
