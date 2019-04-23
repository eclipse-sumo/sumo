#!/bin/bash
javac -cp ../dist/TraaS.jar java_library/APITest.java && java -cp java_library/:../dist/TraaS.jar APITest $@
