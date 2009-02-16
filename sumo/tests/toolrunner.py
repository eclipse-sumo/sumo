#!/usr/bin/env python
import os,subprocess,sys
command = ""
if sys.argv[1].endswith(".jar"):
    command = "java -jar "
subprocess.call(command + os.path.join(os.path.dirname(sys.argv[0]), "..", "tools", " ".join(sys.argv[1:])),
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
