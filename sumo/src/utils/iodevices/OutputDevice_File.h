#ifndef OutputDevice_File_h
#define OutputDevice_File_h

#include <fstream>
#include "OutputDevice.h"

class OutputDevice_File : public OutputDevice {
public:
    OutputDevice_File(std::ofstream *strm);
    ~OutputDevice_File();
    bool ok();
    void close();

    std::ostream &getOStream();

private:
    std::ofstream *myFileStream;

};

#endif
