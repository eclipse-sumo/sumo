#ifndef FileWriter_h
#define FileWriter_h

#include <string>
#include <fstream>

class FileWriter
{
public:
    FileWriter(const std::string &file);
    ~FileWriter();
    void beginStep();
    void add(const std::string &str);
    void closeStep();
private:
    std::ofstream myFile;
};


#endif
