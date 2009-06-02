set PATH=..\..\tools\assign;..\..\tools\trip;%PATH%
generateTripsXml.py -n input\net.net.xml -m input\mtx0708_histo.fma -d input\districts.xml -s 30
dua-iterate.py -n input\net.net.xml -t trips.trips.xml -d input\input_weights1.dump.xml -S 30 -o matrix.xml -s 0 -c ..\..\tools\assign\calibration\src -a 3600
