SUBDIRS = src docs

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

doc:
	$(MAKE) -C docs $@

dist-hook:
	cd $(srcdir)
	cp --parents `find build/msvc6 -name "*.dsw" -o -name "*.dsp"` $(distdir)
	cp --parents `find build -name "*.vcproj" -o -name "*.sln"` $(distdir)
	cp --parents `find data tests -regex ".*\.\(nod\|edg\|rou\|add\|def\)\.xml"` $(distdir)
	cp --parents `find data tests -name "*.cfg"` $(distdir)
	cp --parents `find data tests -name .svn -prune -o -regex ".*/\(errors\|output\|options\|testsuite\)\..*" -print` $(distdir)
	cp --parents `find tools -regex ".*\.\(pl\|py\|java\|jar\)"` $(distdir)
	cp tools/traceExporter/.classpath tools/traceExporter/.project tools/traceExporter/manifest.mf $(distdir)/tools/traceExporter

