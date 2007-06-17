#!/usr/bin/python
import os.path, sys

versionFile = os.path.dirname(sys.argv[0]) + "/../../src/version.h"
svnRevision = "UNKNOWN"
entries = open(".svn/entries")
for i, l in enumerate(entries):
    if i == 3 and l.strip().isdigit():
        svnRevision = l.strip()
    revIndex = l.find('revision="')
    if revIndex >= 0:
        revIndex += 10
	svnRevision = l[revIndex:l.index('"', revIndex)]
	break
entries.close()

version = open(versionFile, 'w')
print >> version, '#define VERSION_STRING "dev-SVN-r%s"' % svnRevision
version.close()
