#include <string>
#include "FileWriter.h"


FileWriter::FileWriter(const std::string &file)
    : myFile(file.c_str())
{
}


FileWriter::~FileWriter()
{
    myFile.close();
}


void
FileWriter::beginStep()
{
}


void
FileWriter::add(const std::string &str)
{
    myFile << str;
}


void
FileWriter::closeStep()
{
    myFile << endl;
}


