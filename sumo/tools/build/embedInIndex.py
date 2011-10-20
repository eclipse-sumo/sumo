#!/usr/bin/env python
"""
@file    embedInIndex.py
@author  Daniel.Krajzewicz@dlr.de
@date    2011-10-20
@version $Id$

Reads all pages in "mirror" and subfolders and embeds their content
in "index.html". The result is written into "docs" preserving the 
directory structure.

Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, os.path, sys, shutil


DEST_FOLDER = "docs"

try: os.mkdir(DEST_FOLDER)
except: pass
try: os.mkdir(DEST_FOLDER + "/images")
except: pass

fd = open("index.html")
tpl = fd.read()
fd.close()


srcRoot = "mirror"
for root, dirs, files in os.walk(srcRoot):
	for name in files:
		t = root.replace("mirror", DEST_FOLDER)
		if name.endswith(".html"):
			fd = open(os.path.join(root, name))
			c = fd.read()
			fd.close()
			#
			if root.find('/')>=0:
				level = root.count("/")
			else:
				level = root.count("\\")
			level = "../" * level
			# title
			cc = tpl.replace("<title>", "<title>" + name[:name.rfind(".html")] + " - ")
			# css inclusion
			cc = cc.replace("sumo.css", level + "sumo.css")
			cc = cc.replace("logo.png", level + "logo.png")
			# links
			b = cc.find("<a href=")
			while b>=0:
				b = cc.find('"', b)
				if not cc[b:].startswith("http"):
					cc = cc[:b+1] + level + cc[b+1:]
				b = cc.find("<a href=", b)
			# content
			b = cc.find("<!-- content begins -->")
			e = cc.find("<!-- content ends -->", b)
			e = cc.find("<", e+1) - 1
			cc = cc[:b] + c + cc[e:]
			
			try: os.makedirs(t)
			except: pass
			fd = open(os.path.join(t, name), "w")
			fd.write(cc)
			fd.close()
		else:
			shutil.copy(os.path.join(root, name), os.path.join(t, name))
