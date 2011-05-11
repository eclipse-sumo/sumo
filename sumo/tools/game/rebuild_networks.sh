#!/bin/bash
netgen --grid --grid.number 2 --grid.attach-length 200 --default-junction-type traffic_light -o square.tmp.net.xml
python patchTrafficLights.py -n square.tmp.net.xml -o square/square.net.xml
rm square.tmp.net.xml

netconvert -c cross/cross.netc.cfg
python patchTrafficLights.py -n cross.tmp.net.xml -o cross/cross.net.xml
rm cross.tmp.net.xml
