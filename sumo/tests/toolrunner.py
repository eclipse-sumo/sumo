#!/usr/bin/env python
import os,subprocess,sys
subprocess.call(os.path.join(os.path.dirname(sys.argv[0]), "..", "tools", " ".join(sys.argv[1:])),
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
