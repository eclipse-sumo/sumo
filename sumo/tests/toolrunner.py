#!/usr/bin/env python
import os,subprocess,sys
if len(sys.argv) < 2:
    sys.exit('required argument <tool> missing')
tool = [os.path.join(os.path.dirname(sys.argv[0]), "..", sys.argv[-1])]
if tool[0].endswith(".jar"):
    tool = ["java", "-jar"] + tool

if tool[0].endswith(".py"):
    tool = [os.environ.get('PYTHON') or 'python'] + tool

import os
from sys import version_info
subprocess.call(tool+sys.argv[1:-1], shell=(os.name=="nt"),
                stdout=sys.stdout, stderr=sys.stderr)
