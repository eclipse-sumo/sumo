#!/bin/bash
java -cp ../../../../bin/TraaS.jar de.tudresden.ws.WebService &
sleep 5
wsimport -clientjar webservice.jar "http://127.0.0.1:4223/SUMO?wsdl"
javac -cp webservice.jar../../../../bin/TraaS.jar: examples/Main.java
