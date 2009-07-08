#!/usr/bin/env python
"""
@file    extractTest.py
@author  Michael.Behrisch@dlr.de
@date    2009-07-08
@version $Id$

Extract all files for a test case into a new dir.
It may copy more files than needed because it copies everything
that is mentioned in the config under copy_test_path.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
import optparse, os, glob, sys, shutil, subprocess
from os.path import join 

optParser = optparse.OptionParser(usage="%prog <options> <test directory>")
optParser.add_option("-o", "--output", default=".", help="send output to directory")
options, args = optParser.parse_args()
if len(args) == 0:
	optParser.error("No test directory given")

for leaf in args:	
	optionsFiles = glob.glob(join(leaf, "options.[0-9a-z]*"))
	if len(optionsFiles) != 1:
		print >> sys.stderr, "Not a unique options file in %s." % dir
		continue
	app = optionsFiles[0].split('.')[1]
	potentials = {}
	dir = leaf
	while True:
		for f in os.listdir(dir):
			path = join(dir, f)
			if not os.path.isdir(path) and not f in potentials:
				potentials[f] = path
		if dir == os.path.dirname(dir) or os.path.exists(join(dir, "config."+app)):
			break
		dir = os.path.dirname(dir)
	config = join(dir, "config."+app)
	if not os.path.exists(config):
		print >> sys.stderr, "Config not found for %s." % dir
		continue
	testPath = leaf[len(os.path.commonprefix([dir, leaf])):]
	testPath = join(options.output, testPath.replace(os.sep, '_'))
	if not os.path.exists(testPath):
		os.makedirs(testPath)
	for line in open(config):
		entry = line.strip().split(':')
		if entry and entry[0] == "copy_test_path" and entry[1] in potentials:
			shutil.copy2(potentials[entry[1]], testPath)
	options = open(optionsFiles[0]).read().split() + ['--save-configuration', 'test.%s.cfg' % app]
	oldWorkDir = os.getcwd()
	os.chdir(testPath)
	if os.name == "posix" and app != "sumo":
		subprocess.call(["sumo-" + app] + options)
	else:
		subprocess.call([app] + options)
	os.chdir(oldWorkDir)
