#!/bin/bash
netgen --grid --grid.number 2 --grid.attach-length 200 --default-junction-type traffic_light -o square.tmp.net.xml
python patchTrafficLights.py -n square.tmp.net.xml -o square/square.net.xml
rm square.tmp.net.xml

netgen --grid --grid.number 2 --grid.attach-length 100 --default-junction-type traffic_light -o square.tmp.net.xml
python patchTrafficLights.py -n square.tmp.net.xml -o kuehne/kuehne.net.xml
rm square.tmp.net.xml

netconvert -c cross/cross.netc.cfg
echo manually patch traffic lights in cross.net.xml:
echo replace tl-logic with those in cross/cross.tls.add.xml 

