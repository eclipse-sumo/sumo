message("entering iodevices")

set(iodevices_files
	OutputDevice.cpp OutputDevice.h
	BinaryFormatter.cpp BinaryFormatter.h
	BinaryInputDevice.cpp BinaryInputDevice.h
	OutputDevice_CERR.cpp OutputDevice_CERR.h
	OutputDevice_COUT.cpp OutputDevice_COUT.h
	OutputDevice_File.cpp OutputDevice_File.h
	OutputDevice_String.cpp OutputDevice_String.h
	OutputDevice_Network.cpp OutputDevice_Network.h
	OutputFormatter.h
	PlainXMLFormatter.cpp PlainXMLFormatter.h)

add_library(utiodevices STATIC ${iodevices_files})
