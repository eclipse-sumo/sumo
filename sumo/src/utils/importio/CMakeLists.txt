message("entering importio")

set(importio_files
	LineReader.cpp
	NamedColumnsParser.cpp
	LineReader.h
	NamedColumnsParser.h)

add_library(utimportio STATIC ${importio_files})
