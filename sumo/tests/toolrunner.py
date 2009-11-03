#!/usr/bin/env python
import os,subprocess,sys
tool = [os.path.join(os.path.dirname(sys.argv[0]), "..", sys.argv[-1])]
if tool[0].endswith(".jar"):
    tool = ["java", "-jar"] + tool
subprocess.call(tool+sys.argv[1:-1], shell=(os.name=="nt"),
                stdout=sys.stdout, stderr=sys.stderr)
