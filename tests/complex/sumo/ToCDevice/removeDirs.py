#!/usr/bin/python

import os
import shutil

#~ orig_fn = "/home/leo/repos/sumo_master/sumo/tests/complex/sumo/ToCDevice/spec/options.complex"
#~ orig_fn = "/home/leo/repos/sumo_master/sumo/tests/complex/sumo/ToCDevice/runner.py"
orig_fn = "/home/leo/repos/sumo_master/sumo/tests/complex/sumo/ToCDevice/input_routes.rou.xml"
#~ b = "options.complex"
#~ b = "runner.py"
b = "input_routes.rou.xml"

dl = os.listdir()
for n in dl:
    print (n)
    if(os.path.isdir(n)):
        d= os.path.join(os.path.realpath(n), b)
        shutil.copy(orig_fn, d)
