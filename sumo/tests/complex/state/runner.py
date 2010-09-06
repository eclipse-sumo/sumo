#!/usr/bin/env python
import os,subprocess,sys

idx = sys.argv.index(":")
saveParams = sys.argv[1:idx]
loadParams = sys.argv[idx+1:]

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin', 'sumo'))
subprocess.call([sumoBinary]+saveParams,
                shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
subprocess.call([sumoBinary]+loadParams,
                shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
