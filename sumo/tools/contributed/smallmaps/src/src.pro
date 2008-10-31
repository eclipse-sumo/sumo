SOURCES += smallmaps.cpp \
           main.cpp
HEADERS += smallmaps.h
TEMPLATE = app
CONFIG += release \
          warn_on \
	  thread \
          qt
TARGET = ../bin/smallmaps
