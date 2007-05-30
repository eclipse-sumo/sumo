#!/usr/bin/python
import os
for root, dirs, files in os.walk("../../src/"):
    for name in files:
        if name.endswith(".h") or name.endswith(".cpp"):
            os.system("astyle --style=kr -U -l -n " + os.path.join(root, name))
        for ignoreDir in ['.svn', 'foreign', 'itm-remoteserver']:
            if ignoreDir in dirs:
                dirs.remove(ignoreDir)
