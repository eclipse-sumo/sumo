export PATH=$PWD/../../tools/assign:$PWD/../../tools/trip:$PATH
generateTripsXml.py -n input/net.net.xml -m input/mtx0708_histo.fma -d input/districts.xml -s 20
dua-iterate.py -n input/net.net.xml -t trips.trips.xml -v
